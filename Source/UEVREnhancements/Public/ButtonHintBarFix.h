#pragma once

#include "CoreMinimal.h"
#include "Patching/NativeHookManager.h"
// #include "Modules/ModuleManager.h"
// #include "Subsystem/ModSubsystem.h"
#include "ButtonHintBarFix.generated.h"

UCLASS()
class UButtonHintBarFix : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	static void RegisterUEVRFixHooks();
};