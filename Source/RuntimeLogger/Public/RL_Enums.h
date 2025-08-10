#pragma once

UENUM(BlueprintType)
enum class ERuntimeLogLevels : uint8
{
	Info		= 0		UMETA(DisplayName = "Info"),
	Warning		= 1		UMETA(DisplayName = "Warning"),
	Critical	= 2		UMETA(DisplayName = "Critical"),
};
ENUM_CLASS_FLAGS(ERuntimeLogLevels)