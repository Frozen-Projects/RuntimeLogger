#include "RL_Statics.h"
#include "RuntimeLogger.h"

URL_Static_Functions::URL_Static_Functions(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void URL_Static_Functions::LogMessage(int32 InLogLevel, FJsonObjectWrapper In_Log)
{
    FString LogString;
	In_Log.JsonObjectToString(LogString);

    switch (InLogLevel)
    {
        case 0:

            UE_LOG(LogTemp, Display, TEXT("%s"), *FString(LogString));
            return;

        case 1:

            UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(LogString));
            return;

        case 2:

            UE_LOG(LogTemp, Error, TEXT("%s"), *FString(LogString));
            return;

        default:
            UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(LogString));
            break;
    }
}