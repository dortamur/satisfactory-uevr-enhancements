
#include "UEVREnhancements_UEVRBridge.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"

float ScaleStickPosition(int32 IntPos) {
  return FMath::Clamp(static_cast<float>(IntPos) / 32000.0f, -1.0f, 1.0f);
}

// Asset paths, in EVRInputButton order. Keep in sync with EVRInputButton in UEVREnhancements_Enumerators.h!
TArray<FName> UUEVREnhancements_UEVRBridge::InputButtonActionPaths = {
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftButtonX"),    // LeftX
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftButtonY"),    // LeftY
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightButtonA"),   // RightA
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightButtonB"),   // RightB
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftClick"),      // LeftClick
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftGrip"),       // LeftGrip
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftTrigger"),    // LeftTrigger
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightClick"),     // RightClick
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightGrip"),      // RightGrip
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightTrigger"),   // RightTrigger
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRStartButton"),    // Start
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRBackButton"),     // Back (new asset)
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRShoulderLeft"),   // LeftShoulder (new asset)
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRShoulderRight"),  // RightShoulder (new asset)
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRDPadUp"),         // DPadUp (new asset)
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRDPadDown"),       // DPadDown (new asset)
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRDPadLeft"),       // DPadLeft (new asset)
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRDPadRight")       // DPadRight (new asset)
};

// Asset paths, in EVRInputStick order.
TArray<FName> UUEVREnhancements_UEVRBridge::InputStickActionPaths = {
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRLeftStick"),      // Left
    TEXT("/UEVREnhancements/Inputs/Actions/IA_VRRightStick")      // Right
};

static_assert(static_cast<int32>(EVRInputButton::Count) == 18, "InputButtonActionPaths must match EVRInputButton");
static_assert(static_cast<int32>(EVRInputStick::Count) == 2, "InputStickActionPaths must match EVRInputStick");

UUEVREnhancements_UEVRBridge::UUEVREnhancements_UEVRBridge() {
  // Input Actions are loaded in InitUEVRBridge (the constructor also runs for the CDO).
}

/** Loads the Input Action assets (once). Missing assets are left null and skipped when broadcasting. */
void UUEVREnhancements_UEVRBridge::LoadInputActions() {
  if (this->InputActionsLoaded) {
    return;
  }
  this->InputActionsLoaded = true;
  this->InputButtonActions.Reset();
  for (const FName& Path : InputButtonActionPaths) {
    UInputAction* Action = LoadObject<UInputAction>(nullptr, *Path.ToString());
    this->InputButtonActions.Add(Action);
    this->DebugLog(FString::Printf(TEXT("%s Input Button Action: %s"), Action ? TEXT("Loaded") : TEXT("Failed to load"), *Path.ToString()));
  }

  this->InputStickActions.Reset();
  for (const FName& Path : InputStickActionPaths) {
    UInputAction* Action = LoadObject<UInputAction>(nullptr, *Path.ToString());
    this->InputStickActions.Add(Action);
    this->DebugLog(FString::Printf(TEXT("%s Input Stick Action: %s"), Action ? TEXT("Loaded") : TEXT("Failed to load"), *Path.ToString()));
  }
}

