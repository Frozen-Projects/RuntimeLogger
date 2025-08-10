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
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	if (RootWidget && WidgetTree)
	{
		this->CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("")));
		RootWidget->AddChild(this->CanvasPanel);

		this->Container_Logs = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), FName(TEXT("Container_Logs")));
		this->CanvasPanel->AddChild(this->Container_Logs);
	}

	return Widget;
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

void URL_Widget_Main::OnLogReceived(FString Out_UUID, FString Out_Log, ERuntimeLogLevels Out_Levels)
{
	if(!IsValid(this->World))
	{ 
		return;
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->World, 0);

	if (!IsValid(PlayerController))
	{
		return;
	}

	TSubclassOf<UUserWidget> WidgetClass = URL_Widget_Entries::StaticClass();

	if (!IsValid(WidgetClass))
	{
		return;
	}
	
	UUserWidget* LogEntry = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
	
	if (!IsValid(LogEntry))
	{
		return;
	}

	this->Container_Logs->AddChild(LogEntry);
}