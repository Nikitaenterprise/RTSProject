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
	USimpleDynamicMeshComponent* SDMComponent = nullptr;
public:
	ADynamicSDMCActor();

	USimpleDynamicMeshComponent* GetSDMComponent() const { return SDMComponent; }

protected:
	virtual void OnMeshEditedInternal() override;
	virtual void UpdateSDMCMesh();
};
