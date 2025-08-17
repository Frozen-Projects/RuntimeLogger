// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Containers/Queue.h"
#include "RL_Enums.h"

#include "RL_Subsystem.generated.h"

// Forward Declarations.
class FRuntimeLogger_Thread;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateRuntimeLogger, FString, Out_UUID, FString, Out_Log, ERuntimeLogLevels, Out_Level);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegateRuntimeLoggerReset);

UCLASS()
class RUNTIMELOGGER_API URuntimeLoggerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:

	mutable FCriticalSection LogGuard;
	TSharedPtr<FRuntimeLogger_Thread> LoggerThread;
	
	TQueue<FString> LogQueue;
	TMap<FString, FString> LogDb;

	TUniquePtr<IFileHandle> LogFileHandle;
	FString LogFilePath;
	bool bIsFirstEntry = true;

	virtual void OpenLogFile();
	virtual void CleanupLogs();
	virtual void StartLogging();
	static ERuntimeLogLevels GetLogLevelFromString(FString LogLevelString);

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void RecordMessages();
	virtual bool IsQueueEmpty();

	/**
	 * Logs a message with a unique identifier.
	 * @param Message A map containing key-value pairs of the message to log. Ideally, it should contain relevant information such as plugin name, function name, and any other details. Log time and UUID will be added automatically.
	 * @return A unique identifier for the logged message.
	 */
	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual FString LogMessage(TMap<FString, FString> Message, ERuntimeLogLevels RuntimeLogLevel = ERuntimeLogLevels::Info);

	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual TMap<FString, FString> GetLogDb();

	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual void ResetLogs();

	UFUNCTION(BlueprintPure, Category = "Runtime Logger")
	virtual FString GetLogFilePath() const;

	UFUNCTION(BlueprintPure, Category = "Runtime Logger")
	virtual FString GetLog(const FString& UUID) const;

	UPROPERTY(BlueprintAssignable, Category = "FF_RealSense")
	FDelegateRuntimeLogger Delegate_Runtime_Logger;

	UPROPERTY(BlueprintAssignable, Category = "FF_RealSense")
	FDelegateRuntimeLoggerReset Delegate_Runtime_Logger_Reset;

	UFUNCTION(BlueprintCallable, Category = "Runtime Logger")
	virtual TMap<FString, FString> JsonToMap(FString JsonString);

	/*
	* This function is just a placeholder to demonstrate the use of __FUNCSIG__ Don't use it in production code.
	* __FUNCSIG__ doesn't work on blueprints. So, you have to fill the function name manually.
	*/
	virtual void SampleFunction()
	{
		TMap<FString, FString> LogData;
		LogData.Add("PluginName", "Runtime Logger");
		LogData.Add("FunctionName", FString(ANSI_TO_TCHAR(__FUNCSIG__)));
		LogData.Add("Details", "This is a sample function for logging purposes.");

		const FString UUID = this->LogMessage(LogData, ERuntimeLogLevels::Warning);
		UE_LOG(LogTemp, Log, TEXT("SampleFunction executed: %s"), *UUID);
	}
};