#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/RL_Widget_Params.h"

#include "RL_Widget_Logs.generated.h"

UCLASS()
class RUNTIMELOGGER_API URL_Widget_Logs : public UUserWidget
{
	GENERATED_BODY()

private:

	UWorld* World = nullptr;
	FString RL_Level_String;

	UFUNCTION()
	virtual void CopyToClipBoard();

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	// Colors Index: 0 - Info, 1 - Warning, 2 - Error
	virtual void SetLogParams(FString UUID, FString In_Log, ERuntimeLogLevels RL_Level, TSubclassOf<URL_Widget_Params> ParamClass, TArray<FColor> Colors);
	virtual FString GetLogLevel();
	virtual void SetLogLevel(ERuntimeLogLevels RL_Level);

	/*
	* BindWidget & BindWidgetOptional
	* You need to open "Show Inherited Variables" to show it in "Variables" section.
	*/ 

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* CanvasPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button_UUID = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UExpandableArea* LogParams = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* Title_UUID = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UVerticalBox* ParamsBody = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	URetainerBox* RetainerBox = nullptr;

};