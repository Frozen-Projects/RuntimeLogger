#pragma once
#include "CoreMinimal.h"

class FRuntimeLoggerOutput : public FOutputDevice
{

public:
	virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

};