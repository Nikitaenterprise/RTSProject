#pragma once

#include "UObject/Interface.h"
#include "Selectable.generated.h"

UINTERFACE(MinimalAPI)
class USelectable : public UInterface
{
	GENERATED_BODY()
};

class RTSPROJECT_API ISelectable
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Selected(bool bIsSelected);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interface")
	void Highlighted(bool bIsHighlighted);
};