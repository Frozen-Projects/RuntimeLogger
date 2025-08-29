#include "RL_Static_Functions.h"
#include "RuntimeLogger.h"

URL_Static_Functions::URL_Static_Functions(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{

}

void URL_Static_Functions::LogString(int32 InLogLevel, FString Log)
{
    switch (InLogLevel)
    {
        case 0:

            UE_LOG(LogTemp, Display, TEXT("%s"), *FString(Log));
            return;

        case 1:

            UE_LOG(LogTemp, Warning, TEXT("%s"), *FString(Log));
            return;

        case 2:

            UE_LOG(LogTemp, Error, TEXT("%s"), *FString(Log));
            return;

        default:
            break;
    }
}