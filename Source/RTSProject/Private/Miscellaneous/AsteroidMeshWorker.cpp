#include "Miscellaneous/AsteroidMeshWorker.h"
#include "Actors/AsteroidResource.h"
#include "Windows/WindowsPlatformProcess.h"
#include <functional>
#include "Kismet/KismetMathLibrary.h"
#include "MeshProcessingPlugin/Public/MeshProcessingPlugin.h"
#include "Generators/SphereGenerator.h"
#include "Engine/World.h"

FAsteroidMeshWorker* FAsteroidMeshWorker::Runnable = nullptr;

FAsteroidMeshWorker::FAsteroidMeshWorker(AAsteroidResource* ThatAsteroidResource,
		TArray<ADynamicSDMCActor*>& ThatSpawnedAsteroids) :
StopTaskCounter(0)
{
	AsteroidResource = ThatAsteroidResource;
	SpawnedAsteroids = &ThatSpawnedAsteroids;
	Thread = FRunnableThread::Create(this, TEXT("FAsteroidMeshWorker"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}
 
FAsteroidMeshWorker::~FAsteroidMeshWorker()
{
	delete Thread;
	Thread = nullptr;
}

bool FAsteroidMeshWorker::Init()
{
	if (!FModuleManager::Get().IsModuleLoaded("MeshProcessingPlugin"))
	{
		return false;
	}
	AsteroidsToModify = (*SpawnedAsteroids).Num();
	return true;
}

uint32 FAsteroidMeshWorker::Run()
{	
	for (uint32 Index = 0; Index < AsteroidsToModify; Index++)
	{
		const auto Mesh = (*SpawnedAsteroids)[Index]->GetMeshComponent()->GetMesh();
		// generate new mesh
		FSphereGenerator SphereGen;
		SphereGen.NumPhi = SphereGen.NumTheta = FMath::Clamp((*SpawnedAsteroids)[Index]->TessellationLevel, 3, 50);
		SphereGen.Radius = (*SpawnedAsteroids)[Index]->MinimumRadius +
			(*SpawnedAsteroids)[Index]->VariableRadius * FMathd::Sin(UKismetMathLibrary::RandomFloat());
		(*Mesh).Copy(&SphereGen.Generate());
		//(*SpawnedAsteroids)[Index]->RegenerateMesh();
		// if(UKismetMathLibrary::RandomBool())
		// {
		// 	AsteroidResource->SubtractMesh((*SpawnedAsteroids)[Index]);
		// }
		// else
		// {
		// 	AsteroidResource->UnionWithMesh((*SpawnedAsteroids)[Index]);
		// }
		TotalNumberOfModifiedAsteroids++;
		//prevent thread from using too many resources
		FWindowsPlatformProcess::Sleep(0.005);
	} 
	return 0;
}

void FAsteroidMeshWorker::Stop()
{
	StopTaskCounter.Increment();
}

FAsteroidMeshWorker* FAsteroidMeshWorker::AsteroidMeshWorker(AAsteroidResource* AsteroidResource,
	TArray<ADynamicSDMCActor*>& SpawnedAsteroids)
{
	// Create new instance of thread if it does not exist
	// and the platform supports multi threading!
	if (!Runnable && FWindowsPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FAsteroidMeshWorker(AsteroidResource, SpawnedAsteroids);			
	}
	return Runnable;
}
 
void FAsteroidMeshWorker::EnsureCompletion()
{
	Stop();
	Thread->WaitForCompletion();
}
 
void FAsteroidMeshWorker::Shutdown()
{
	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable;
		Runnable = nullptr;
	}
}
 
bool FAsteroidMeshWorker::IsThreadFinished()
{
	if(Runnable) return Runnable->IsFinished();
	return true;
}