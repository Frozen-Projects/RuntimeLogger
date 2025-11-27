#include "Widgets/RL_Widget_Params.h"

void URL_Widget_Params::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Widget_Params::NativeConstruct()
{
	Super::NativeConstruct();
	this->Button_Description->OnClicked.AddDynamic(this, &URL_Widget_Params::CopyToClipBoard);
}

void URL_Widget_Params::NativeDestruct()
{
	Super::NativeDestruct();
}

void URL_Widget_Params::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> URL_Widget_Params::RebuildWidget()
{
	return Super::RebuildWidget();
}

void URL_Widget_Params::CopyToClipBoard()
{
	FPlatformApplicationMisc::ClipboardCopy(*this->Entry_Description->GetText().ToString());
}

void URL_Widget_Params::SetLogParams(FString ParamName, FString ParamValue, ERuntimeLogLevels RL_Level, TArray<FColor> Colors)
{
	if (Colors.Num() < 3)
	{
		return;
	}

	FSlateColor TitleColor;

	switch (RL_Level)
	{
		case ERuntimeLogLevels::Info:

			TitleColor = Colors[0].ReinterpretAsLinear();
			break;

		case ERuntimeLogLevels::Warning:

			TitleColor = Colors[1].ReinterpretAsLinear();
			break;

		case ERuntimeLogLevels::Critical:

			TitleColor = Colors[2].ReinterpretAsLinear();
			break;

		default:

			TitleColor = Colors[0].ReinterpretAsLinear();
			break;
	}

	this->Entry_Title->SetColorAndOpacity(TitleColor);
	this->Entry_Title->SetText(FText::FromString(ParamName));

	this->Entry_Description->SetColorAndOpacity(TitleColor);
	this->Entry_Description->SetText(FText::FromString(ParamValue));

	this->Delimiter->SetColorAndOpacity(TitleColor);
}