#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BasicValueViewer.generated.h"

UCLASS()
class RTSPROJECT_API UBasicValueViewer : public UUserWidget
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* TextBox = nullptr;
    
public:
	template<typename ValueType>
	void SetText(const ValueType& Value);
};

template<typename ValueType>
void UBasicValueViewer::SetText(const ValueType& Value)
{
	if (TextBox)
	{
		TextBox->SetText(FText::FromString(FString(Value)));
	}
}

template<>
inline void UBasicValueViewer::SetText<float>(const float& Value)
{
	if (TextBox)
	{
		TextBox->SetText(FText::FromString(FString::SanitizeFloat(Value, 0)));
	}
}

template<>
inline void UBasicValueViewer::SetText<int32>(const int32& Value)
{
	if (TextBox)
	{
		TextBox->SetText(FText::FromString(FString::SanitizeFloat(Value, 0)));
	}
}
