#include "Subsystem/RL_Subsystem.h"

DEFINE_LOG_CATEGORY(LogRL);

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	this->OpenLogFile();

	this->LogCaptureDevice = MakeUnique<FRuntimeLoggerOutput>();
	this->LogCaptureDevice->InitSubsystem(this);

	if (GLog)
	{
		GLog->AddOutputDevice(this->LogCaptureDevice.Get());
	}
}

void URuntimeLoggerSubsystem::Deinitialize()
{
	if (GLog && this->LogCaptureDevice)
	{
		GLog->RemoveOutputDevice(this->LogCaptureDevice.Get());
		this->LogCaptureDevice.Reset();
	}

	this->CleanupLogs();

	Super::Deinitialize();
}

#pragma region Internals

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

bool URuntimeLoggerSubsystem::IsSameMessage(FJsonObjectWrapper In_Message)
{
	if (this->LastLog.IsEmpty())
	{
		return false;
	}

	const FString MessageString = In_Message.JsonObject->HasField(MESSAGE_FIELD) ? In_Message.JsonObject->GetStringField(MESSAGE_FIELD) : TEXT("");
	const FString VerbosityString = In_Message.JsonObject->HasField(VERBOSITY_FIELD) ? In_Message.JsonObject->GetStringField(VERBOSITY_FIELD) : TEXT("");

	FJsonObjectWrapper LastMessageJson;
	if (!LastMessageJson.JsonObjectFromString(this->LastLog))
	{
		return false;
	}

	const FString LastMessageString = LastMessageJson.JsonObject->HasField(MESSAGE_FIELD) ? LastMessageJson.JsonObject->GetStringField(MESSAGE_FIELD) : TEXT("");
	const FString LastVerbosityString = LastMessageJson.JsonObject->HasField(VERBOSITY_FIELD) ? LastMessageJson.JsonObject->GetStringField(VERBOSITY_FIELD) : TEXT("");

	const double Interval = this->SameMessageInterval <= 0 ? 30.0 : this->SameMessageInterval;

	if (MessageString == LastMessageString && VerbosityString == LastVerbosityString && (FDateTime::UtcNow() - this->LastLogTime).GetTotalSeconds() <= Interval)
	{
		return true;
	}

	else
	{
		return false;
	}
}

#pragma endregion Internals

int32 URuntimeLoggerSubsystem::RecordLog(const FString& In_UUID, FJsonObjectWrapper Message)
{
	if (In_UUID.IsEmpty())
	{
		UE_LOG(LogRL, Warning, TEXT("RecordLog called with empty UUID."));
		return -1;
	}

	if (!Message.JsonObject->HasField(MESSAGE_FIELD))
	{
		UE_LOG(LogRL, Warning, TEXT("RecordLog called with JSON object missing 'Message' field."));
		return -1;
	}

	if (!this->bAllowSameMessage && IsSameMessage(Message))
	{
		UE_LOG(LogRL, Warning, TEXT("RecordLog called with same message within 30 seconds and bAllowSameMessage is false."));
		return -1;
	}

	this->LogDb.Add(In_UUID, Message);
	
	FString MessageString;
	if (!Message.JsonObjectToString(MessageString))
	{
		UE_LOG(LogRL, Warning, TEXT("RecordLog failed to convert JSON object to string."));
		return -1;
	}

	if (MessageString.IsEmpty())
	{
		UE_LOG(LogRL, Warning, TEXT("RecordLog called with empty message string."));
		return -1;
	}

	FString LevelString;
	ERuntimeLogLevels VerbosityLevel = ERuntimeLogLevels::Info;

	if (Message.JsonObject->TryGetStringField(VERBOSITY_FIELD, LevelString))
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
		Message.JsonObject->SetStringField(TEXT("UUID"), In_UUID);

		FString FileString;
		Message.JsonObjectToString(FileString);
		const FString FileEntry = FileString + "\n";

		TStringConversion<TStringConvert<FString::ElementType, UTF8CHAR>> StringConverter = StringCast<UTF8CHAR>(*FileEntry);
		this->LogFileStream.write((const char*)StringConverter.Get(), StringConverter.Length());
		this->LogFileStream.flush();

		this->LastLog = MessageString;
		this->LastLogTime = FDateTime::UtcNow();

		return 1;
	}

	else
	{
		return 0;
	}
}

void URuntimeLoggerSubsystem::ResetLogs()
{
	this->CleanupLogs();
	this->OpenLogFile();
}

TMap<FString, FJsonObjectWrapper> URuntimeLoggerSubsystem::GetLogDb() const
{
	return this->LogDb;
}

FString URuntimeLoggerSubsystem::GetLogFilePath() const
{
	FString TempPath = this->LogFilePath;
	FPaths::NormalizeFilename(TempPath);
	return TempPath;
}

FJsonObjectWrapper URuntimeLoggerSubsystem::GetLog(const FString& UUID) const
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