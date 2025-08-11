#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/RL_Each_Log.h"

#include "RL_Widget_Main.generated.h"

UCLASS()
class RUNTIMELOGGER_API URL_Widget_Main : public UUserWidget
{
	GENERATED_BODY()

private:

	URuntimeLoggerSubsystem* LoggerSubsystem = nullptr;

	UWorld* World = nullptr;

	virtual void SetSubsystem();

	UFUNCTION()
	virtual void OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level);

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/*
	* BindWidget & BindWidgetOptional
	* You need to open "Show Inherited Variables" to show it in "Variables" section.
	*/ 

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* CanvasPanel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* Container_Logs = nullptr;

	/*
	* It has to be a derived blueprint class !
	*/
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<URL_Each_Log> Each_Log_Class = nullptr;

	/*
	* It has to be a derived blueprint class !
	*/
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<URL_Each_Log_Param> Log_Param_Class = nullptr;

};