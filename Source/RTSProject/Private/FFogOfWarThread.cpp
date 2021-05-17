#include "FFogOfWarThread.h"
#include "FogOfWar.h"
#include "FogOfWarComponent.h"

FFogOfWarThread::FFogOfWarThread(AFogOfWar* manager)
{
	Manager = manager;
	Thread = FRunnableThread::Create(this, TEXT("AFogOfWarWorker"), 0U, TPri_BelowNormal);
}

FFogOfWarThread::~FFogOfWarThread()
{
	delete Thread;
	Thread = nullptr;
}

void FFogOfWarThread::ShutDown()
{
	Stop();
	Thread->WaitForCompletion();
}

bool FFogOfWarThread::Init()
{
	if (Manager)
	{
		Manager->GetWorld()->GetFirstPlayerController()->ClientMessage("Fog of War worker thread started");
		return true;
	}
	return false;
}

uint32 FFogOfWarThread::Run()
{
	FPlatformProcess::Sleep(0.03f);
	while (StopTaskCounter.GetValue() == 0) 
	{
		//the compiler was complaining about the time variable not being initialized, so = 0.0f
		float Time = 0.0f;
		if (Manager && Manager->GetWorld()) Time = Manager->GetWorld()->TimeSeconds;
		if (!Manager->bHasFOWTextureUpdate) 
		{
			UpdateFowTexture();
			if (Manager && Manager->GetWorld()) 
			{
				Manager->fowUpdateTime = Manager->GetWorld()->TimeSince(Time);
			}
		}
		FPlatformProcess::Sleep(0.1f);
	}
	return 0;
}

