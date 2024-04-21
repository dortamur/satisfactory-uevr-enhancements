
#pragma once

#include "Framework/Application/SlateApplication.h"
#include "MouseEmulation.generated.h"

UCLASS()
class UMouseEmulation : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void SimulateMouseButton(bool bLeft, bool bButtonDown, bool bWidgetsOnly, bool bWorldOnly);
};
