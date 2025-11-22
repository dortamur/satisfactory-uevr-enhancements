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
