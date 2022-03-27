#pragma once

#include "ProceduralMeshComponent.h"
#include "DynamicMeshBaseActor.h"
#include "DynamicPMCActor.generated.h"



UCLASS()
class MESHPROCESSINGPLUGIN_API ADynamicPMCActor : public ADynamicMeshBaseActor
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(VisibleAnywhere)
	UProceduralMeshComponent* MeshComponent = nullptr;

public:
	
	ADynamicPMCActor();
	UProceduralMeshComponent* GetMeshComponent() const { return MeshComponent; }
	
protected:
	
	/**
	 * ADynamicBaseActor API
	 */
	virtual void OnMeshEditedInternal() override;
	virtual void UpdatePMCMesh();

};
