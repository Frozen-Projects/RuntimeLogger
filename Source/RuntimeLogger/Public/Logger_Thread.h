#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

// Forward Declarations.
class FRunnableThread;
class URuntimeLoggerSubsystem;

class FRuntimeLogger_Thread : public FRunnable
{

public:

	// Sets default values for this actor's properties.
	FRuntimeLogger_Thread(URuntimeLoggerSubsystem* Manager);

	// Destructor.
	virtual ~FRuntimeLogger_Thread() override;

	virtual bool Init() override;

	// Running on runnable thread.
	virtual uint32 Run() override;

	// You need to clean up the thread manually after calling this function.
	virtual void Stop() override;

	virtual void TriggerWakeEvent();
	virtual void WaitForWakeEvent();

private:

	FRunnableThread* RunnableThread = nullptr;
	const FString ThreadName = "RuntimeLogger_Thread";

	URuntimeLoggerSubsystem* LoggerSubsystem = nullptr;

	FThreadSafeBool bIsRunning = false;
	FThreadSafeBool bIsPaused = false;
	FEvent* WakeEvent = nullptr;
};