/** Returns the left/right swapped equivalent of an input button (used for Left Hand Mode). */
EVRInputButton UUEVREnhancements_UEVRBridge::MirrorInputButton(EVRInputButton InputButton) {
  // TODO: Steam Frame alternate mirrored mapping
  switch (InputButton) {
    case EVRInputButton::LeftX:         return EVRInputButton::RightA;
    case EVRInputButton::LeftY:         return EVRInputButton::RightB;
    case EVRInputButton::RightA:        return EVRInputButton::LeftX;
    case EVRInputButton::RightB:        return EVRInputButton::LeftY;
    case EVRInputButton::LeftClick:     return EVRInputButton::RightClick;
    case EVRInputButton::RightClick:    return EVRInputButton::LeftClick;
    case EVRInputButton::LeftGrip:      return EVRInputButton::RightGrip;
    case EVRInputButton::RightGrip:     return EVRInputButton::LeftGrip;
    case EVRInputButton::LeftTrigger:   return EVRInputButton::RightTrigger;
    case EVRInputButton::RightTrigger:  return EVRInputButton::LeftTrigger;
    case EVRInputButton::LeftShoulder:  return EVRInputButton::RightShoulder;
    case EVRInputButton::RightShoulder: return EVRInputButton::LeftShoulder;
    default:                            return InputButton; // Start, Back
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
    // Mapping is about to change: end any held actions so none get stuck
    this->ReleaseHeldActions();
    this->LeftHandMode = left_hand_mode;
    if (this->AimMode == 2 || this->AimMode == 3) {
      this->SetAimMode(this->LeftHandMode ? 3 : 2);
    }
  }
}

/** Manually set UEVR Aim Mode. Used for manual camera transitions eg; over-the-shoulder gesture inventory opening.
 * 0 - Game Aim Mode
 * 1 - HMD Aim Mode
 * 2 - Right Hand Aim Mode
 * 3 - Left Hand Aim Mode
*/
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

/** Manually enable/disable UObjectHook updates. */
void UUEVREnhancements_UEVRBridge::SetUObjectHookDisabled(bool disabled) {
  if (disabled != this->UObjectHookDisabled) {
    this->DebugLog(FString::Printf(TEXT("UObjectHookDisabled changed: %s"), disabled ? TEXT("true") : TEXT("false")));
    this->UObjectHookDisabled = disabled;
  }
}

/** Set UEVR mod value. */
void UUEVREnhancements_UEVRBridge::SetUEVRModValue(FString property, FString value) {
  this->DebugLog(FString::Printf(TEXT("SetUEVRModValue: %s = %s"), *property, *value));
  this->UEVRModPropName = property;
  this->UEVRModPropValue = value;
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
    case EVRPlayerState::Cinematic:
    case EVRPlayerState::PhotoModeUI:
    case EVRPlayerState::KeyboardUI:
      aim_mode = 0;
      break;
    case EVRPlayerState::PhotoMode:
      aim_mode = 1;
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
    case EVRPlayerState::KeyboardUI:
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

/** Called by UEVR plugin. Updates state of a single button. InputButton is an EVRInputButton value. */
void UUEVREnhancements_UEVRBridge::UpdateInputButton(int32 InputButton, bool Pressed) {
  if (InputButton < 0 || InputButton >= NumInputButtons) {
    this->DebugLog(FString::Printf(TEXT("UpdateInputButton: invalid input button %d"), InputButton));
    return;
  }
  this->ButtonStates[InputButton] = Pressed;
}

/** Called by UEVR plugin. Updates state of a stick. InputStick is an EVRInputStick value, X/Y are raw XInput values. */
void UUEVREnhancements_UEVRBridge::UpdateInputStick(int32 InputStick, int32 X, int32 Y) {
  if (InputStick < 0 || InputStick >= NumInputSticks) {
    this->DebugLog(FString::Printf(TEXT("UpdateInputStick: invalid input stick %d"), InputStick));
    return;
  }
  this->StickStates[InputStick] = FVector2D(ScaleStickPosition(X), ScaleStickPosition(Y));
}

/** Deprecated: for older Lua scripts. Note A/B/X/Y param names follow the old Lua call order. */
void UUEVREnhancements_UEVRBridge::UpdateButtonState(bool A, bool B, bool X, bool Y, bool LS, bool LG, bool RS, bool RG,
                                                     bool Start) {
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::RightA), A);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::RightB), B);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::LeftX), X);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::LeftY), Y);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::LeftClick), LS);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::LeftGrip), LG);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::RightClick), RS);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::RightGrip), RG);
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::Start), Start);
}

/** Deprecated: for older Lua scripts. */
void UUEVREnhancements_UEVRBridge::UpdateLeftStickState(int32 X, int32 Y) {
  this->UpdateInputStick(static_cast<int32>(EVRInputStick::Left), X, Y);
}

