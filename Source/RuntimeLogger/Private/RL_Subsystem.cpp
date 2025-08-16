#include "RL_Subsystem.h"
#include "Logger_Thread.h"

#include "Misc/App.h"
#include "JsonUtilities.h"
#include "JsonObjectWrapper.h"
#include "Kismet/KismetSystemLibrary.h"

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	this->StartLogging();
}

void URuntimeLoggerSubsystem::Deinitialize()
{
	this->CleanupLogs();
	Super::Deinitialize();
}

void URuntimeLoggerSubsystem::OpenLogFile()
{
	const FString ProjectName = FApp::GetProjectName();
	FString SaveDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "/" + ProjectName + "_RuntimeLogger_" + FDateTime::Now().ToString() + ".log";
	FPaths::MakePlatformFilename(SaveDir);

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	this->LogFileHandle.Reset(PlatformFile.OpenWrite(*SaveDir, true, true));

	if (this->LogFileHandle.IsValid())
	{
		this->LogFilePath = SaveDir;
	}

	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Failed to open log file at path: %s"), *SaveDir);
	}
}

void URuntimeLoggerSubsystem::StartLogging()
{
	this->OpenLogFile();
	FRuntimeLogger_Thread* TempThread = new FRuntimeLogger_Thread(this);

	if (!TempThread)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Runtime logger thread is not valid !"));
		return;
	}

	this->LoggerThread = TempThread;
}

ERuntimeLogLevels URuntimeLoggerSubsystem::GetLogLevelFromString(FString LogLevelString)
{
	TArray<FString> Sections;
	LogLevelString.ParseIntoArray(Sections, TEXT("::"), true);

	const UEnum* EnumPtr = StaticEnum<ERuntimeLogLevels>();
	const int64 EnumValue = EnumPtr->GetValueByName(*Sections[1]);
	return static_cast<ERuntimeLogLevels>(EnumValue);
}

void URuntimeLoggerSubsystem::CleanupLogs()
{
	if (this->LoggerThread)
	{
		delete this->LoggerThread;
		this->LoggerThread = nullptr;
	}

	if (this->LogFileHandle.IsValid())
	{
		FTCHARToUTF8 Utf8(*FString("]"));
		this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
		this->LogFileHandle->Flush();
	}

	this->LogQueue.Empty();
	this->LogDb.Empty();
}

void URuntimeLoggerSubsystem::RecordMessages()
{
	FString RawString;

	if (!this->LogQueue.Dequeue(RawString))
	{
		return;
	}

	if (!LogFileHandle.IsValid())
	{
		return;
	}

	// RECORDING LOGS TO TMAP DB

	FJsonObjectWrapper RawJson;
	RawJson.JsonObjectFromString(RawString);

	const FString UUID = RawJson.JsonObject->GetStringField(TEXT("UUID"));
	RawJson.JsonObject->RemoveField(TEXT("UUID"));

	FString LogDataString;
	RawJson.JsonObjectToString(LogDataString);
	this->LogDb.Add(UUID, LogDataString);

	// RECORDING LOGS TO FILE

	FJsonObjectWrapper FileJson;
	FileJson.JsonObject->SetObjectField(UUID, RawJson.JsonObject);

	FString FileString;
	FileJson.JsonObjectToString(FileString);
	const FString FileEntry = this->bIsFirstEntry ? "[" + FileString : ",\n" + FileString;

	FTCHARToUTF8 Utf8(*FileEntry);
	this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
	this->bIsFirstEntry = false;

	// BROADCASTING LOGS TO DELEGATE

	const FString LevelString = RawJson.JsonObject->GetStringField(TEXT("LogLevel"));
	const ERuntimeLogLevels RuntimeLogLevel = URuntimeLoggerSubsystem::GetLogLevelFromString(LevelString);

	AsyncTask(ENamedThreads::GameThread, [this, UUID, LogDataString, RuntimeLogLevel]()
		{
			this->Delegate_Runtime_Logger.Broadcast(UUID, LogDataString, RuntimeLogLevel);
		}
	);
}

bool URuntimeLoggerSubsystem::IsQueueEmpty()
{
	return this->LogQueue.IsEmpty();
}

FString URuntimeLoggerSubsystem::LogMessage(TMap<FString, FString> Message, ERuntimeLogLevels RuntimeLogLevel)
{
	if (Message.IsEmpty())
	{
		return FString();
	}

	FJsonObjectWrapper TempJson;

	for (const TPair<FString, FString> EachParam : Message)
	{
		TempJson.JsonObject->SetStringField(EachParam.Key, EachParam.Value);
	}

	const FString UUID = FGuid::NewGuid().ToString();
	TempJson.JsonObject->SetStringField(TEXT("UUID"), UUID);

	const FString LogTime = FDateTime::Now().ToString();
	TempJson.JsonObject->SetStringField(TEXT("LogTime"), LogTime);

	const FString LevelString = UEnum::GetValueAsString(RuntimeLogLevel);
	TempJson.JsonObject->SetStringField(TEXT("LogLevel"), LevelString);

	FString LogString;
	TempJson.JsonObjectToString(LogString);
	
	if (!this->LoggerThread)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Logger thread is not initialized ! Cannot log message."));
		return FString();
	}

	if (!this->LogQueue.Enqueue(LogString))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to enqueue log message ! : %s"), *LogString);
		return FString();
	}

	this->LoggerThread->TriggerWakeEvent();
	return UUID;
}

TMap<FString, FString> URuntimeLoggerSubsystem::GetLogDb()
{
	return this->LogDb;
}

void URuntimeLoggerSubsystem::ResetLogs()
{
	this->LoggerThread->WaitForWakeEvent();
	
	FScopeLock Lock(&this->LogGuard);
	this->LogDb.Empty();
	this->Delegate_Runtime_Logger_Reset.Broadcast();

	this->LoggerThread->TriggerWakeEvent();
}

FString URuntimeLoggerSubsystem::GetLogFilePath() const
{
	FString TempPath = this->LogFilePath;
	FPaths::NormalizeFilename(TempPath);
	return TempPath;
}

FString URuntimeLoggerSubsystem::GetLog(const FString& UUID) const
{
	if (this->LogDb.Contains(UUID))
	{
		return *this->LogDb.Find(UUID);
	}
	
	else
	{
		return FString();
	}
}

TMap<FString, FString> URuntimeLoggerSubsystem::JsonToMap(FString JsonString)
{
	FJsonObjectWrapper RawJson;

	if (!RawJson.JsonObjectFromString(JsonString))
	{
		// This is a helper function. So, we can use our logging system to log errors.

		TMap<FString, FString> LogData;
		LogData.Add("PluginName", "Runtime Logger");
		LogData.Add("FunctionName", FString(ANSI_TO_TCHAR(__FUNCSIG__)));
		LogData.Add("Details", "There was a problem to convert json to a TMap !");

		this->LogMessage(LogData, ERuntimeLogLevels::Critical);
	}

	TMap<FString, FString> Map_Data;
	for (TPair<FString, TSharedPtr<FJsonValue>> EachParam : RawJson.JsonObject->Values)
	{
		const FString Key = EachParam.Key;
		const FString Value = EachParam.Value.IsValid() ? EachParam.Value->AsString() : FString();
		Map_Data.Add(Key, Value);
	}

	return Map_Data;
}