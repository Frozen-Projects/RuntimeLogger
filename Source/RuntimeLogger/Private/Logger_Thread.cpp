#include "Logger_Thread.h"
#include "RL_Subsystem.h"

FRuntimeLogger_Thread::FRuntimeLogger_Thread(URuntimeLoggerSubsystem* Manager)
{
	if (!IsValid(Manager))
	{
		UE_LOG(LogTemp, Fatal, TEXT("Runtime Logger Subsystem is not valid ! Cannot create thread."));
		return;
	}

	this->LoggerSubsystem = Manager;

	this->WakeEvent = FPlatformProcess::GetSynchEventFromPool();

	if (!WakeEvent)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Failed to create wake event for Runtime Logger Thread !"));
		return;
	}

	this->RunnableThread = FRunnableThread::Create(this, *this->ThreadName);

	if (!this->RunnableThread)
	{
		UE_LOG(LogTemp, Fatal, TEXT("Failed to create runnable thread for Runtime Logger !"));
		return;
	}
}

FRuntimeLogger_Thread::~FRuntimeLogger_Thread()
{
	this->Stop();

	if (this->WakeEvent)
	{
		FPlatformProcess::ReturnSynchEventToPool(this->WakeEvent);
		this->WakeEvent = nullptr;
	}

	if (this->RunnableThread)
	{
		this->RunnableThread->WaitForCompletion();
		//this->RunnableThread->Kill(true);
		
		delete this->RunnableThread;
		this->RunnableThread = nullptr;
	}
}

bool FRuntimeLogger_Thread::Init()
{
	this->bIsRunning = true;
	return true;
}

uint32 FRuntimeLogger_Thread::Run()
{
	while (this->bIsRunning)
	{
		if (this->bIsPaused && this->WakeEvent)
		{
			this->WakeEvent->Wait();
		}

		if (IsValid(this->LoggerSubsystem))
		{
			this->LoggerSubsystem->RecordMessages();
			UE_LOG(LogTemp, Log, TEXT("Runtime Logger Thread is running."));

			if (this->LoggerSubsystem->IsQueueEmpty())
			{
				this->WakeEvent->Wait();
			}
		}
	}
	
	return 0;
}

void FRuntimeLogger_Thread::Stop()
{
	this->bIsPaused = false;
	this->bIsRunning = false;

	if (this->WakeEvent)
	{
		this->WakeEvent->Trigger();
	}
}

void FRuntimeLogger_Thread::TriggerWakeEvent()
{
	this->bIsPaused = false;

	if (this->WakeEvent)
	{
		this->WakeEvent->Trigger();
	}
}

void FRuntimeLogger_Thread::WaitForWakeEvent()
{
	this->bIsPaused = true;
}