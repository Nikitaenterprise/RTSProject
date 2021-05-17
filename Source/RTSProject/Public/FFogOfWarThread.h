#pragma once

#include "CoreMinimal.h"

class AFogOfWar;

class RTSPROJECT_API FFogOfWarThread : public FRunnable
{
public:
	//Thread to run the FRunnable on
	FRunnableThread* Thread;

	//Pointer to our manager
	AFogOfWar* Manager;

	//Thread safe counter
	FThreadSafeCounter StopTaskCounter;
	bool bShouldUpdate = false;
	bool bIsWriteUnFog = false;
	bool bIsWriteFow = false;
	bool bIsWriteTerraIncognita = false;
	bool bCheckActorIsInTerraIncognita = false;//Bool, is the actor in terra incognita territory

public:

	FFogOfWarThread(AFogOfWar* manager);
	virtual ~FFogOfWarThread();

	//FRunnable interface
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;

	//Method to perform work
	void UpdateFowTexture();
	void ShutDown();
};
