
#include "UEVREnhancements_UEVRBridge.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"

float ScaleStickPosition(int32 IntPos) {
  return FMath::Clamp(static_cast<float>(IntPos) / 32000.0f, -1.0f, 1.0f);
}

TArray<FName> InputActionPaths = {
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftButtonX"),  // 0
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftButtonY"),  // 1
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightButtonA"), // 2
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightButtonB"), // 3
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftClick"),    // 4
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftGrip"),     // 5
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftTrigger"),  // 6
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightClick"),   // 7
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightGrip"),    // 8
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightTrigger"), // 9
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRStartButton"),  // 10
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftStick"),    // 11
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightStick")    // 12
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

void UUEVREnhancements_UEVRBridge::SetLeftHandMode(bool left_hand_mode) {
  if (left_hand_mode != this->LeftHandMode) {
    this->DebugLog(FString::Printf(TEXT("LeftHandMode changed: %d"), left_hand_mode));
    this->LeftHandMode = left_hand_mode;
    if (this->AimMode == 2 || this->AimMode == 3) {
      this->SetAimMode(this->LeftHandMode ? 3 : 2);
    }
  }
}

/** Manually set UEVR Aim Mode. Used for manual camera transitions eg; over-the-shoulder gesture inventory opening. */
void UUEVREnhancements_UEVRBridge::SetAimMode(int32 aim_mode) {
  if (aim_mode != this->AimMode) {
    this->DebugLog(FString::Printf(TEXT("AimMode changed: %d"), aim_mode));
    this->AimMode = aim_mode;
  }
}

/** Manually set UEVR Movement Mode. */
void UUEVREnhancements_UEVRBridge::SetMovementMode(int32 movement_mode) {
  if (movement_mode != this->MovementMode) {
    this->DebugLog(FString::Printf(TEXT("MovementMode changed: %d"), movement_mode));
    this->MovementMode = movement_mode;
  }
}

/** Manually set UEVR Roomscale Mode. */
void UUEVREnhancements_UEVRBridge::SetRoomscaleMode(bool roomscale_mode) {
  if (roomscale_mode != this->RoomscaleMode) {
    this->DebugLog(FString::Printf(TEXT("RoomscaleMode changed: %s"), roomscale_mode ? TEXT("true") : TEXT("false")));
    this->RoomscaleMode = roomscale_mode;
  }
}

/**  */
void UUEVREnhancements_UEVRBridge::UpdateVRPlayerState(EVRPlayerState NewPlayerState, int32 DefaultMovementMode) {
  if (this->PlayerState != NewPlayerState) {
    this->DebugLog(FString::Printf(TEXT("PlayerState changed: %d => %d"), static_cast<uint8>(this->PlayerState), static_cast<uint8>(NewPlayerState)));
    this->PlayerState = NewPlayerState;
  }

  // Check change to UEVR Aim Mode
  // 0 - Game Aim Mode
  // 1 - HMD Aim Mode
  // 2 - Right Hand Aim Mode
  // 3 - Left Hand Aim Mode
  int32 aim_mode = this->LeftHandMode ? 3 : 2;
  switch (NewPlayerState) {
    case EVRPlayerState::UIInteract:
    case EVRPlayerState::Vehicle:
    case EVRPlayerState::Train:
    case EVRPlayerState::Hypertube:
    case EVRPlayerState::PauseMenu:
      aim_mode = 0;
      break;
  }
  if (aim_mode != this->AimMode) {
    this->DebugLog(FString::Printf(TEXT("AimMode changed: %d"), aim_mode));
    this->AimMode = aim_mode;
  }

  // Check change to UEVR Movement Mode
  int32 movement_mode = DefaultMovementMode;
  switch (NewPlayerState) {
    case EVRPlayerState::Vehicle:
    case EVRPlayerState::Train:
      movement_mode = 0;
      break;
  }
  if (movement_mode != this->MovementMode) {
    this->DebugLog(FString::Printf(TEXT("MovementMode changed: %d"), movement_mode));
    this->MovementMode = movement_mode;
  }

  // Check change to Interaction Mode (used for streaming camera stabilisation switches)
  bool interact_mode = false;
  switch (NewPlayerState) {
    case EVRPlayerState::UIInteract:
    case EVRPlayerState::PauseMenu:
      interact_mode = true;
      break;
  }
  if (interact_mode != this->UIInteractMode) {
    this->DebugLog(FString::Printf(TEXT("UIInteractMode changed: %s"), interact_mode ? TEXT("true") : TEXT("false")));
    this->UIInteractMode = interact_mode;
  }

  // Check change to Roomscale mode
  bool roomscale_mode = true;
  // Disable "roomscale" if in Net Mode Client
  if (GetWorld()->GetNetMode() == ENetMode::NM_Client) {
    roomscale_mode = false;
  }
  switch (NewPlayerState) {
    case EVRPlayerState::Vehicle:
    case EVRPlayerState::Train:
      roomscale_mode = false;
      break;
  }
  if (roomscale_mode != this->RoomscaleMode) {
    this->DebugLog(FString::Printf(TEXT("RoomscaleMode changed: %s"), roomscale_mode ? TEXT("true") : TEXT("false")));
    this->RoomscaleMode = roomscale_mode;
  }

}

/** Function for UEVR hook to Recenter View. */
void UUEVREnhancements_UEVRBridge::RecenterView() {
  this->DebugLog(FString::Printf(TEXT("RecenterView called!")));
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

  if (false) {
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
}

/** Called by UEVR plugin. Updates state of Left Stick. */
void UUEVREnhancements_UEVRBridge::UpdateLeftStickState(int32 X, int32 Y) {
  this->stick_left_x = ScaleStickPosition(X);
  this->stick_left_y = ScaleStickPosition(Y);
  if (false) {
    FString debug_msg = FString::Printf(TEXT("Left Stick: X=%d Y=%d SX=%.2f SY=%.2f"), X, Y, this->stick_left_x, this->stick_left_y);
    this->DebugLog(debug_msg);
  }
}

/** Called by UEVR plugin. Updates state of Right Stick. */
void UUEVREnhancements_UEVRBridge::UpdateRightStickState(int32 X, int32 Y) {
  this->stick_right_x = ScaleStickPosition(X);
  this->stick_right_y = ScaleStickPosition(Y);
  if (false) {
    FString debug_msg = FString::Printf(TEXT("Right Stick: X=%d Y=%d SX=%.2f SY=%.2f"), X, Y, this->stick_right_x, this->stick_right_y);
    this->DebugLog(debug_msg);
  }
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

  // UButtonHintBarFix::RegisterUEVRFixHooks();

  if (UEVRBridgeInitialised.IsBound())
  {
    UEVRBridgeInitialised.Broadcast();
  }
}

void UUEVREnhancements_UEVRBridge::InputActionsTick() {
  if (!this->LeftHandMode) {
    // Right Hand / Default mapping
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
    this->CheckStickPosAction(this->stick_left_x, this->stick_left_y, this->InputActions[11]);
    this->CheckStickPosAction(this->stick_right_x, this->stick_right_y, this->InputActions[12]);
  } else {
    // Left Hand / Reverse mapping
    this->CheckButtonAction(this->button_x, this->InputActions[2]);
    this->CheckButtonAction(this->button_y, this->InputActions[3]);
    this->CheckButtonAction(this->button_a, this->InputActions[0]);
    this->CheckButtonAction(this->button_b, this->InputActions[1]);
    this->CheckButtonAction(this->button_left_stick, this->InputActions[7]);
    this->CheckButtonAction(this->button_left_grip, this->InputActions[8]);
    this->CheckButtonAction(this->button_left_trigger, this->InputActions[9]);
    this->CheckButtonAction(this->button_right_stick, this->InputActions[4]);
    this->CheckButtonAction(this->button_right_grip, this->InputActions[5]);
    this->CheckButtonAction(this->button_right_trigger, this->InputActions[6]);
    this->CheckButtonAction(this->button_start, this->InputActions[10]);
    this->CheckStickPosAction(this->stick_left_x, this->stick_left_y, this->InputActions[12]);
    this->CheckStickPosAction(this->stick_right_x, this->stick_right_y, this->InputActions[11]);
  }
}

/** Called on Tick to translate a button state into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckButtonAction(bool Condition, UInputAction *Action) {
  bool last_state = LastActions.FindRef(Action);

  // this->DebugLog(FString::Printf(TEXT("CheckButtonAction Tick: %s: %s => %s"), *Action->GetName(), last_state ? TEXT("true") : TEXT("false"), Condition ? TEXT("true") : TEXT("false")));
  if (Condition || last_state) {
    // this->DebugLog(FString::Printf(TEXT("CheckButtonAction Broadcast: %s = %s"), *Action->GetName(), Condition ? TEXT("true") : TEXT("false")));
    DoInputAction.Broadcast(Condition, Action);
  }

  if (Condition != last_state) {
    // this->DebugLog(FString::Printf(TEXT("CheckButtonAction Updating State: %s = %s"), *Action->GetName(), Condition ? TEXT("true") : TEXT("false")));
    LastActions.Emplace(Action, Condition);
  }
}

/** Called on Tick to translate a stick position into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckStickPosAction(double X, double Y, UInputAction *Action) {
  bool last_state = LastActions.FindRef(Action);

  bool new_state = !(FMath::IsNearlyEqual(X, 0.0, 0.001) && FMath::IsNearlyEqual(Y, 0.0, 0.001));
  if (new_state || last_state) {
    // this->DebugLog(FString::Printf(TEXT("CheckButtonAction Broadcast: %s = %s"), *Action->GetName(), Condition ? TEXT("true") : TEXT("false")));
    FVector2D StickPos = new_state ? FVector2D(X, Y) : FVector2D(0.0f, 0.0f);
    DoVectorInputAction.Broadcast(StickPos, Action);
  }

  if (new_state != last_state) {
    // this->DebugLog(FString::Printf(TEXT("CheckButtonAction Updating State: %s = %s"), *Action->GetName(), Condition ? TEXT("true") : TEXT("false")));
    LastActions.Emplace(Action, new_state);
  }
}

/** Starts a new haptics effect for the right controller. */
void UUEVREnhancements_UEVRBridge::SetHapticsRightEffect(FS_VRHapticEffect effect) {
  this->DebugLog(FString::Printf(TEXT("SetHapticsRightEffect: %f, %f, %f, %d, %f"), effect.duration, effect.frequency, effect.amplitude, effect.repeat, effect.delay));
  this->HapticsRightEffect = effect;
  this->haptics_right_duration = effect.duration;
  this->haptics_right_amplitude = effect.amplitude;
  this->haptics_right_frequency = effect.frequency;
}

/** Continues a repeated haptics effect for the right controller. */
void UUEVREnhancements_UEVRBridge::RepeatHapticsRightEffect(int32 _repeat) {}

/** Starts a new haptics effect for the left controller. */
void UUEVREnhancements_UEVRBridge::SetHapticsLeftEffect(FS_VRHapticEffect effect) {\
  // TODO: Migrate from Blueprint
  this->DebugLog(FString::Printf(TEXT("SetHapticsLeftEffect: %f, %f, %f, %d, %f"), effect.duration, effect.frequency, effect.amplitude, effect.repeat, effect.delay));
  this->HapticsLeftEffect = effect;
  this->haptics_left_duration = effect.duration;
  this->haptics_left_amplitude = effect.amplitude;
  this->haptics_left_frequency = effect.frequency;
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
