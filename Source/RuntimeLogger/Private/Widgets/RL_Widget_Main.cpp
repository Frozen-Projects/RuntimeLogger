#include "Widgets/RL_Widget_Main.h"

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
	if (this->LoggerSubsystem && this->LoggerSubsystem->Delegate_Runtime_Logger.IsBound())
	{
		LoggerSubsystem->Delegate_Runtime_Logger.RemoveDynamic(this, &URL_Widget_Main::OnLogReceived);
	}

	this->bIsClosing = true;
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
	URuntimeLoggerSubsystem* TempSubsystem = GEngine->GetEngineSubsystem<URuntimeLoggerSubsystem>();

	if (!IsValid(TempSubsystem))
	{
		UE_LOG(LogRL, Error, TEXT("Runtime Logger Subsystem is not valid ! Make sure it is initialized in the GameInstance !"));
		return;
	}

	UWorld* CurrentWorld = GEngine->GetCurrentPlayWorld();

	if (!CurrentWorld)
	{
		UE_LOG(LogTemp, Error, TEXT("Current Play World is not valid !"));
		return;
	}

	this->World = CurrentWorld;

	this->LoggerSubsystem = TempSubsystem;
	this->LoggerSubsystem->Delegate_Runtime_Logger.AddDynamic(this, &URL_Widget_Main::OnLogReceived);
}

void URL_Widget_Main::OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level)
{
	this->GenerateChildWidgets(Out_UUID, Out_Log, Out_Level);
	this->CreateFilters(Out_Level);
}

void URL_Widget_Main::GenerateChildWidgets(FString In_UUID, FString In_Log, ERuntimeLogLevels In_Level)
{
	if (this->bIsClosing)
	{
		return;
	}

	if (!IsValid(this->World))
	{
		return;
	}

	if (!IsValid(this->LoggerSubsystem))
	{
		return;
	}

	if (!this->Each_Log_Class)
	{
		return;
	}

	if (!this->Log_Param_Class)
	{
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->World, 0);

	if (!IsValid(PlayerController))
	{
		return;
	}

	URL_Widget_Logs* Each_Log = CreateWidget<URL_Widget_Logs>(PlayerController, this->Each_Log_Class);

	if (!IsValid(Each_Log))
	{
		return;
	}

	UPanelSlot* AddedSlot = this->Container_Logs->AddChild(Each_Log);

	if (!IsValid(AddedSlot))
	{
		return;
	}

	UScrollBoxSlot* ScrollBoxSlot = Cast<UScrollBoxSlot>(AddedSlot);

	if (!IsValid(ScrollBoxSlot))
	{
		return;
	}

	ScrollBoxSlot->SetPadding(FMargin(10.0f));
	ScrollBoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Fill);
	ScrollBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Fill);

	TArray<FColor> Log_Colors;
	Log_Colors.Add(this->Color_Info);
	Log_Colors.Add(this->Color_Warning);
	Log_Colors.Add(this->Color_Error);

	Each_Log->SetLogParams(In_UUID, In_Log, In_Level, this->Log_Param_Class, Log_Colors);

	this->MAP_Widgets.Add(In_UUID, Each_Log);
}

void URL_Widget_Main::CreateFilters(ERuntimeLogLevels In_Level)
{
	if (this->bIsClosing)
	{
		return;
	}

	TArray<FString> String_Sections;
	UEnum::GetValueAsString(In_Level).ParseIntoArray(String_Sections, TEXT("::"));
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

		URL_Widget_Logs* FoundWidget = *this->MAP_Widgets.Find(Target_UUID);

		if (!IsValid(FoundWidget))
		{
			return;
		}

		this->Container_Logs->ScrollWidgetIntoView(FoundWidget, false);
	}
}

void URL_Widget_Main::OnFilterSelection(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	TArray<URL_Widget_Logs*> LogWidgets;
	this->MAP_Widgets.GenerateValueArray(LogWidgets);

	if (SelectedItem == Str_All_Criticalities)
	{
		for (URL_Widget_Logs* Each_Widget : LogWidgets)
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
		for (URL_Widget_Logs* Each_Widget : LogWidgets)
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