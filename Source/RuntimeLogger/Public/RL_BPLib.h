#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "RL_Includes.h"
#include "RL_UUIDv7.h"

#include "RL_BPLib.generated.h"

UENUM(BlueprintType)
enum class ERuntimeLogLevels : uint8
{
	Info = 0		UMETA(DisplayName = "Info"),
	Warning = 1		UMETA(DisplayName = "Warning"),
	Critical = 2	UMETA(DisplayName = "Critical"),
};
ENUM_CLASS_FLAGS(ERuntimeLogLevels)

UDELEGATE(BlueprintAuthorityOnly)
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateRLExport, bool, IsSuccessfull, FJsonObjectWrapper, Out_Result);

UCLASS()
class URL_Static_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static RUNTIMELOGGER_API bool LogFileToJson(FJsonObjectWrapper& Out_Json, FString LogFile);

	/*
	* root component name is "root".
	*/
	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger")
	static void LogFileToJson_BP(FDelegateRLExport Delegate_Export, FString In_File);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log Message", Keywords = "log, message, string"), Category = "Frozen Forest|Runtime Logger")
	static RUNTIMELOGGER_API void LogMessage(int32 InLogLevel, FString In_Log);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log Json", Keywords = "log, json"), Category = "Frozen Forest|Runtime Logger")
	static RUNTIMELOGGER_API void LogJson(int32 InLogLevel, FJsonObjectWrapper In_Log);

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger|UUIDv7")
	static RUNTIMELOGGER_API FString GenerateUUIDv7();

	UFUNCTION(BlueprintCallable, Category = "Frozen Forest|Runtime Logger|UUIDv7")
	static RUNTIMELOGGER_API TArray<uint8> GenerateUUIDv7Bytes();

};