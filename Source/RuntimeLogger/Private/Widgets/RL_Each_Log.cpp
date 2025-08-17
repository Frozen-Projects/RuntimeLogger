#include "Widgets/RL_Each_Log.h"

void URL_Each_Log::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void URL_Each_Log::NativeConstruct()
{
	Super::NativeConstruct();
	
	this->Button_UUID->OnClicked.AddDynamic(this, &URL_Each_Log::CopyToClipBoard);

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

void URL_Each_Log::NativeDestruct()
{
	Super::NativeDestruct();
}

void URL_Each_Log::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

TSharedRef<SWidget> URL_Each_Log::RebuildWidget()
{
	return Super::RebuildWidget();
}

void URL_Each_Log::CopyToClipBoard()
{
	FPlatformApplicationMisc::ClipboardCopy(*this->Title_UUID->GetText().ToString());
}

void URL_Each_Log::SetLogParams(FString UUID, TMap<FString, FString> OtherParams, ERuntimeLogLevels RL_Level, TSubclassOf<URL_Each_Log_Param> ParamClass, TArray<FColor> Colors)
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

	for (const TPair<FString, FString> EachParam : OtherParams)
	{
		URL_Each_Log_Param* NewParam = CreateWidget<URL_Each_Log_Param>(PlayerController, ParamClass);

		if (IsValid(NewParam))
		{
			this->ParamsBody->AddChild(NewParam);
			NewParam->SetLogParams(EachParam.Key, EachParam.Value, RL_Level, Colors);
		}
	}
}

FString URL_Each_Log::GetLogLevel()
{
	return this->RL_Level_String;
}

void URL_Each_Log::SetLogLevel(ERuntimeLogLevels RL_Level)
{
	const FString RawLogLevel = UEnum::GetValueAsString(RL_Level);
	
	TArray<FString> LogLevelSections;
	RawLogLevel.ParseIntoArray(LogLevelSections, TEXT("::"));

	this->RL_Level_String = LogLevelSections[1];
}