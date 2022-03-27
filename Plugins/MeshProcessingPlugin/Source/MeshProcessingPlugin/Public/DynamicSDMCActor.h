#pragma once

#include "SimpleDynamicMeshComponent.h"
#include "DynamicMeshBaseActor.h"
#include "DynamicSDMCActor.generated.h"

class FDynamicMesh3;

UCLASS()
class MESHPROCESSINGPLUGIN_API ADynamicSDMCActor : public ADynamicMeshBaseActor
{
	GENERATED_BODY()
protected:
	
	UPROPERTY(VisibleAnywhere)
	USimpleDynamicMeshComponent* MeshComponent = nullptr;

public:
	
	ADynamicSDMCActor();

	USimpleDynamicMeshComponent* GetMeshComponent() const { return MeshComponent; }
	
protected:
	
	virtual void OnMeshEditedInternal() override;
	virtual void UpdateSDMCMesh();
};
