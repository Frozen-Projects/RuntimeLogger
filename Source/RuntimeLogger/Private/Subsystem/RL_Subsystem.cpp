#include "Subsystem/RL_Subsystem.h"

#include "Misc/App.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogRL);

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	this->OpenLogFile();

	LogCaptureDevice = MakeUnique<FRuntimeLoggerOutput>();

	if (GLog)
	{
		GLog->AddOutputDevice(LogCaptureDevice.Get());
	}
}

void URuntimeLoggerSubsystem::Deinitialize()
{
	if (GLog && this->LogCaptureDevice)
	{
		GLog->RemoveOutputDevice(this->LogCaptureDevice.Get());
	}

	this->LogCaptureDevice.Reset();

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
		// We used custom log name because if there is an issue opening the file, LogTemp will cause recursive calls infinitely.
		UE_LOG(LogRL, Error, TEXT("Failed to open log file at path: %s"), *SaveDir);
	}
}

void URuntimeLoggerSubsystem::CleanupLogs()
{
	if (this->LogFileHandle.IsValid())
	{
		this->LogFileHandle->Flush();
	}

	this->LogDb.Empty();
}

ERuntimeLogLevels URuntimeLoggerSubsystem::GetLogLevelFromString(const FString& LogLevelString)
{
	if (LogLevelString == "Display" || LogLevelString == "Log")
	{
		return ERuntimeLogLevels::Info;
	}

	else if (LogLevelString == "Warning")
	{
		return ERuntimeLogLevels::Warning;
	}

	else if (LogLevelString == "Error" || LogLevelString == "Fatal")
	{
		return ERuntimeLogLevels::Critical;
	}

	else
	{
		return ERuntimeLogLevels::Info;
	}
}

int32 URuntimeLoggerSubsystem::RecordLog(const FString& In_UUID, FJsonObjectWrapper Message)
{
	if (In_UUID.IsEmpty())
	{
		return -1;
	}

	this->LogDb.Add(In_UUID, Message);
	
	FString MessageString;

	if (!Message.JsonObjectToString(MessageString))
	{
		return -1;
	}

	if (MessageString.IsEmpty())
	{
		return -1;
	}

	FString LevelString;
	ERuntimeLogLevels VerbosityLevel = ERuntimeLogLevels::Info;

	if (Message.JsonObject->TryGetStringField(TEXT("Verbosity"), LevelString))
	{
		VerbosityLevel = URuntimeLoggerSubsystem::GetLogLevelFromString(LevelString);
	}

	this->Delegate_Runtime_Logger.Broadcast(In_UUID, MessageString, VerbosityLevel);

	if (!this->LogFileHandle.IsValid())
	{
		this->OpenLogFile();
	}

	if (this->LogFileHandle.IsValid())
	{
		FJsonObjectWrapper FileJson = Message;
		FileJson.JsonObject->SetStringField(TEXT("UUID"), In_UUID);

		FString FileString;
		FileJson.JsonObjectToString(FileString);
		const FString FileEntry = FileString + "\n";

		FTCHARToUTF8 Utf8(*FileEntry);
		this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());

		return 1;
	}

	else
	{
		return 0;
	}
}

TMap<FString, FJsonObjectWrapper> URuntimeLoggerSubsystem::GetLogDb()
{
	return this->LogDb;
}

void URuntimeLoggerSubsystem::ResetLogs()
{
	this->CleanupLogs();
	this->OpenLogFile();
}

FString URuntimeLoggerSubsystem::GetLogFilePath() const
{
	FString TempPath = this->LogFilePath;
	FPaths::NormalizeFilename(TempPath);
	return TempPath;
}

bool URuntimeLoggerSubsystem::LogFileToSingleJson(FJsonObjectWrapper& Out_Json, FString LogFile)
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

	FString FileString;
	if (!FFileHelper::LoadFileToString(FileString, *LogFile))
	{
		return false;
	}

	TArray<FString> LogEntries;
	FileString.ParseIntoArray(LogEntries, TEXT("\n"), true);

	FJsonObjectWrapper ResultJson = FJsonObjectWrapper();
	TArray<TSharedPtr<FJsonValue>> Details;

	for (const FString& Entry : LogEntries)
	{
		FJsonObjectWrapper LogEntry;
		if (LogEntry.JsonObjectFromString(Entry))
		{
			Details.Add(MakeShared<FJsonValueObject>(LogEntry.JsonObject));
		}
	}

	ResultJson.JsonObject->SetArrayField(TEXT("root"), Details);
	Out_Json = ResultJson;

	return true;
}

FJsonObjectWrapper URuntimeLoggerSubsystem::GetLog(const FString& UUID)
{
	if (this->LogDb.Contains(UUID))
	{
		return *this->LogDb.Find(UUID);
	}
	
	else
	{
		return FJsonObjectWrapper();
	}
}