#pragma once

#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"

class AAsteroidResource;
class ADynamicSDMCActor;
class FDynamicMesh3;

class RTSPROJECT_API FAsteroidMeshWorker : public FRunnable
{
	static FAsteroidMeshWorker* Runnable;
	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;
	/** The Data Ptr */
	AAsteroidResource* AsteroidResource = nullptr;
	TArray<ADynamicSDMCActor*>* SpawnedAsteroids = nullptr;
	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;
	
	uint32 AsteroidsToModify;
	uint32 TotalNumberOfModifiedAsteroids = 0;
public:
	bool IsFinished() const
	{
		return TotalNumberOfModifiedAsteroids >= AsteroidsToModify;
	}
	
	//~~~ Thread Core Functions ~~~
	//Constructor / Destructor
	FAsteroidMeshWorker(AAsteroidResource* ThatAsteroidResource,
		TArray<ADynamicSDMCActor*>& ThatSpawnedAsteroids);
	virtual ~FAsteroidMeshWorker();

	// Begin FRunnable interface.
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	// End FRunnable interface
    
	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();
    
	//~~~ Starting and Stopping Thread ~~~
	/* 
		Start the thread and the worker from static (easy access)! 
		This code ensures only 1 Prime Number thread will be able to run at a time. 
		This function returns a handle to the newly started instance.
	*/
	static FAsteroidMeshWorker* AsteroidMeshWorker(AAsteroidResource* AsteroidResource,
		TArray<ADynamicSDMCActor*>& SpawnedAsteroids);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

	static bool IsThreadFinished();
};
