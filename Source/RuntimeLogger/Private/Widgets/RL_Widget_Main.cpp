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
		return;
	}

	this->World = TempWorld;
	URuntimeLoggerSubsystem* TempSubsystem = this->World->GetGameInstance()->GetSubsystem<URuntimeLoggerSubsystem>();

	if (!IsValid(TempSubsystem))
	{
		return;
	}

	this->LoggerSubsystem = TempSubsystem;
	this->LoggerSubsystem->Delegate_Runtime_Logger.AddDynamic(this, &URL_Widget_Main::OnLogReceived);
}

void URL_Widget_Main::OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Level)
{
	if (!EntryClass)
	{
		return;
	}

	if(!IsValid(this->World))
	{ 
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->World, 0);

	if (!IsValid(PlayerController))
	{
		return;
	}
	
	URL_Widget_Entries* LogEntry = CreateWidget<URL_Widget_Entries>(PlayerController, EntryClass);
	
	if (!IsValid(LogEntry))
	{
		return;
	}

	UPanelSlot* AddedSlot = this->Container_Logs->AddChild(LogEntry);
	
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

	LogEntry->SetLogParams(Out_UUID, Out_Log, Out_Level);
}