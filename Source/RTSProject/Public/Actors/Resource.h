#pragma once

#include "GameFramework/Actor.h"
#include "Resource.generated.h"

class UStaticMeshComponent;
class UResourceSourceAttributeSet;

UENUM()
enum class EResourceType : uint8
{
	Asteroid UMETA(DisplayName = "Asteroid"),
	None UMETA(DisplayName = "None"),
	
	Num // Total
};

UCLASS()
class RTSPROJECT_API AResource : public AActor
{
	GENERATED_BODY()
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* StaticMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	UResourceSourceAttributeSet* ResourceSourceAttributeSet = nullptr;

protected:
	
	EResourceType ResourceType;

public:

	AResource();
	float Consume(float Amount);

protected:

	virtual void BeginPlay() override;
};
