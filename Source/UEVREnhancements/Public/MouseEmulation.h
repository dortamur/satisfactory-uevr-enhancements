
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
  static void SimulateMouseButtonWithModifier(const FKey MouseButton, bool bButtonDown, bool bShift, bool bCtrl, bool bAlt);

  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void SimulateMouseDoubleClick(const FKey MouseButton);

  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void SimulateMouseMove(const FKey MouseButton);

  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void SimulateMouseScroll(float ScrollAmount);

  // UFUNCTION(BlueprintCallable, Category = "Utility")
  // static void MouseXYToScreenPosition(float X, float Y, FVector2D& ScreenPosition, FVector2D& WindowPosition, FVector2D& WindowSize);

  UFUNCTION(BlueprintCallable, Category = "Utility")
  static void ScaleXYToMousePosition(float X, float Y, FVector2D& MousePosition);
};
