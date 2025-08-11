#include "Widgets/RL_Widget_Main.h"

void URL_Widget_Main::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Widget_Main::NativeConstruct()
{
	Super::NativeConstruct();
	this->SetSubsystem();
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
}

void URL_Widget_Main::OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level)
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
}