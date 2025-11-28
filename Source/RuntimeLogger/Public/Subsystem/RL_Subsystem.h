// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "RL_Statics.h"
#include "Subsystem/RL_Device.h"

THIRD_PARTY_INCLUDES_START
#include <iostream>
#include <fstream>
#include <string>
THIRD_PARTY_INCLUDES_END

#include "RL_Subsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRL, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateRLOnLogReceived, FString, Out_UUID, FString, Out_Log, ERuntimeLogLevels, Out_Level);

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateRLExport, bool, IsSuccessfull, FJsonObjectWrapper, Out_Result);

UCLASS()
class RUNTIMELOGGER_API URuntimeLoggerSubsystem : public UGameInstanceSubsystem
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
	virtual bool LogFileToJson(FJsonObjectWrapper& Out_Json, FString LogFile);
	virtual bool MemoryToJson(FJsonObjectWrapper& Out_Json);

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/*
	* Return -1 : Failed to record log.
	* Return 0  : Log recorded in memory only.
	* Return 1  : Log recorded in memory and to file successfully.
	*/
	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual int32 RecordLog(const FString& In_UUID, FJsonObjectWrapper Message);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual TMap<FString, FJsonObjectWrapper> GetLogDb();

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void ResetLogs();

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual FString GetLogFilePath() const;

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual FJsonObjectWrapper GetLog(const FString& UUID);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void LogFileToJson_BP(FDelegateRLExport Delegate_Export, FString In_File);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void MemoryToJson_BP(FDelegateRLExport Delegate_Export);

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|Runtime Logger")
	FDelegateRLOnLogReceived Delegate_Runtime_Logger;

};