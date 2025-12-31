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
        if (!MessageJson.JsonObjectFromString(Message) || !MessageJson.JsonObject->HasField(MESSAGE_FIELD))
        {
            MessageJson.JsonObject->SetStringField(MESSAGE_FIELD, Message);
        }

        if (!MessageJson.JsonObject->HasField(LOGTIME_FIELD))
        {
            const FString LogTime = FDateTime::Now().ToIso8601();
            MessageJson.JsonObject->SetStringField(LOGTIME_FIELD, LogTime);
        }

        if (!MessageJson.JsonObject->HasField(VERBOSITY_FIELD))
        {
            switch (Verbosity)
            {
                case ELogVerbosity::NoLogging:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("NoLogging"));
                    break;

                case ELogVerbosity::Fatal:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Fatal"));
                    break;

                case ELogVerbosity::Error:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Error"));
                    break;

                case ELogVerbosity::Warning:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Warning"));
                    break;

                case ELogVerbosity::Display:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Display"));
                    break;

                case ELogVerbosity::Log:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Log"));
                    break;

                case ELogVerbosity::Verbose:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Verbose"));
                    break;

                case ELogVerbosity::VeryVerbose:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("VeryVerbose"));
                    break;

                case ELogVerbosity::NumVerbosity:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("NumVerbosity"));
                    break;

                case ELogVerbosity::VerbosityMask:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("VerbosityMask"));
                    break;

                case ELogVerbosity::SetColor:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("SetColor"));
                    break;

                case ELogVerbosity::BreakOnLog:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("BreakOnLog"));
                    break;

                default:
                    MessageJson.JsonObject->SetStringField(VERBOSITY_FIELD, TEXT("Unknown"));
                    break;
            }
        }

		const FString UUID = URL_Static_Functions::GenerateUUIDv7();

        if (!IsValid(this->LoggerSubsystem) || this->LoggerSubsystem->RecordLog(UUID, MessageJson) == -1)
        {
			UE_LOG(LogRL, Warning, TEXT("%s : Failed to record the log message : %s"), *FunctionName, Message);
        }
	});
}