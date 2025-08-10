#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Widgets/RL_Widget_Includes.h"
#include "RL_Subsystem.h"

#include "RL_Widget_Entries.generated.h"

UCLASS()
class RUNTIMELOGGER_API URL_Widget_Entries : public UUserWidget
{
	GENERATED_BODY()

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

};