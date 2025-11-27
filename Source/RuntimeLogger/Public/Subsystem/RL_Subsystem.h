// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "RL_Statics.h"

#include "RL_Subsystem.generated.h"

// Forward Declarations.
class FRuntimeLoggerOutput;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateRLOnLogReceived, FString, Out_UUID, FString, Out_Log, ERuntimeLogLevels, Out_Level);

UCLASS()
class RUNTIMELOGGER_API URuntimeLoggerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
private:
	
	TMap<FString, FJsonObjectWrapper> LogDb;
	FRuntimeLoggerOutput* LogCaptureDevice = nullptr;

	FString LogFilePath;
	TUniquePtr<IFileHandle> LogFileHandle;
	FThreadSafeBool bIsFirstEntry = true;

	virtual void OpenLogFile();
	static ERuntimeLogLevels GetLogLevelFromString(FString LogLevelString);

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual bool RecordLog(const FString& In_UUID, FJsonObjectWrapper Message);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual TMap<FString, FJsonObjectWrapper> GetLogDb();

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	virtual void CleanupLogs();

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual FString GetLogFilePath() const;

	UFUNCTION(BlueprintPure, Category = "Frozen Forest|Runtime Logger")
	virtual FJsonObjectWrapper GetLog(const FString& UUID);

	UPROPERTY(BlueprintAssignable, Category = "Frozen Forest|Runtime Logger")
	FDelegateRLOnLogReceived Delegate_Runtime_Logger;

};