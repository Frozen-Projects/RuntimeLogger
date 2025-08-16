#include "Widgets/RL_Widget_Main.h"
#include "JsonUtilities.h"
#include "JsonObjectWrapper.h"

void URL_Widget_Main::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(this->Filter_Criticality))
	{
		this->Filter_Criticality->SetSelectedOption(Str_All_Criticalities);
	}
}

void URL_Widget_Main::NativeConstruct()
{
	Super::NativeConstruct();
	
	this->SetSubsystem();
	this->Search_Box->OnTextCommitted.AddDynamic(this, &URL_Widget_Main::OnSearchTextCommit);
	this->Filter_Criticality->OnSelectionChanged.AddDynamic(this, &::URL_Widget_Main::OnFilterSelection);
}

void URL_Widget_Main::NativeDestruct()
{
	Super::NativeDestruct();
}

void URL_Widget_Main::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> URL_Widget_Main::RebuildWidget()
{
	return Super::RebuildWidget();
}

void URL_Widget_Main::SetSubsystem()
{
	UWorld* TempWorld = GEngine->GetCurrentPlayWorld();

	if (!IsValid(TempWorld))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Current Play World is not valid !"));
		return;
	}

	this->World = TempWorld;
	URuntimeLoggerSubsystem* TempSubsystem = this->World->GetGameInstance()->GetSubsystem<URuntimeLoggerSubsystem>();

	if (!IsValid(TempSubsystem))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Runtime Logger Subsystem is not valid ! Make sure it is initialized in the GameInstance !"));
		return;
	}

	this->LoggerSubsystem = TempSubsystem;
	this->LoggerSubsystem->Delegate_Runtime_Logger.AddDynamic(this, &URL_Widget_Main::OnLogReceived);
	this->LoggerSubsystem->Delegate_Runtime_Logger_Reset.AddDynamic(this, &URL_Widget_Main::OnLogsReset);
}

void URL_Widget_Main::OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level)
{
	this->GenerateChildWidgets(Out_UUID, Out_Log, Out_Level);
	this->CreateFilters(Out_Level);
}

void URL_Widget_Main::GenerateChildWidgets(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level)
{
	if (!IsValid(this->World))
	{
		UE_LOG(LogTemp, Fatal, TEXT("World is not valid !"));
		return;
	}

	if (!IsValid(this->LoggerSubsystem))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Runtime Logger Subsystem is not valid !"));
		return;
	}

	if (!Each_Log_Class)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Each Log Class is not set !"));
		return;
	}

	if (!Log_Param_Class)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Log Param Class is not set !"));
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->World, 0);

	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Player Controller is not valid!"));
		return;
	}

	URL_Each_Log* Each_Log = CreateWidget<URL_Each_Log>(PlayerController, Each_Log_Class);

	if (!IsValid(Each_Log))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Each Log Widget is not valid!"));
		return;
	}

	UPanelSlot* AddedSlot = this->Container_Logs->AddChild(Each_Log);

	if (!IsValid(AddedSlot))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Failed to add Each Log Widget to Container!"));
		return;
	}

	UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(AddedSlot);

	if (!IsValid(ScrollBoxSlot))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Added Slot is not a ScrollBoxSlot!"));
		return;
	}

	ScrollBoxSlot->SetPadding(FMargin(10.0f));
	ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	ScrollBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	TMap<FString, FString> Map_LogData = this->LoggerSubsystem->JsonToMap(Out_Log);
	Each_Log->SetLogParams(Out_UUID, Map_LogData, Out_Level, Log_Param_Class);

	this->MAP_Widgets.Add(Out_UUID, Each_Log);
}

void URL_Widget_Main::CreateFilters(ERuntimeLogLevels Out_Level)
{
	TArray<FString> String_Sections;
	UEnum::GetValueAsString(Out_Level).ParseIntoArray(String_Sections, TEXT("::"));
	const FString Criticality_String = String_Sections[1];

	const int RetVal = this->Filter_Criticality->FindOptionIndex(Criticality_String);

	if (RetVal == -1)
	{
		this->Filter_Criticality->AddOption(Criticality_String);
	}	
}

void URL_Widget_Main::OnLogsReset()
{
	if (!IsValid(this->Container_Logs))
	{
		return;
	}

	TArray<UWidget*> Children = this->Container_Logs->GetAllChildren();

	if (Children.IsEmpty())
	{
		return;
	}

	for (UWidget* Each_Child : Children)
	{
		if (IsValid(Each_Child))
		{
			Each_Child->RemoveFromParent();
		}
	}

	this->Container_Logs->ClearChildren();
	this->Filter_Criticality->ClearOptions();
	this->Filter_Criticality->AddOption(Str_All_Criticalities);

	this->MAP_Widgets.Empty();
}

void URL_Widget_Main::OnSearchTextCommit(const FText& InText, ETextCommit::Type InCommitType)
{
	if (InCommitType == ETextCommit::OnEnter)
	{
		if (InText.IsEmpty())
		{
			return;
		}

		const FString Target_UUID = InText.ToString();

		if (this->MAP_Widgets.IsEmpty())
		{
			return;
		}

		if (!IsValid(this->Container_Logs))
		{
			return;
		}

		URL_Each_Log* FoundWidget = *this->MAP_Widgets.Find(Target_UUID);

		if (!IsValid(FoundWidget))
		{
			return;
		}

		this->Container_Logs->ScrollWidgetIntoView(FoundWidget, false);
	}
}

void URL_Widget_Main::OnFilterSelection(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	TArray<URL_Each_Log*> LogWidgets;
	this->MAP_Widgets.GenerateValueArray(LogWidgets);

	if (SelectedItem == Str_All_Criticalities)
	{
		for (URL_Each_Log* Each_Widget : LogWidgets)
		{
			if (!IsValid(Each_Widget))
			{
				continue;
			}

			Each_Widget->SetVisibility(ESlateVisibility::Visible);
		}
	}

	else
	{
		for (URL_Each_Log* Each_Widget : LogWidgets)
		{
			if (!IsValid(Each_Widget))
			{
				continue;
			}

			if (SelectedItem != Each_Widget->GetLogLevel())
			{
				Each_Widget->SetVisibility(ESlateVisibility::Collapsed);
			}

			else
			{
				Each_Widget->SetVisibility(ESlateVisibility::Visible);
			}
		}
	}
}