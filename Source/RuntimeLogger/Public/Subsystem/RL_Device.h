#pragma once
#include "CoreMinimal.h"

class URuntimeLoggerSubsystem;

class FRuntimeLoggerOutput : public FOutputDevice
{

private:

	URuntimeLoggerSubsystem* LoggerSubsystem = nullptr;

public:

	virtual void InitSubsystem(URuntimeLoggerSubsystem* In_LoggerSubsystem);

	virtual bool CanBeUsedOnAnyThread() const override;
	virtual bool CanBeUsedOnMultipleThreads() const override;
	virtual bool CanBeUsedOnPanicThread() const override;
	virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

};