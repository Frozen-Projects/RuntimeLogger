#include "Subsystem/RL_Device.h"
#include "Subsystem/RL_Subsystem.h"

void FRuntimeLoggerOutput::InitSubsystem(URuntimeLoggerSubsystem* In_LoggerSubsystem)
{
	this->LoggerSubsystem = In_LoggerSubsystem;
}

bool FRuntimeLoggerOutput::CanBeUsedOnAnyThread() const
{
    return true;
}

void FRuntimeLoggerOutput::Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category)
{
	// We want both LogTemp and PrintString messages.
    if (Category != FName(TEXT("LogTemp")) && Category != FName(TEXT("LogBlueprintUserMessages")))
    {
        return;
    }

	const FString FunctionName = FString::Printf(TEXT("%s::%s"), TEXT("RuntimeLogger"), ANSI_TO_TCHAR(__FUNCTION__));

    AsyncTask(ENamedThreads::GameThread, [this, Message, Verbosity, FunctionName]()
    {
        if (!this->LoggerSubsystem)
        {
            UE_LOG(LogRL, Warning, TEXT("%s : LoggerSubsystem is not valid. Can't visualize the log message : %s"), *FunctionName, Message);
            return;
        }

        FJsonObjectWrapper MessageJson;
        if (!MessageJson.JsonObjectFromString(Message))
        {
            MessageJson.JsonObject->SetStringField(TEXT("Message"), Message);
        }

        const FString LogTime = FDateTime::Now().ToIso8601();
       
        MessageJson.JsonObject->SetStringField(TEXT("LogTime"), LogTime);

        switch (Verbosity)
        {
            case ELogVerbosity::NoLogging:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("NoLogging"));
                break;
        
            case ELogVerbosity::Fatal:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Fatal"));
                break;
        
            case ELogVerbosity::Error:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Error"));
                break;
        
            case ELogVerbosity::Warning:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Warning"));
                break;
        
            case ELogVerbosity::Display:
			    MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Display"));
                break;
        
            case ELogVerbosity::Log:
			    MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Log"));
                break;
        
            case ELogVerbosity::Verbose:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Verbose"));
                break;
        
            case ELogVerbosity::VeryVerbose:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("VeryVerbose"));
                break;
        
            case ELogVerbosity::NumVerbosity:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("NumVerbosity"));
                break;
        
            case ELogVerbosity::VerbosityMask:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("VerbosityMask"));
                break;
        
            case ELogVerbosity::SetColor:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("SetColor"));
                break;
       
            case ELogVerbosity::BreakOnLog:
			    MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("BreakOnLog"));
                break;
        
            default:
                MessageJson.JsonObject->SetStringField(TEXT("Verbosity"), TEXT("Unknown"));
                break;
        }

		const FString UUID = URL_Static_Functions::GenerateUUIDv7();

        if (!IsValid(this->LoggerSubsystem) || this->LoggerSubsystem->RecordLog(UUID, MessageJson) == -1)
        {
			UE_LOG(LogRL, Warning, TEXT("%s : Failed to record the log message : %s"), *FunctionName, Message);
        }
	});
}