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
	bool isWriteUnFog = false;
	bool isWriteFow = false;
	bool isWriteTerraIncog = false;
	bool bCheckActorInTerraIncog = false;//Bool, is the actor in terra incognita territory
public:

	
	FFogOfWarThread();
	FFogOfWarThread(AFogOfWar* manager);
	virtual ~FFogOfWarThread();

	//FRunnable interface
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	//Method to perform work
	void UpdateFowTexture();
	void ShutDown();
};
