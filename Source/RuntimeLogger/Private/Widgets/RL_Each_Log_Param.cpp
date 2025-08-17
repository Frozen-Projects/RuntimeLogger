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

void URL_Each_Log_Param::SetLogParams(FString ParamName, FString ParamValue, ERuntimeLogLevels RL_Level, TArray<FColor> Colors)
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