#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/RL_Widget_Entries.h"

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

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<URL_Widget_Entries> EntryClass = nullptr;

};