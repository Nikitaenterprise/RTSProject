#pragma once

#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "DynamicMeshBaseActor.h"
#include "DynamicSMCActor.generated.h"

UCLASS()
class MESHPROCESSINGPLUGIN_API ADynamicSMCActor : public ADynamicMeshBaseActor
{
	GENERATED_BODY()
protected:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY(Transient)
	UStaticMesh* StaticMesh = nullptr;

public:

	ADynamicSMCActor();
	UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }
	UStaticMesh* GetStaticMesh() const { return StaticMesh; }

protected:

	virtual void BeginPlay() override;
	virtual void PostLoad() override;
	virtual void PostActorCreated() override;
	
	/**
	 * ADynamicBaseActor API
	 */
	virtual void OnMeshEditedInternal() override;
	virtual void UpdateSMCMesh();
	
};
