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

	UWorld* World = nullptr;
	URuntimeLoggerSubsystem* LoggerSubsystem = nullptr;
	
	TMap<FString, URL_Each_Log*> MAP_Widgets;
	TSet<FString> Current_Criticalities;

	virtual void SetSubsystem();

	UFUNCTION()
	virtual void OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level);
	virtual void GenerateChildWidgets(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level);
	virtual void CreateFilters(ERuntimeLogLevels Out_Level);

	// It is for resetting old widgets without completely destroying whole log system.
	UFUNCTION()
	virtual void OnLogsReset();

	// It is for search box.
	UFUNCTION()
	virtual void OnSearchTextCommit(const FText& InText, ETextCommit::Type InCommitType);

	UFUNCTION()
	virtual void OnFilterSelection(FString SelectedItem, ESelectInfo::Type SelectionType);

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	/*
	* You need to open "Show Inherited Variables" to show it in "Variables" section.
	*/ 

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* Canvas_Panel = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetOptional))
	UVerticalBox* Design_Tree = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* Horizontal_Scroller = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UScrollBox* Container_Logs = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UEditableTextBox* Search_Box = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UComboBoxString* Filter_Criticality = nullptr;

	UPROPERTY(BlueprintReadWrite)
	FString Str_All_Criticalities = "All Levels";

	UPROPERTY(BlueprintReadWrite)
	FColor Color_Info = FColor::Purple;

	UPROPERTY(BlueprintReadWrite)
	FColor Color_Warning = FColor::Yellow;

	UPROPERTY(BlueprintReadWrite)
	FColor Color_Error = FColor::Red;

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