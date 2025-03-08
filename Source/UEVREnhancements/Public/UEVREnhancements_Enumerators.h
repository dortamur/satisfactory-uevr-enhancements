#pragma once

UENUM(BlueprintType)
enum class EVRPlayerState : uint8 {
  Default               UMETA(DisplayName = "Default"),
  UIInteract            UMETA(DisplayName = "UIInteract"),
  WristInteractLeft     UMETA(DisplayName = "WristInteractLeft"),
  WristInteractRight    UMETA(DisplayName = "WristInteractRight"),
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
  Portal                UMETA(DisplayName = "Portal")
};
