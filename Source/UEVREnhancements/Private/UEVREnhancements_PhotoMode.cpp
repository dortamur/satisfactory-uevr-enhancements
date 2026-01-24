#include "UEVREnhancements_PhotoMode.h"
#include "FGPlayerState.h"
#include "GameFramework/PlayerState.h"
#include <FGPhotoModeComponent.h>
#include <NativeHookManager.h>

void UUEVREnhancements_PhotoMode::DebugLog(FString DebugString) {
  UE_LOG(UEVREnhancements, Verbose, TEXT("[VRPhotoMode] %s"), *DebugString);
}

void UUEVREnhancements_PhotoMode::RegisterPhotoModeHooks() {
  if (!WITH_EDITOR) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Adding Hooks to Photo Mode...")));
    SUBSCRIBE_METHOD(UFGPhotoModeComponent::EnterPhotoMode, [this](auto &scope, UFGPhotoModeComponent *self) {
      UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Photo Mode Enabled")));
      if (VRChangePhotoMode.IsBound()) {
        VRChangePhotoMode.Broadcast(true);
      }
    });
    SUBSCRIBE_METHOD(UFGPhotoModeComponent::ExitPhotoMode, [this](auto &scope, UFGPhotoModeComponent *self) {
      UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Photo Mode Disabled")));
      if (VRChangePhotoMode.IsBound()) {
        VRChangePhotoMode.Broadcast(false);
      }
    });
  }
}

void UUEVREnhancements_PhotoMode::UpdateDecoupledMovement(float ForwardBack, float LeftRight, float UpDown, float Turn, float DeltaTime,
                             float DeadZone, float Scale, class UFGPhotoModeComponent *PhotoModeComponent) {
  float TimeScaled = DeltaTime * Scale;

  UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Update Decoupled Movement: Scale=%f Time=%f FB=%f LR=%f UD=%f T=%f"), Scale, DeltaTime, ForwardBack, LeftRight, UpDown, Turn));

  if (ForwardBack > DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Move Forward: %f"), ForwardBack * TimeScaled));
    PhotoModeComponent->MoveForward(ForwardBack * TimeScaled);
  } else if (ForwardBack < -DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Move Backward: %f"), -ForwardBack * TimeScaled));
    PhotoModeComponent->MoveBackwards(-ForwardBack * TimeScaled);
  }

  if (LeftRight > DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Move Right: %f"), LeftRight * TimeScaled));
    PhotoModeComponent->MoveRight(LeftRight * TimeScaled);
  } else if (LeftRight < -DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Move Left: %f"), -LeftRight * TimeScaled));
    PhotoModeComponent->MoveLeft(-LeftRight * TimeScaled);
  }

  if (UpDown > DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Move Up: %f"), UpDown * TimeScaled));
    PhotoModeComponent->MoveUp(UpDown * TimeScaled);
  } else if (UpDown < -DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Move Down: %f"), -UpDown * TimeScaled));
    PhotoModeComponent->MoveDown(-UpDown * TimeScaled);
  }

  if (Turn > DeadZone || Turn < -DeadZone) {
    UUEVREnhancements_PhotoMode::DebugLog(FString::Printf(TEXT("Turn: %f"), Turn * TimeScaled));
    PhotoModeComponent->MoveMouseX(Turn * TimeScaled);
  }
}
