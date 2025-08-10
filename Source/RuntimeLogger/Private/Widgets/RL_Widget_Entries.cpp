#include "Widgets/RL_Widget_Entries.h"

void URL_Widget_Entries::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Widget_Entries::NativeConstruct()
{
	Super::NativeConstruct();
	this->Button_UUID->OnClicked.AddDynamic(this, &URL_Widget_Entries::CopyToClipBoard);
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
	return Super::RebuildWidget();
}

void URL_Widget_Entries::SetLogParams(FString UUID, FString OtherParams, ERuntimeLogLevels RL_Level)
{
	this->Title_UUID->SetText(FText::FromString(UUID));
}

void URL_Widget_Entries::CopyToClipBoard()
{
	FPlatformApplicationMisc::ClipboardCopy(*this->Title_UUID->GetText().ToString());
}