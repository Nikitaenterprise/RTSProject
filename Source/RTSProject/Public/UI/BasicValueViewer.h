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
	void SetValue(const ValueType& Value);
};

template<typename ValueType>
void UBasicValueViewer::SetValue(const ValueType& Value)
{
	TextBox->Text = FText::FromString(FString(Value));
}

template<>
inline void UBasicValueViewer::SetValue<float>(const float& Value)
{
	TextBox->Text = FText::FromString(FString::SanitizeFloat(Value, 0));
}

template<>
inline void UBasicValueViewer::SetValue<int32>(const int32& Value)
{
	TextBox->Text = FText::FromString(FString::SanitizeFloat(Value, 0));
}
