#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonUtilities.h"
#include "JsonObjectWrapper.h"

#include "RL_Statics.generated.h"

UENUM(BlueprintType)
enum class ERuntimeLogLevels : uint8
{
	Info = 0		UMETA(DisplayName = "Info"),
	Warning = 1		UMETA(DisplayName = "Warning"),
	Critical = 2	UMETA(DisplayName = "Critical"),
};
ENUM_CLASS_FLAGS(ERuntimeLogLevels)

UCLASS()
class URL_Static_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log Message", Keywords = "log, display, string, result, return, code"), Category = "Frozen Forest|Runtime Logger")
	static void LogMessage(int32 InLogLevel, FJsonObjectWrapper In_Log);

};