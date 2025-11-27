#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/RL_Widget_Includes.h"
#include "Subsystem/RL_Subsystem.h"

#include "RL_Widget_Params.generated.h"

UCLASS()
class RUNTIMELOGGER_API URL_Widget_Params : public UUserWidget
{
	GENERATED_BODY()

private:

	UFUNCTION()
	virtual void CopyToClipBoard();

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	// Colors Index: 0 - Info, 1 - Warning, 2 - Error
	virtual void SetLogParams(FString ParamName, FString ParamValue, ERuntimeLogLevels RL_Level, TArray<FColor> Colors);

	/*
	* BindWidget & BindWidgetOptional
	* You need to open "Show Inherited Variables" to show it in "Variables" section.
	*/ 

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* CanvasPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UHorizontalBox* ParamsContainer = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Entry_Title = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Entry_Description = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Delimiter = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_Description = nullptr;

};