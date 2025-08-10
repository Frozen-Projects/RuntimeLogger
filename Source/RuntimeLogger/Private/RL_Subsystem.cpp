#include "RL_Subsystem.h"
#include "Logger_Thread.h"

#include "Misc/App.h"
#include "JsonUtilities.h"
#include "JsonObjectWrapper.h"
#include "Kismet/KismetSystemLibrary.h"

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	this->StartLogging();
	Super::Initialize(Collection);
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Failed to open log file at path: " + SaveDir);
		UE_LOG(LogTemp, Error, TEXT("Failed to open log file at path: %s"), *SaveDir);
	}
}

void URuntimeLoggerSubsystem::StartLogging()
{
	this->OpenLogFile();
	this->LoggerThread = new FRuntimeLogger_Thread(this);
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
	}

	if (this->LogFileHandle.IsValid())
	{
		FTCHARToUTF8 Utf8(*FString("]"));
		this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
		this->LogFileHandle->Flush();
	}

	this->LoggerThread = nullptr;
	this->LogQueue.Empty();
	this->LogDb.Empty();
}

void URuntimeLoggerSubsystem::RecordMessages()
{
	FString RawString;

	if (this->LogQueue.Dequeue(RawString))
	{
		FJsonObjectWrapper RawJson;
		RawJson.JsonObjectFromString(RawString);

		const FString UUID = RawJson.JsonObject->GetStringField(TEXT("UUID"));
		RawJson.JsonObject->RemoveField(TEXT("UUID"));

		if (LogFileHandle.IsValid())
		{
			FJsonObjectWrapper FileJson;
			FileJson.JsonObject->SetObjectField(UUID, RawJson.JsonObject);

			FString FileString;
			FileJson.JsonObjectToString(FileString);
			
			//const int64 Position = this->LogFileHandle->Tell();
			//const FString FileEntry = Position == 0 ? "[" + FileString : ",\n" + FileString;
			const FString FileEntry = this->bIsFirstEntry ? "[" + FileString : ",\n" + FileString;

			FTCHARToUTF8 Utf8(*FileEntry);
			this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
			this->bIsFirstEntry = false;
		}

		FString LogDataString;
		RawJson.JsonObjectToString(LogDataString);
		this->LogDb.Add(UUID, LogDataString);

		const FString LevelString = RawJson.JsonObject->GetStringField(TEXT("LogLevel"));
		const ERuntimeLogLevels RuntimeLogLevel = this->GetLogLevelFromString(LevelString);

		AsyncTask(ENamedThreads::GameThread, [this, UUID, LogDataString, RuntimeLogLevel]()
			{
				this->Delegate_Runtime_Logger.Broadcast(UUID, LogDataString, RuntimeLogLevel);
			}
		);
	}
}

FString URuntimeLoggerSubsystem::LogMessage(TMap<FString, FString> Message, ERuntimeLogLevels RuntimeLogLevel)
{
	if (!Message.IsEmpty())
	{
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
		
		if (!this->LogQueue.Enqueue(LogString))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Failed to enqueue log message ! : " + LogString);
			UE_LOG(LogTemp, Error, TEXT("Failed to enqueue log message ! : %s"), *LogString);
		}

		return UUID;
	}

	return FString();
}

TMap<FString, FString> URuntimeLoggerSubsystem::GetLogDb()
{
	return this->LogDb;
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
