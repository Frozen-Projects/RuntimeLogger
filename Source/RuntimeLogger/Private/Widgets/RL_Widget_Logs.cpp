#include "Widgets/RL_Widget_Logs.h"

void URL_Widget_Logs::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Widget_Logs::NativeConstruct()
{
	Super::NativeConstruct();
	
	this->Button_UUID->OnClicked.AddDynamic(this, &URL_Widget_Logs::CopyToClipBoard);

	if (IsValid(this->World))
	{
		return;
	}

	UWorld* TempWorld = GEngine->GetCurrentPlayWorld();
	
	if (!IsValid(TempWorld))
	{
		return;
	}

	this->World = TempWorld;
}

void URL_Widget_Logs::NativeDestruct()
{
	Super::NativeDestruct();
}

void URL_Widget_Logs::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> URL_Widget_Logs::RebuildWidget()
{
	return Super::RebuildWidget();
}

void URL_Widget_Logs::CopyToClipBoard()
{
	FPlatformApplicationMisc::ClipboardCopy(*this->Title_UUID->GetText().ToString());
}

void URL_Widget_Logs::SetLogParams(FString UUID, FString In_Log, ERuntimeLogLevels RL_Level, TSubclassOf<URL_Widget_Params> ParamClass, TArray<FColor> Colors)
{
	if (!ParamClass)
	{
		return;
	}

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

	this->Title_UUID->SetColorAndOpacity(TitleColor);
	this->Title_UUID->SetText(FText::FromString(UUID));
	this->SetLogLevel(RL_Level);

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this->World, 0);

	if (!IsValid(PlayerController))
	{
		return;
	}

	FJsonObjectWrapper LogObject;
	if (!LogObject.JsonObjectFromString(In_Log))
	{
		return;
	}

	for (TPair<FString, TSharedPtr<FJsonValue>> EachParam : LogObject.JsonObject->Values)
	{
		URL_Widget_Params* NewParam = CreateWidget<URL_Widget_Params>(PlayerController, ParamClass);

		if (IsValid(NewParam))
		{
			this->ParamsBody->AddChild(NewParam);
			const FString ParamValue = EachParam.Value->AsString();
			NewParam->SetLogParams(EachParam.Key, ParamValue, RL_Level, Colors);
		}
	}
}

FString URL_Widget_Logs::GetLogLevel()
{
	return this->RL_Level_String;
}

void URL_Widget_Logs::SetLogLevel(ERuntimeLogLevels RL_Level)
{
	const FString RawLogLevel = UEnum::GetValueAsString(RL_Level);
	
	TArray<FString> LogLevelSections;
	RawLogLevel.ParseIntoArray(LogLevelSections, TEXT("::"));

	this->RL_Level_String = LogLevelSections[1];
}