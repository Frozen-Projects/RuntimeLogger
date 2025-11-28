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
	FString SaveDir = UKismetSystemLibrary::GetProjectSavedDirectory() + "/" + ProjectName + "_RL_" + FDateTime::Now().ToString() + ".log";
	FPaths::MakePlatformFilename(SaveDir);

	if (this->LogFileBuffer.open(TCHAR_TO_UTF8(*SaveDir), std::ios::in | std::ios::out | std::ios::app | std::ios::binary))
	{
		this->LogFilePath = SaveDir;
		this->LogFileStream.rdbuf(&this->LogFileBuffer);
	}

	else
	{
		// We used custom log name because if there is an issue opening the file, LogTemp will cause recursive calls infinitely.
		UE_LOG(LogRL, Error, TEXT("Failed to open log file at path: %s"), *SaveDir);
	}
}

void URuntimeLoggerSubsystem::CleanupLogs()
{
	if (this->LogFileBuffer.is_open())
	{
		this->LogFileStream.flush();
		this->LogFileBuffer.close();
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

	if (!this->LogFileBuffer.is_open())
	{
		this->OpenLogFile();
	}

	if (this->LogFileBuffer.is_open())
	{
		FJsonObjectWrapper FileJson = Message;
		FileJson.JsonObject->SetStringField(TEXT("UUID"), In_UUID);

		FString FileString;
		FileJson.JsonObjectToString(FileString);
		const FString FileEntry = FileString + "\n";

		FTCHARToUTF8 Utf8(*FileEntry);
		this->LogFileStream.write(Utf8.Get(), Utf8.Length());
		this->LogFileStream.flush();

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

bool URuntimeLoggerSubsystem::LogFileToJson(FJsonObjectWrapper& Out_Json, FString LogFile)
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

bool URuntimeLoggerSubsystem::MemoryToJson(FJsonObjectWrapper& Out_Json)
{
	if (this->LogDb.IsEmpty())
	{
		return false;
	}

	FJsonObjectWrapper ResultJson = FJsonObjectWrapper();
	TArray<TSharedPtr<FJsonValue>> Details;
	
	for (const TPair<FString, FJsonObjectWrapper>& Pair : this->LogDb)
	{
		FJsonObjectWrapper LogEntry = Pair.Value;
		LogEntry.JsonObject->SetStringField(TEXT("UUID"), Pair.Key);
		Details.Add(MakeShared<FJsonValueObject>(LogEntry.JsonObject));
	}

	ResultJson.JsonObject->SetArrayField(TEXT("root"), Details);
	Out_Json = ResultJson;

	return true;
}

void URuntimeLoggerSubsystem::LogFileToJson_BP(FDelegateRLExport Delegate_Export, FString In_File)
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Delegate_Export, In_File]()
	{
		FJsonObjectWrapper ResultJson;
		const bool bIsSuccessfull = this->LogFileToJson(ResultJson, In_File);

		AsyncTask(ENamedThreads::GameThread, [Delegate_Export, bIsSuccessfull, ResultJson]()
		{
			Delegate_Export.ExecuteIfBound(bIsSuccessfull, ResultJson);
		});
	});
}

void URuntimeLoggerSubsystem::MemoryToJson_BP(FDelegateRLExport Delegate_Export)
{
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, Delegate_Export]()
	{
		FJsonObjectWrapper ResultJson;
		const bool bIsSuccessfull = this->MemoryToJson(ResultJson);

		AsyncTask(ENamedThreads::GameThread, [Delegate_Export, bIsSuccessfull, ResultJson]()
		{
			Delegate_Export.ExecuteIfBound(bIsSuccessfull, ResultJson);
		});
	});
}