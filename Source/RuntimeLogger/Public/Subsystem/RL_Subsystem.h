// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "RL_BPLib.h"
#include "Subsystem/RL_Device.h"

#include "RL_Subsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRL, Log, All);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateRLOnLogReceived, FString, Out_UUID, FString, Out_Log, ERuntimeLogLevels, Out_Level);

// We used game instance rather than subsystem to store log data because only its ``BeginDestroy`` is guaranteed to be called on application exit and it prevents double log while capturing logs at ``EventEndPlay``.
UCLASS()
class RUNTIMELOGGER_API URuntimeLoggerGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:

	TUniquePtr<FRuntimeLoggerOutput> LogCaptureDevice;

	FString LogFilePath;
	std::filebuf LogFileBuffer;
	std::ostream LogFileStream{ nullptr };
	TMap<FString, FJsonObjectWrapper> LogDb;

	static ERuntimeLogLevels GetLogLevelFromString(const FString& LogLevelString);

	virtual void OpenLogFile();
	virtual void CleanupLogs();
	virtual bool MemoryToJson(FJsonObjectWrapper& Out_Json);

public:
	
	virtual void Init() override;
	virtual void Shutdown() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual FString GetLogFilePath() const;

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void ResetLogs();

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual TMap<FString, FJsonObjectWrapper> GetLogDb() const;

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual FJsonObjectWrapper GetLog(const FString& UUID);

	/*
	* root component name is "root".
	*/
	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void MemoryToJson_BP(FDelegateRLExport Delegate_Export);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual int32 RecordLog(const FString& In_UUID, FJsonObjectWrapper Message);

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|Runtime Logger")
	FDelegateRLOnLogReceived Delegate_Runtime_Logger;

};