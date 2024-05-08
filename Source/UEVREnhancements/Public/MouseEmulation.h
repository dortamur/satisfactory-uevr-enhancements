
#pragma once

#include "Framework/Application/SlateApplication.h"
#include "MouseEmulation.generated.h"

UCLASS()
class UMouseEmulation : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void SimulateMouseButton(const FKey MouseButton, bool bButtonDown, bool bWidgetsOnly, bool bWorldOnly);

  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void SimulateMouseScroll(float ScrollAmount);
};
