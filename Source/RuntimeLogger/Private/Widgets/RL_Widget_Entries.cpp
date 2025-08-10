#include "Widgets/RL_Widget_Entries.h"

void URL_Widget_Entries::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Widget_Entries::NativeConstruct()
{
	Super::NativeConstruct();
}

void URL_Widget_Entries::NativeDestruct()
{
	Super::NativeDestruct();
}

void URL_Widget_Entries::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> URL_Widget_Entries::RebuildWidget()
{
	TSharedRef<SWidget> Widget = Super::RebuildWidget();

	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());

	if (RootWidget && WidgetTree)
	{
		this->CanvasPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), FName(TEXT("")));
		RootWidget->AddChild(this->CanvasPanel);

		this->Container_Logs = WidgetTree->ConstructWidget<UScrollBox>(UScrollBox::StaticClass(), FName(TEXT("Container_Logs")));
		RootWidget->AddChild(this->CanvasPanel);
	}

	return Widget;
}