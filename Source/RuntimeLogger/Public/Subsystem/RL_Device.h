#pragma once
#include "CoreMinimal.h"

#define MESSAGE_FIELD TEXT("Message")
#define LOGTIME_FIELD TEXT("LogTime")
#define VERBOSITY_FIELD TEXT("Verbosity")

class URuntimeLoggerSubsystem;

class FRuntimeLoggerOutput : public FOutputDevice
{

private:

	URuntimeLoggerSubsystem* LoggerSubsystem = nullptr;

public:

	virtual void InitSubsystem(URuntimeLoggerSubsystem* In_LoggerSubsystem);

	virtual bool CanBeUsedOnAnyThread() const override;
	virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

};