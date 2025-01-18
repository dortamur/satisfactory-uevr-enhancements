
#include "UEVREnhancements_UEVRBridge.h"
#include "Math/UnrealMathUtility.h"

float ScaleStickPosition(int32 IntPos) {
  return FMath::Clamp(static_cast<float>(IntPos) / 32000.0f, -1.0f, 1.0f);
}

UUEVREnhancements_UEVRBridge::UUEVREnhancements_UEVRBridge() {}

void UUEVREnhancements_UEVRBridge::DebugLog(FText Text) {
  UE_LOGFMT(UEVREnhancements, Verbose, TEXT("[UEVRBridge] {0}"), Text);
}

/** Called by UEVR plugin. Updates state of main controller buttons. */
void UUEVREnhancements_UEVRBridge::UpdateButtonState(bool A, bool B, bool X, bool Y, bool LS, bool LG, bool RS, bool RG,
                                                     bool Start) {
  this->button_a = A;
  this->button_b = B;
  this->button_x = X;
  this->button_y = Y;
  this->button_left_stick = LS;
  this->button_left_grip = LG;
  this->button_right_stick = RS;
  this->button_right_grip = RG;
  this->button_start = Start;

FString debug_msg = FString::Printf(TEXT("Buttons: A=%s B=%s X=%s Y=%s LS=%s RS=%s LG=%s RG=%s Start=%s"),
    A ? TEXT("true") : TEXT("false"),
    B ? TEXT("true") : TEXT("false"),
    X ? TEXT("true") : TEXT("false"),
    Y ? TEXT("true") : TEXT("false"),
    LS ? TEXT("true") : TEXT("false"),
    RS ? TEXT("true") : TEXT("false"),
    LG ? TEXT("true") : TEXT("false"),
    RG ? TEXT("true") : TEXT("false"),
    Start ? TEXT("true") : TEXT("false"));
  this->DebugLog(FText::FromString(debug_msg));
}

/** Called by UEVR plugin. Updates state of Left Stick. */
void UUEVREnhancements_UEVRBridge::UpdateLeftStickState(int32 X, int32 Y) {
  this->stick_left_x = ScaleStickPosition(X);
  this->stick_left_y = ScaleStickPosition(Y);
  FString debug_msg = FString::Printf(TEXT("Left Stick: X=%d Y=%d SX=%.2f SY=%.2f"), X, Y, this->stick_left_x, this->stick_left_y)
  this->DebugLog(FText::FromString(debug_msg));
}

/** Called by UEVR plugin. Updates state of Right Stick. */
void UUEVREnhancements_UEVRBridge::UpdateRightStickState(int32 X, int32 Y) {
  this->stick_right_x = ScaleStickPosition(X);
  this->stick_right_y = ScaleStickPosition(Y);
  FString debug_msg = FString::Printf(TEXT("Right Stick: X=%d Y=%d SX=%.2f SY=%.2f"), X, Y, this->stick_right_x, this->stick_right_y)
  this->DebugLog(FText::FromString(debug_msg));
}

/** Called by UEVR plugin. Updates state of Left Trigger. */
void UUEVREnhancements_UEVRBridge::UpdateLeftTriggerState(bool LT) {
  this->button_left_trigger = LT;
}

/** Called by UEVR plugin. Updates state of Right Trigger. */
void UUEVREnhancements_UEVRBridge::UpdateRightTriggerState(bool RT) {
  this->button_right_trigger = RT;
}

/** Called by UEVR plugin when UEVR injects (switching game to VR mode).  */
void UUEVREnhancements_UEVRBridge::InitUEVRBridge(FString Profile, FString UEVR) {}

/** Called on Tick to translate a button state into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckButtonAction(bool Condition, const UInputAction *Action) {}

/** Called on Tick to translate a stick position into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckStickPosAction(double X, double Y, const UInputAction *Action,
                                                       bool _isNotZero) {}

/** Starts a new haptics effect for the right controller. */
UFUNCTION(BlueprintCallable)
void UUEVREnhancements_UEVRBridge::SetHapticsRightEffect(FS_VRHapticEffect effect) {}

/** Continues a repeated haptics effect for the right controller. */
void UUEVREnhancements_UEVRBridge::RepeatHapticsRightEffect(int32 _repeat) {}

/** Starts a new haptics effect for the left controller. */
UFUNCTION(BlueprintCallable, Category = "Default")
void UUEVREnhancements_UEVRBridge::SetHapticsLeftEffect(FS_VRHapticEffect effect) {}

/** Continues a repeated haptics effect for the left controller. */
void UUEVREnhancements_UEVRBridge::RepeatHapticsLeftEffect(int32 _repeat) {}

/** Compares the current UEVR API version against the provided version and returns true if the same or newer. */
UFUNCTION(BlueprintPure)
void UUEVREnhancements_UEVRBridge::CheckAPIVersion(FString MinVersion, bool &Valid, TArray<int32> _minVersion,
                                                   TArray<int32> _actualVersion, bool _result) {}

/** Given a semantic version as a string, converts and returns it to as an array of integers for numeric comparison. */
UFUNCTION(BlueprintCallable)
void UUEVREnhancements_UEVRBridge::VersionStringToArrayInt(FString Version, TArray<int32> &VersionInts,
                                                           TArray<int32> _arrayInts, FString _stringRemaining) {}