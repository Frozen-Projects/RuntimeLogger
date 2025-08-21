#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "RL_Static_Functions.generated.h"

UCLASS()
class URL_Static_Functions : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log String", Keywords = "log, display, string, result, return, code"), Category = "Frozen Forest|Runtime Logger")
	static void LogString(int32 InLogLevel, FString Log);

};