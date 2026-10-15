#pragma once

UENUM(BlueprintType)
enum class EVRPlayerState : uint8 {
  Default               UMETA(DisplayName = "Default"),
  UIInteract            UMETA(DisplayName = "UIInteract"),
  WristInteractLeft     UMETA(DisplayName = "WristInteractLeft"),
  WristInteractRight    UMETA(DisplayName = "WristInteractRight"),
  RadialInteractLeft    UMETA(DisplayName = "RadialInteractLeft"),
  RadialInteractRight   UMETA(DisplayName = "RadialInteractRight"),
  Build                 UMETA(DisplayName = "Build"),
  Nudge                 UMETA(DisplayName = "Nudge"),
  Dismantle             UMETA(DisplayName = "Dismantle"),
  Paint                 UMETA(DisplayName = "Paint"),
  Vehicle               UMETA(DisplayName = "Vehicle"),
  Train                 UMETA(DisplayName = "Train"),
  Hypertube             UMETA(DisplayName = "Hypertube"),
  Dead                  UMETA(DisplayName = "Dead"),
  PauseMenu             UMETA(DisplayName = "PauseMenu"),
  HandMining            UMETA(DisplayName = "HandMining"),
  Swimming              UMETA(DisplayName = "Swimming"),
  Ziplining             UMETA(DisplayName = "Ziplining"),
  Portal                UMETA(DisplayName = "Portal"),
  Cinematic             UMETA(DisplayName = "Cinematic"),
  PhotoMode             UMETA(DisplayName = "PhotoMode"),
  PhotoModeUI           UMETA(DisplayName = "PhotoModeUI"),
  KeyboardUI            UMETA(DisplayName = "KeyboardUI"),
  CopySettings          UMETA(DisplayName = "CopySettings"),
  ActorMode             UMETA(DisplayName = "ActorMode")
};

UENUM(BlueprintType)
enum class EVRControllerState : uint8 {
  None                  UMETA(DisplayName = "None"),
  Controller            UMETA(DisplayName = "Controller"),
  Hand                  UMETA(DisplayName = "Hand"),
  Pointer               UMETA(DisplayName = "Pointer"),
  Hover                 UMETA(DisplayName = "Hover"),
  Equipment             UMETA(DisplayName = "Equipment"),
  Build                 UMETA(DisplayName = "Build")
};

UENUM(BlueprintType)
enum class EVRInputActionState : uint8 {
  Start                 UMETA(DisplayName = "Start"),
  Ongoing               UMETA(DisplayName = "Ongoing"),
  End                   UMETA(DisplayName = "End")
};

UENUM(BlueprintType)
enum class EVRControllerType : uint8 {
  None                  UMETA(DisplayName = "None"),
  Touch                 UMETA(DisplayName = "Touch"),
  Frame                 UMETA(DisplayName = "Frame")
};

/**
 * Physical controller buttons, named by position (not by game function).
 * The numeric values are mirrored in UEVR profile script (UEVREnhancements.lua) (INPUT_BUTTON table) - keep them in sync!
 */
UENUM(BlueprintType)
enum class EVRInputButton : uint8 {
  LeftX                 UMETA(DisplayName = "Left X"),
  LeftY                 UMETA(DisplayName = "Left Y"),
  RightA                UMETA(DisplayName = "Right A"),
  RightB                UMETA(DisplayName = "Right B"),
  LeftClick             UMETA(DisplayName = "Left Stick Click"),
  LeftGrip              UMETA(DisplayName = "Left Grip"),
  LeftTrigger           UMETA(DisplayName = "Left Trigger"),
  RightClick            UMETA(DisplayName = "Right Stick Click"),
  RightGrip             UMETA(DisplayName = "Right Grip"),
  RightTrigger          UMETA(DisplayName = "Right Trigger"),
  Start                 UMETA(DisplayName = "Start"),
  Back                  UMETA(DisplayName = "Back"),
  LeftShoulder          UMETA(DisplayName = "Left Shoulder"),
  RightShoulder         UMETA(DisplayName = "Right Shoulder"),
  DPadUp                UMETA(DisplayName = "D-Pad Up"),
  DPadDown              UMETA(DisplayName = "D-Pad Down"),
  DPadLeft              UMETA(DisplayName = "D-Pad Left"),
  DPadRight             UMETA(DisplayName = "D-Pad Right"),
  Count                 UMETA(Hidden)
};

/** Analog sticks. Numeric values are mirrored in UEVR profile script (UEVREnhancements.lua) (INPUT_STICK table). */
UENUM(BlueprintType)
enum class EVRInputStick : uint8 {
  Left                  UMETA(DisplayName = "Left Stick"),
  Right                 UMETA(DisplayName = "Right Stick"),
  Count                 UMETA(Hidden)
};