/** Deprecated: for older Lua scripts. */
void UUEVREnhancements_UEVRBridge::UpdateRightStickState(int32 X, int32 Y) {
  this->UpdateInputStick(static_cast<int32>(EVRInputStick::Right), X, Y);
}

/** Deprecated: for older Lua scripts. */
void UUEVREnhancements_UEVRBridge::UpdateLeftTriggerState(bool LT) {
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::LeftTrigger), LT);
}

/** Deprecated: for older Lua scripts. */
void UUEVREnhancements_UEVRBridge::UpdateRightTriggerState(bool RT) {
  this->UpdateInputButton(static_cast<int32>(EVRInputButton::RightTrigger), RT);
}

/** Called by UEVR plugin when UEVR injects (switching game to VR mode).  */
void UUEVREnhancements_UEVRBridge::InitUEVRBridge(FString Profile, FString UEVR, FString BuildDate, FString CommitHash, FString Branch) {
  this->LoadInputActions();
  this->ProfileVersion = Profile;
  this->APIVersion = UEVR;
  this->UEVRBuildDate = BuildDate;
  this->UEVRCommitHash = CommitHash;
  this->UEVRBranch = Branch;
  this->IsInitialised = true;

  this->DebugLog(FString::Printf(TEXT("Init UEVRBridge: Profile=%s UEVR API=%s Build Date=%s Commit Hash=%s Branch=%s"), *Profile, *UEVR, *BuildDate, *CommitHash, *Branch));

  // UButtonHintBarFix::RegisterUEVRFixHooks();

  if (UEVRBridgeInitialised.IsBound())
  {
    UEVRBridgeInitialised.Broadcast();
  }
}

void UUEVREnhancements_UEVRBridge::SetControllerType(int32 NewControllerType) {
  EVRControllerType ControllerTypeEnum = static_cast<EVRControllerType>(NewControllerType);
  if (ControllerTypeEnum != this->ControllerType) {
    this->DebugLog(FString::Printf(TEXT("ControllerType changed: %s"), *UEnum::GetValueAsString(ControllerTypeEnum)));
    this->ControllerType = ControllerTypeEnum;

    if (UEVRBridgeControllerTypeChanged.IsBound())
    {
      UEVRBridgeControllerTypeChanged.Broadcast();
    }
  }
}

void UUEVREnhancements_UEVRBridge::InputActionsTick() {
  // In Left Hand mode, each physical input drives the mirrored action slot
  for (int32 i = 0; i < NumInputButtons; ++i) {
    const EVRInputButton Slot = this->LeftHandMode ? MirrorInputButton(static_cast<EVRInputButton>(i)) : static_cast<EVRInputButton>(i);
    this->CheckInputButtonAction(this->ButtonStates[i], Slot);
  }
  for (int32 i = 0; i < NumInputSticks; ++i) {
    const EVRInputStick Slot = this->LeftHandMode ? (i == 0 ? EVRInputStick::Right : EVRInputStick::Left) : static_cast<EVRInputStick>(i);
    this->CheckInputStickAction(this->StickStates[i], Slot);
  }
}

/** Sends End events for any held actions and clears tracked state (used when the mapping changes). */
void UUEVREnhancements_UEVRBridge::ReleaseHeldActions() {
  for (int32 i = 0; i < NumInputButtons; ++i) {
    if (this->LastButtonStates[i]) {
      this->LastButtonStates[i] = false;
      if (this->InputButtonActions.IsValidIndex(i) && this->InputButtonActions[i]) {
        DoInputAction.Broadcast(false, this->InputButtonActions[i], EVRInputActionState::End);
      }
    }
  }
  for (int32 i = 0; i < NumInputSticks; ++i) {
    if (this->LastStickStates[i]) {
      this->LastStickStates[i] = false;
      if (this->InputStickActions.IsValidIndex(i) && this->InputStickActions[i]) {
        DoVectorInputAction.Broadcast(FVector2D::ZeroVector, this->InputStickActions[i]);
      }
    }
  }
}

