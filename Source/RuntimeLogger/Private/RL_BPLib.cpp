#include "RL_BPLib.h"
#include "RuntimeLogger.h"

URL_Static_Functions::URL_Static_Functions(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

bool URL_Static_Functions::LogFileToJson(FJsonObjectWrapper& Out_Json, FString LogFile)
{
	if (LogFile.IsEmpty())
	{
		return false;
	}

	FPaths::NormalizeFilename(LogFile);

	if (!FPaths::FileExists(LogFile))
	{
		return false;
	}

	const std::string PathUtf8 = TCHAR_TO_UTF8(*LogFile);

	std::ifstream in(PathUtf8, std::ios::in | std::ios::binary);
	if (!in.is_open())
	{
		return false;
	}

	std::string Line;
	FJsonObjectWrapper ResultJson = FJsonObjectWrapper();
	TArray<TSharedPtr<FJsonValue>> Details;

	while (std::getline(in, Line))
	{
		if (!Line.empty() && Line.back() == '\n')
		{
			Line.pop_back();
		}

		const FString LineFString = UTF8_TO_TCHAR(Line.c_str());

		FJsonObjectWrapper LogEntry;
		if (LogEntry.JsonObjectFromString(LineFString))
		{
			Details.Add(MakeShared<FJsonValueObject>(LogEntry.JsonObject));
		}
	}

	ResultJson.JsonObject->SetArrayField(TEXT("root"), Details);
	Out_Json = ResultJson;

	return true;
}

void URL_Static_Functions::LogFileToJson_BP(FDelegateRLExport Delegate_Export, FString In_File)
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [Delegate_Export, In_File]()
		{
			FJsonObjectWrapper ResultJson;
			const bool bIsSuccessfull = URL_Static_Functions::LogFileToJson(ResultJson, In_File);

			AsyncTask(ENamedThreads::GameThread, [Delegate_Export, bIsSuccessfull, ResultJson]()
				{
					Delegate_Export.ExecuteIfBound(bIsSuccessfull, ResultJson);
				});
		});
}

void URL_Static_Functions::LogMessage(int32 InLogLevel, FString In_Log)
{
	switch (InLogLevel)
	{
		case 0:
			UE_LOG(LogTemp, Display, TEXT("%s"), *In_Log);
			return;
		case 1:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *In_Log);
			return;
		case 2:
			UE_LOG(LogTemp, Error, TEXT("%s"), *In_Log);
			return;
		default:
			UE_LOG(LogTemp, Warning, TEXT("%s"), *In_Log);
			break;
	}
}

void URL_Static_Functions::LogJson(int32 InLogLevel, FJsonObjectWrapper In_Log)
{
    FString LogString;
	In_Log.JsonObjectToString(LogString);

    switch (InLogLevel)
    {
        case 0:

            UE_LOG(LogTemp, Display, TEXT("%s"), *LogString);
            return;

        case 1:

            UE_LOG(LogTemp, Warning, TEXT("%s"), *LogString);
            return;

        case 2:

            UE_LOG(LogTemp, Error, TEXT("%s"), *LogString);
            return;

        default:
            UE_LOG(LogTemp, Warning, TEXT("%s"), *LogString);
            break;
    }
}

FString URL_Static_Functions::GenerateUUIDv7()
{
	FRL_UUIDv7 UUID;
	return UTF8_TO_TCHAR(UUID.generateString().c_str());
}

TArray<uint8> URL_Static_Functions::GenerateUUIDv7Bytes()
{
	FRL_UUIDv7 UUID;
	
	std::array<uint8_t, 16> bytes = UUID.generateBytes();
	TArray<uint8> result;
	result.Append(bytes.data(), bytes.size());
	return result;
}