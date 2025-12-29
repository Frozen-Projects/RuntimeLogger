#pragma once
#include "CoreMinimal.h"

class URuntimeLoggerGameInstance;

class FRuntimeLoggerOutput : public FOutputDevice
{

private:

	URuntimeLoggerGameInstance* GI_Logger = nullptr;

public:

	virtual void Init_GI(URuntimeLoggerGameInstance* In_GI);

	virtual bool CanBeUsedOnAnyThread() const override;
	virtual void Serialize(const TCHAR* Message, ELogVerbosity::Type Verbosity, const FName& Category) override;

};