/** Called on Tick to translate a button state into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckInputButtonAction(bool Condition, EVRInputButton Slot) {
  const int32 Index = static_cast<int32>(Slot);
  const bool last_state = this->LastButtonStates[Index];
  UInputAction* Action = this->InputButtonActions.IsValidIndex(Index) ? this->InputButtonActions[Index].Get() : nullptr;

  if (Action && (Condition || last_state)) {
    EVRInputActionState ActionState = Condition && last_state ? EVRInputActionState::Ongoing : (Condition ? EVRInputActionState::Start : EVRInputActionState::End);
    DoInputAction.Broadcast(Condition, Action, ActionState);
  }

  this->LastButtonStates[Index] = Condition;
}

/** Called on Tick to translate a stick position into an Input Action. */
void UUEVREnhancements_UEVRBridge::CheckInputStickAction(const FVector2D& Pos, EVRInputStick Slot) {
  const int32 Index = static_cast<int32>(Slot);
  const bool last_state = this->LastStickStates[Index];
  UInputAction* Action = this->InputStickActions.IsValidIndex(Index) ? this->InputStickActions[Index].Get() : nullptr;

  const bool new_state = !(FMath::IsNearlyEqual(Pos.X, 0.0, 0.001) && FMath::IsNearlyEqual(Pos.Y, 0.0, 0.001));
  if (Action && (new_state || last_state)) {
    DoVectorInputAction.Broadcast(new_state ? Pos : FVector2D::ZeroVector, Action);
  }

  this->LastStickStates[Index] = new_state;
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
void UUEVREnhancements_UEVRBridge::CheckUEVRVersion(FString MinVersion, FString CommitHash, FString BuildDate, bool &Valid) {
  TArray<int32> _minVersion, _actualVersion;
  VersionStringToArrayInt(this->APIVersion, _actualVersion);
  VersionStringToArrayInt(MinVersion, _minVersion);

  // If a git commit is specified, and it matches, it's valid!
  if (!CommitHash.IsEmpty() && this->UEVRCommitHash == CommitHash)
  {
    this->DebugLog(FString::Printf(TEXT("UEVR commit hash match: %s == %s"), *this->UEVRCommitHash, *CommitHash));
    Valid = true;
    return;
  }

  // If a build date is required, check it's the same or newer
  if (!BuildDate.IsEmpty() && DateStringToInt(this->UEVRBuildDate) < DateStringToInt(BuildDate))
  {
    this->DebugLog(FString::Printf(TEXT("UEVR Build Date is too old: %d < %d"), DateStringToInt(this->UEVRBuildDate), DateStringToInt(BuildDate)));
    Valid = false;
    return;
  }

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
      this->DebugLog(FString::Printf(TEXT("UEVR API Version is too old: %s < %s"), *this->APIVersion, *MinVersion));
      Valid = false;
      return;
    }
  }

  // All numbers equal - is one longer (minor revision)?
  if (_actualVersion.Num() < _minVersion.Num()) {
    this->DebugLog(FString::Printf(TEXT("UEVR API Version is too old: %s < %s"), *this->APIVersion, *MinVersion));
    Valid = false;
    return;
  }

  // All tests passed!
  Valid = true;
  return;
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

// Given a string of format DD.MM.YY, return an Integer representation for simple comparison
int32 UUEVREnhancements_UEVRBridge::DateStringToInt(FString Date)
{
  TArray<FString> Parts;
  Date.ParseIntoArray(Parts, TEXT("."), true);
  int32 DateInt = 0;
  if (Parts.Num() == 3)
  {
    // Reorder DD.MM.YYYY → YYYYMMDD integer for direct comparison
    DateInt = FCString::Atoi(*Parts[2]) * 10000
            + FCString::Atoi(*Parts[1]) * 100
            + FCString::Atoi(*Parts[0]);
  }
  this->DebugLog(FString::Printf(TEXT("Date String to Int: %s => %d"), *Date, DateInt));
  return DateInt;
}