#include "DynamicSDMCActor.h"
#include "DynamicMesh3.h"

ADynamicSDMCActor::ADynamicSDMCActor()
{
	SDMComponent = CreateDefaultSubobject<USimpleDynamicMeshComponent>(TEXT("MeshComponent"), false);
	SetRootComponent(SDMComponent);
}

void ADynamicSDMCActor::OnMeshEditedInternal()
{
	UpdateSDMCMesh();
	Super::OnMeshEditedInternal();
}

void ADynamicSDMCActor::UpdateSDMCMesh()
{
	if (SDMComponent)
	{
		*(SDMComponent->GetMesh()) = SourceMesh;
		SDMComponent->NotifyMeshUpdated();

		// update material on new section
		UMaterialInterface* UseMaterial = (this->Material != nullptr) ? this->Material : UMaterial::GetDefaultMaterial(MD_Surface);
		SDMComponent->SetMaterial(0, UseMaterial);
	}
}

