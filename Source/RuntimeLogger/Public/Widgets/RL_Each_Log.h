#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/RL_Each_Log_Param.h"

#include "RL_Each_Log.generated.h"

UCLASS()
class RUNTIMELOGGER_API URL_Each_Log : public UUserWidget
{
	GENERATED_BODY()

private:

	UWorld* World = nullptr;

	UFUNCTION()
	virtual void CopyToClipBoard();

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void SetLogParams(FString UUID, TMap<FString, FString> OtherParams, ERuntimeLogLevels RL_Level, TSubclassOf<URL_Each_Log_Param> ParamClass);

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