void FFogOfWarThread::UpdateFowTexture()
{
	Manager->LastFrameTextureData = TArray<FColor>(Manager->TextureData);
	uint32 HalfTextureSize = Manager->TextureSize / 2;
	int SignedTextureSize = static_cast<int>(Manager->TextureSize); //For convenience....
	TSet<FVector2D> CurrentlyInSight;
	TSet<FVector2D> TexelsToBlur;
	int SightTexels = Manager->SightRange * Manager->SamplesPerMeter;
	float Dividend = 100.0f / Manager->SamplesPerMeter;

	for (auto& Actor : Manager->FowActors)
	{	
		if (StopTaskCounter.GetValue() != 0) return; // if you experience an occasional crash
		if (!Actor) return;
		FVector Position = Actor->GetActorLocation();
		//We divide by 100.0 because 1 texel equals 1 meter of visibility-data.
		int PosX = static_cast<int>(Position.X / Dividend) + HalfTextureSize;
		int PosY = static_cast<int>(Position.Y / Dividend) + HalfTextureSize;
		//float integerX, integerY;

		//FVector2D fractions = FVector2D(modf(position.X / 50.0f, &integerX), modf(position.Y / 50.0f, &integerY));
		FVector2D TextureSpacePos = FVector2D(PosX, PosY);
		int Size = static_cast<int>(Manager->TextureSize);

		FCollisionQueryParams QueryParams(FName(TEXT("FOW trace")), false, Actor);
		int HalfKernelSize = (Manager->blurKernelSize - 1) / 2;

		//Store the positions we want to blur
		for (int y = PosY - SightTexels - HalfKernelSize; y <= PosY + SightTexels + HalfKernelSize; y++) 
		{
			for (int x = PosX - SightTexels - HalfKernelSize; x <= PosX + SightTexels + HalfKernelSize; x++) 
			{
				if (x > 0 && x < Size && y > 0 && y < Size) 
				{
					TexelsToBlur.Add(FIntPoint(x, y));
				}
			}
		}
		
		//This is checking if the current actor is able to:
		//A. Fully unveil the texels, B. unveil FOW, C, Unveil Terra Incognita
		//Accessing the registerToFOW property Unfog boolean
		//I declared the .h file for RegisterToFOW
		//Dont forget the braces >()
		bIsWriteUnFog = Actor->FindComponentByClass<UFogOfWarComponent>()->bCanWriteUnFog;
		bIsWriteFow = Actor->FindComponentByClass<UFogOfWarComponent>()->bCanWriteFow;
		bIsWriteTerraIncognita = Actor->FindComponentByClass<UFogOfWarComponent>()->bCanWriteTerraIncognita;

		if (bIsWriteUnFog) 
		{
			//Unveil the positions our actors are currently looking at
			for (int y = PosY - SightTexels; y <= PosY + SightTexels; y++) 
			{
				for (int x = PosX - SightTexels; x <= PosX + SightTexels; x++) 
				{
					//Kernel for radial sight
					if (x > 0 && x < Size && y > 0 && y < Size) 
					{
						FVector2D currentTextureSpacePos = FVector2D(x, y);
						int length = static_cast<int>((TextureSpacePos - currentTextureSpacePos).Size());
						if (length <= SightTexels) 
						{
							//Position - SightTexels*Dividend
							FVector currentWorldSpacePos = FVector(
								(x - static_cast<int>(HalfTextureSize)) * Dividend,
								(y - static_cast<int>(HalfTextureSize)) * Dividend,
								Position.Z);
							
							//CONSIDER: This is NOT the most efficient way to do conditional unfogging. With long view distances and/or a lot of actors affecting the FOW-data
							//it would be preferrable to not trace against all the boundary points and internal texels/positions of the circle, but create and cache "rasterizations" of
							//viewing circles (using Bresenham's midpoint circle algorithm) for the needed sightranges, shift the circles to the actor's location
							//and just trace against the boundaries.
							//We would then use Manager->GetWorld()->LineTraceSingle() and find the first collision texel. Having found the nearest collision
							//for every ray we would unveil all the points between the collision and origo using Bresenham's Line-drawing algorithm.
							//However, the tracing doesn't seem like it takes much time at all (~0.02ms with four actors tracing circles of 18 texels each),
							//it's the blurring that chews CPU..

							if (!Manager->GetWorld()->LineTraceTestByChannel(Position, currentWorldSpacePos, ECC_WorldStatic, QueryParams)) 
							{
								//If the actor is able to affect terra incognita
								if (bIsWriteTerraIncognita) Manager->UnfoggedData[x + y * Manager->TextureSize] = true;
								//Store the positions we are currently seeing.
								CurrentlyInSight.Add(FVector2D(x, y));
							}
						}
					}
				}
			}
		}
		
		//Is the current actor marked for checking if is in terra incognita
		bCheckActorIsInTerraIncognita = Actor->FindComponentByClass<UFogOfWarComponent>()->bCheckActorIsInTerraIncognita;
		
		if (bCheckActorIsInTerraIncognita) 
		{
			//if the current position TextureSpacePosXY in the UnfoggedData bool array is false the actor is in the Terra Incognita
			if (Manager->UnfoggedData[TextureSpacePos.X + TextureSpacePos.Y * Manager->TextureSize] == false) 
			{
				Actor->FindComponentByClass<UFogOfWarComponent>()->bIsActorInTerraIncognita = true;
			}
			else 
			{
				Actor->FindComponentByClass<UFogOfWarComponent>()->bIsActorInTerraIncognita = false;
			}
		}
	}

	/*
	for (auto Itr(Manager->FowActors.CreateIterator()); Itr; Itr++) 
	{
		// if you experience an occasional crash
		if (StopTaskCounter.GetValue() != 0) return;
		//Find actor position
		if (!*Itr) return;
		FVector position = (*Itr)->GetActorLocation();

		//We divide by 100.0 because 1 texel equals 1 meter of visibility-data.
		int posX = (int)(position.X / Dividend) + HalfTextureSize;
		int posY = (int)(position.Y / Dividend) + HalfTextureSize;
		//float integerX, integerY;

		//FVector2D fractions = FVector2D(modf(position.X / 50.0f, &integerX), modf(position.Y / 50.0f, &integerY));
		FVector2D textureSpacePos = FVector2D(posX, posY);
		int size = (int)Manager->TextureSize;

		FCollisionQueryParams queryParams(FName(TEXT("FOW trace")), false, (*Itr));
		int halfKernelSize = (Manager->blurKernelSize - 1) / 2;

		//Store the positions we want to blur
		for (int y = posY - SightTexels - halfKernelSize; y <= posY + SightTexels + halfKernelSize; y++) {
			for (int x = posX - SightTexels - halfKernelSize; x <= posX + SightTexels + halfKernelSize; x++) {
				if (x > 0 && x < size && y > 0 && y < size) {
					TexelsToBlur.Add(FIntPoint(x, y));
				}
			}
		}

		//This is checking if the current actor is able to:
		//A. Fully unveil the texels, B. unveil FOW, C, Unveil Terra Incognita
		//Accessing the registerToFOW property Unfog boolean
		//I declared the .h file for RegisterToFOW
		//Dont forget the braces >()

		if (*Itr != nullptr) {
			bIsWriteUnFog = (*Itr)->FindComponentByClass<UFogOfWarComponent>()->bCanWriteUnFog;
			bIsWriteFow = (*Itr)->FindComponentByClass<UFogOfWarComponent>()->bCanWriteFow;
			bIsWriteTerraIncognita = (*Itr)->FindComponentByClass<UFogOfWarComponent>()->bCanWriteTerraIncognita;
		}



		if (bIsWriteUnFog) {
			//Unveil the positions our actors are currently looking at
			for (int y = posY - SightTexels; y <= posY + SightTexels; y++) {
				for (int x = posX - SightTexels; x <= posX + SightTexels; x++) {
					//Kernel for radial sight
					if (x > 0 && x < size && y > 0 && y < size) {
						FVector2D currentTextureSpacePos = FVector2D(x, y);
						int length = static_cast<int>((textureSpacePos - currentTextureSpacePos).Size());
						if (length <= SightTexels) {
							FVector currentWorldSpacePos = FVector(
								((x - static_cast<int>(HalfTextureSize))) * Dividend,
								((y - static_cast<int>(HalfTextureSize))) * Dividend,
								position.Z);

							//CONSIDER: This is NOT the most efficient way to do conditional unfogging. With long view distances and/or a lot of actors affecting the FOW-data
							//it would be preferrable to not trace against all the boundary points and internal texels/positions of the circle, but create and cache "rasterizations" of
							//viewing circles (using Bresenham's midpoint circle algorithm) for the needed sightranges, shift the circles to the actor's location
							//and just trace against the boundaries.
							//We would then use Manager->GetWorld()->LineTraceSingle() and find the first collision texel. Having found the nearest collision
							//for every ray we would unveil all the points between the collision and origo using Bresenham's Line-drawing algorithm.
							//However, the tracing doesn't seem like it takes much time at all (~0.02ms with four actors tracing circles of 18 texels each),
							//it's the blurring that chews CPU..

							if (!Manager->GetWorld()->LineTraceTestByChannel(position, currentWorldSpacePos, ECC_WorldStatic, queryParams)) {

								//Is the actor able to affect the terra incognita

								if (bIsWriteTerraIncognita) {
									//if the actor is able then
									//Unveil the positions we are currently seeing
									Manager->UnfoggedData[x + y * Manager->TextureSize] = true;
								}
								//Store the positions we are currently seeing.
								CurrentlyInSight.Add(FVector2D(x, y));

							}
						}
					}
				}
			}
		}

		//Is the current actor marked for checking if is in terra incognita

		if (*Itr != nullptr) {
			bCheckActorIsInTerraIncognita = (*Itr)->FindComponentByClass<UFogOfWarComponent>()->bCheckActorIsInTerraIncognita;
		}
		if (bCheckActorIsInTerraIncognita) {
			//if the current position textureSpacePosXY in the UnfoggedData bool array is false the actor is in the Terra Incognita
			if (Manager->UnfoggedData[textureSpacePos.X + textureSpacePos.Y * Manager->TextureSize] == false) {
				(*Itr)->FindComponentByClass<UFogOfWarComponent>()->bIsActorInTerraIncognita = true;

			}
			else {
				(*Itr)->FindComponentByClass<UFogOfWarComponent>()->bIsActorInTerraIncognita = false;
			}
		}

	}
	*/
	if (Manager->GetIsBlurEnabled()) 
	{
		//Horizontal blur pass
		int Offset = floorf(Manager->blurKernelSize / 2.0f);
		for (auto Itr(TexelsToBlur.CreateIterator()); Itr; ++Itr) 
		{
			int x = Itr->IntPoint().X;
			int y = Itr->IntPoint().Y;
			float Sum = 0;
			for (int i = 0; i < Manager->blurKernelSize; i++) 
			{
				int ShiftedIndex = i - Offset;
				if (x + ShiftedIndex >= 0 && x + ShiftedIndex <= SignedTextureSize - 1) 
				{
					if (Manager->UnfoggedData[x + ShiftedIndex + (y * SignedTextureSize)]) 
					{
						//If we are currently looking at a position, unveil it completely
						if (CurrentlyInSight.Contains(FVector2D(x + ShiftedIndex, y))) 
						{
							Sum += Manager->blurKernel[i] * 255;
						}
						//If this is a previously discovered position that we're not currently looking at, put it into a "shroud of darkness".
						else 
						{
							//sum += (Manager->blurKernel[i] * 100);
							Sum += Manager->blurKernel[i] * Manager->FowMaskColor; //i mod this to make the blurred color unveiled
						}
					}
				}
			}
			Manager->HorizontalBlurData[x + y * SignedTextureSize] = static_cast<uint8>(Sum);
		}

		//Vertical blur pass
		for (auto Itr(TexelsToBlur.CreateIterator()); Itr; ++Itr) 
		{
			int x = Itr->IntPoint().X;
			int y = Itr->IntPoint().Y;
			float Sum = 0;
			for (int i = 0; i < Manager->blurKernelSize; i++) 
			{
				int ShiftedIndex = i - Offset;
				if (y + ShiftedIndex >= 0 && y + ShiftedIndex <= SignedTextureSize - 1)
				{
					Sum += (Manager->blurKernel[i] * Manager->HorizontalBlurData[x + (y + ShiftedIndex) * SignedTextureSize]);
				}
			}
			Manager->TextureData[x + y * SignedTextureSize] = FColor(static_cast<uint8>(Sum), static_cast<uint8>(Sum), static_cast<uint8>(Sum), 255);
		}
	}
	else 
	{
		for (int y = 0; y < SignedTextureSize; y++) 
		{
			for (int x = 0; x < SignedTextureSize; x++) 
			{
				if (Manager->UnfoggedData[x + (y * SignedTextureSize)]) 
				{
					//If we are currently looking at a position, unveil it completely
					//if the vectors are inside de TSet
					if (CurrentlyInSight.Contains(FVector2D(x, y))) 
					{
						Manager->TextureData[x + y * SignedTextureSize] = FColor(Manager->UnfogColor, Manager->UnfogColor, Manager->UnfogColor, 255);

						if (Manager->bIsFowTimerEnabled) Manager->FOWArray[x + (y * SignedTextureSize)] = false;
					}
					//If this is a previously discovered position that we're not currently looking at, put it into a "shroud of darkness".
					else
					{
						//This line sets the color to black again in the textureData, sets the UnfoggedData to False
						Manager->TextureData[x + y * SignedTextureSize] = FColor(Manager->FowMaskColor, Manager->FowMaskColor, Manager->FowMaskColor, 255);

						if (Manager->bIsFowTimerEnabled) 
						{
							Manager->FOWArray[x + (y * SignedTextureSize)] = true;

							if (Manager->FOWTimeArray[x + y * SignedTextureSize] >= Manager->FowTimeLimit) 
							{
								//setting the color
								Manager->TextureData[x + y * SignedTextureSize] = FColor(0.0, 0.0, 0.0, 255.0);
								//from FOW to TerraIncognita
								Manager->UnfoggedData[x + (y * SignedTextureSize)] = false;
								//reset the value
								Manager->FOWArray[x + (y * SignedTextureSize)] = false;
							}
						}
					}
				}
			}
		}
	}

	Manager->bHasFOWTextureUpdate = true;
}


void FFogOfWarThread::Stop() {
	StopTaskCounter.Increment();
}


