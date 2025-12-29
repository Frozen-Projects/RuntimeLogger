// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"

#include "RL_BPLib.h"
#include "Subsystem/RL_Device.h"

#include "RL_Subsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRL, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateRLOnLogReceived, FString, Out_UUID, FString, Out_Log, ERuntimeLogLevels, Out_Level);

UCLASS()
class RUNTIMELOGGER_API URuntimeLoggerSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
private:
	
	TMap<FString, FJsonObjectWrapper> LogDb;
	TUniquePtr<FRuntimeLoggerOutput> LogCaptureDevice;

	FString LogFilePath;
	std::filebuf LogFileBuffer;
	std::ostream LogFileStream{ nullptr };

	virtual void OpenLogFile();
	virtual void CleanupLogs();
	static ERuntimeLogLevels GetLogLevelFromString(const FString& LogLevelString);
	virtual bool MemoryToJson(FJsonObjectWrapper& Out_Json);
	virtual bool IsSameMessage(FJsonObjectWrapper In_Message);

	FString LastLog;
	FDateTime LastLogTime;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/*
	* Return -1 : Failed to record log.
	* Return 0  : Log recorded in memory only.
	* Return 1  : Log recorded in memory and to file successfully.
	*/
	virtual int32 RecordLog(const FString& In_UUID, FJsonObjectWrapper Message);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void ResetLogs();

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual TMap<FString, FJsonObjectWrapper> GetLogDb() const;

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual FString GetLogFilePath() const;

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual FJsonObjectWrapper GetLog(const FString& UUID) const;

	/*
	* root component name is "root".
	*/
	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void MemoryToJson_BP(FDelegateRLExport Delegate_Export);

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|Runtime Logger")
	FDelegateRLOnLogReceived Delegate_Runtime_Logger;
	
	//If true, allows recording same message that came in last 30 seconds.
	UPROPERTY(BlueprintReadWrite, Category = "Frozen Forest|Runtime Logger")
	bool bAllowSameMessage = false;

	// Interval in seconds to consider messages as same when bAllowSameMessage is false.
	UPROPERTY(BlueprintReadWrite, Category = "Frozen Forest|Runtime Logger")
	double SameMessageInterval = 30.0;

};