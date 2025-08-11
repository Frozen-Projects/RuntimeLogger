#include "Widgets/RL_Each_Log_Param.h"

void URL_Each_Log_Param::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Each_Log_Param::NativeConstruct()
{
	Super::NativeConstruct();
	this->Button_Description->OnClicked.AddDynamic(this, &URL_Each_Log_Param::CopyToClipBoard);
}

void URL_Each_Log_Param::NativeDestruct()
{
	Super::NativeDestruct();
}

void URL_Each_Log_Param::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> URL_Each_Log_Param::RebuildWidget()
{
	return Super::RebuildWidget();
}

void URL_Each_Log_Param::CopyToClipBoard()
{
	FPlatformApplicationMisc::ClipboardCopy(*this->Entry_Description->GetText().ToString());
}

void URL_Each_Log_Param::SetLogParams(FString ParamName, FString ParamValue, ERuntimeLogLevels RL_Level)
{
	FSlateColor TitleColor;

	switch (RL_Level)
	{
		case ERuntimeLogLevels::Info:

			TitleColor = FSlateColor(FLinearColor::White);
			break;

		case ERuntimeLogLevels::Warning:

			TitleColor = FSlateColor(FLinearColor::Yellow);
			break;

		case ERuntimeLogLevels::Critical:

			TitleColor = FSlateColor(FLinearColor::Red);
			break;

		default:

			TitleColor = FSlateColor(FLinearColor::White);
			break;
	}

	this->Entry_Title->SetColorAndOpacity(TitleColor);
	this->Entry_Title->SetText(FText::FromString(ParamName));

	this->Entry_Description->SetColorAndOpacity(TitleColor);
	this->Entry_Description->SetText(FText::FromString(ParamValue));

	this->Delimiter->SetColorAndOpacity(TitleColor);
}