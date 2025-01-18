
#include "UEVREnhancements_UEVRBridge.h"
#include "Math/UnrealMathUtility.h"


float ScaleStickPosition(int32 IntPos) {
  return FMath::Clamp(static_cast<float>(IntPos) / 32000.0f, -1.0f, 1.0f);
}

TArray<FName> InputActionPaths = {
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftButtonX"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftButtonY"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightButtonA"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightButtonB"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftClick"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftGrip"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftTrigger"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightClick"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightGrip"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightTrigger"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftStick"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightStick"),
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRStartButton")
};

UUEVREnhancements_UEVRBridge::UUEVREnhancements_UEVRBridge() {
  for (const FName& Path : InputActionPaths)
  {
    UInputAction* Action = LoadObject<UInputAction>(nullptr, *Path.ToString());
    if (Action)
    {
      this->InputActions.Add(Action);
      this->DebugLog(FString::Printf(TEXT("Loaded Input Action: %s"), *Path.ToString()));
    }
    else
    {
      this->DebugLog(FString::Printf(TEXT("Failed to load Input Action: %s"), *Path.ToString()));
    }
  }
}

void UUEVREnhancements_UEVRBridge::DispatchLifecycleEvent(ELifecyclePhase Phase) {
    //Register default content before calling blueprint event logic
    if (Phase == ELifecyclePhase::INITIALIZATION) {
        UE_LOG(UEVREnhancements, Verbose, TEXT("[UEVRBridge] Initialisation!"));
    }

    Super::DispatchLifecycleEvent(Phase);
}

void UUEVREnhancements_UEVRBridge::DebugLog(FString DebugString) {
  UE_LOG(UEVREnhancements, Verbose, TEXT("[UEVRBridge] %s"), *DebugString);
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
  this->DebugLog(debug_msg);
}

/** Called by UEVR plugin. Updates state of Left Stick. */
void UUEVREnhancements_UEVRBridge::UpdateLeftStickState(int32 X, int32 Y) {
  this->stick_left_x = ScaleStickPosition(X);
  this->stick_left_y = ScaleStickPosition(Y);
  FString debug_msg = FString::Printf(TEXT("Left Stick: X=%d Y=%d SX=%.2f SY=%.2f"), X, Y, this->stick_left_x, this->stick_left_y);
  this->DebugLog(debug_msg);
}

/** Called by UEVR plugin. Updates state of Right Stick. */
void UUEVREnhancements_UEVRBridge::UpdateRightStickState(int32 X, int32 Y) {
  this->stick_right_x = ScaleStickPosition(X);
  this->stick_right_y = ScaleStickPosition(Y);
  FString debug_msg = FString::Printf(TEXT("Right Stick: X=%d Y=%d SX=%.2f SY=%.2f"), X, Y, this->stick_right_x, this->stick_right_y);
  this->DebugLog(debug_msg);
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
void UUEVREnhancements_UEVRBridge::InitUEVRBridge(FString Profile, FString UEVR) {
  this->ProfileVersion = Profile;
  this->APIVersion = UEVR;
  this->IsInitialised = true;

  this->DebugLog(FString::Printf(TEXT("Init UEVRBridge: Profile=%s UEVR=%s"), *Profile, *UEVR));

  UButtonHintBarFix::RegisterUEVRFixHooks();

  if (UEVRBridgeInitialised.IsBound())
  {
    UEVRBridgeInitialised.Broadcast();
  }
}

void UUEVREnhancements_UEVRBridge::ButtonActionsTick() {
  this->CheckButtonAction(this->button_x, this->InputActions[0]);
  this->CheckButtonAction(this->button_y, this->InputActions[1]);
  this->CheckButtonAction(this->button_a, this->InputActions[2]);
  this->CheckButtonAction(this->button_b, this->InputActions[3]);
  this->CheckButtonAction(this->button_left_stick, this->InputActions[4]);
  this->CheckButtonAction(this->button_left_grip, this->InputActions[5]);
  this->CheckButtonAction(this->button_left_trigger, this->InputActions[6]);
  this->CheckButtonAction(this->button_right_stick, this->InputActions[7]);
  this->CheckButtonAction(this->button_right_grip, this->InputActions[8]);
  this->CheckButtonAction(this->button_right_trigger, this->InputActions[9]);
  this->CheckButtonAction(this->button_start, this->InputActions[10]);
}

/** Called on Tick to translate a button state into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckButtonAction(bool Condition, const UInputAction *Action) {
  bool last_state = LastActions[Action];

  this->DebugLog(FString::Printf(TEXT("CheckButtonAction Tick: %s => %s"), last_state ? TEXT("true") : TEXT("false"), Condition ? TEXT("true") : TEXT("false")));
  if (Condition || last_state) {
    DoInputAction.Broadcast(Condition, Action);
  }

  if (Condition != last_state) {
    LastActions[Action] = Condition;
  }
}

/** Called on Tick to translate a stick position into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckStickPosAction(double X, double Y, const UInputAction *Action,
                                                       bool _isNotZero) {}

/** Starts a new haptics effect for the right controller. */
void UUEVREnhancements_UEVRBridge::SetHapticsRightEffect(FS_VRHapticEffect effect) {
  // TODO: Migrate from Blueprint
}

/** Continues a repeated haptics effect for the right controller. */
void UUEVREnhancements_UEVRBridge::RepeatHapticsRightEffect(int32 _repeat) {}

/** Starts a new haptics effect for the left controller. */
void UUEVREnhancements_UEVRBridge::SetHapticsLeftEffect(FS_VRHapticEffect effect) {\
  // TODO: Migrate from Blueprint
}

/** Continues a repeated haptics effect for the left controller. */
void UUEVREnhancements_UEVRBridge::RepeatHapticsLeftEffect(int32 _repeat) {}

/** Compares the current UEVR API version against the provided version and returns true if the same or newer. */
void UUEVREnhancements_UEVRBridge::CheckAPIVersion(FString MinVersion, bool &Valid) {
  TArray<int32> _minVersion, _actualVersion;
  VersionStringToArrayInt(this->APIVersion, _actualVersion);
  VersionStringToArrayInt(MinVersion, _minVersion);

  // Compare versions at each level
  for (int32 i = 0; i < FMath::Min(_actualVersion.Num(), _minVersion.Num()); ++i)
  {
      if (_actualVersion[i] > _minVersion[i])
      {
          Valid = true;
          return;
      }
      else if (_actualVersion[i] < _minVersion[i])
      {
          Valid = false;
          return;
      }
  }

  // All numbers equal - is one longer (minor revision)?
  Valid = _actualVersion.Num() >= _minVersion.Num();
}

/** Given a semantic version as a string, converts and returns it to as an array of integers for numeric comparison. */
void UUEVREnhancements_UEVRBridge::VersionStringToArrayInt(FString Version, TArray<int32> &VersionInts) {
  // Clear the output array
  VersionInts.Empty();

  // Split the version string by dots
  TArray<FString> VersionParts;
  Version.ParseIntoArray(VersionParts, TEXT("."), true);

  // Convert each part to an integer and add to the output array
  for (const FString& Part : VersionParts)
  {
      int32 VersionNumber = FCString::Atoi(*Part);
      VersionInts.Add(VersionNumber);
  }
}
