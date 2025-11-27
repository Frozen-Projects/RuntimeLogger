#include "Subsystem/RL_Subsystem.h"
#include "Subsystem/RL_Device.h"

#include "Misc/App.h"
#include "Kismet/KismetSystemLibrary.h"

void URuntimeLoggerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	this->OpenLogFile();

	LogCaptureDevice = new FRuntimeLoggerOutput();
	if (GLog)
	{
		GLog->AddOutputDevice(LogCaptureDevice);
	}
}

void URuntimeLoggerSubsystem::Deinitialize()
{
	if (GLog && this->LogCaptureDevice)
	{
		GLog->RemoveOutputDevice(this->LogCaptureDevice);
	}

	delete this->LogCaptureDevice;
	this->LogCaptureDevice = nullptr;

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
		UE_LOG(LogTemp, Error, TEXT("Failed to open log file at path: %s"), *SaveDir);
	}
}

ERuntimeLogLevels URuntimeLoggerSubsystem::GetLogLevelFromString(FString LogLevelString)
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

void URuntimeLoggerSubsystem::CleanupLogs()
{
	if (this->LogFileHandle.IsValid())
	{
		FTCHARToUTF8 Utf8(*FString("]}"));
		this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
		this->LogFileHandle->Flush();
	}

	this->LogDb.Empty();
}

bool URuntimeLoggerSubsystem::RecordLog(const FString& In_UUID, FJsonObjectWrapper Message)
{
	if (In_UUID.IsEmpty())
	{
		return false;
	}

	this->LogDb.Add(In_UUID, Message);
	
	FString MessageString;
	Message.JsonObjectToString(MessageString);

	FJsonObjectWrapper FileJson = Message;
	FileJson.JsonObject->SetStringField(TEXT("UUID"), In_UUID);

	FString FileString;
	FileJson.JsonObjectToString(FileString);
	const FString FileEntry = this->bIsFirstEntry ? "{\"root\":[" + FileString : ",\n" + FileString;

	FTCHARToUTF8 Utf8(*FileEntry);
	this->LogFileHandle->Write(reinterpret_cast<const uint8*>(Utf8.Get()), Utf8.Length());
	this->bIsFirstEntry = false;

	const FString LevelString = Message.JsonObject->GetStringField(TEXT("Verbosity"));
	const ERuntimeLogLevels VerbosityLevel = URuntimeLoggerSubsystem::GetLogLevelFromString(LevelString);

	this->Delegate_Runtime_Logger.Broadcast(In_UUID, MessageString, VerbosityLevel);
	return true;
}

TMap<FString, FJsonObjectWrapper> URuntimeLoggerSubsystem::GetLogDb()
{
	return this->LogDb;
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