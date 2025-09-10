#include "UEVREnhancements_VREquipment.h"
#include "FGChargedWeapon.h"
#include "UEVREnhancements.h"
#include "Kismet/GameplayStatics.h"

static FDelegateHandle AFGChargedWeapon_SpawnChargedProjectile;
static FDelegateHandle AFGEquipment_PlayCameraAnimation;

class VRNobeliskThrowHooks {
public:
  static void TweakSpawnChargedProjectile(
      TCallScope<void (__cdecl *)(AFGChargedWeapon *,FTransform,int32)> &Scope,
      AFGChargedWeapon *ChargedWeapon, FTransform spawnTransform, int32 throwForce) {
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakSpawnChargedProjectile: Adjusting Nobelisk spawn for VR... Force: %d Transform: %s"),
           throwForce, *spawnTransform.ToString()));

    // UWorld* World = ChargedWeapon->GetWorld();
    // UGameplayStatics::GetAllActorsOfClass(World, AVRCoordinatorSystem::StaticClass(), Found);
    // if (Found.Num() > 0)
    // {
    //     AVRCoordinatorSystem* VRCoordinator = Cast<AVRCoordinatorSystem>(Found[0]);
    //     UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("VR Coordinator Subsystem: %s"), VRCoordinator ? TEXT("Found") : TEXT("Not Found"));
    // if (VRCoordinator) {
    //   UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("VR Initialised: %s"), VRCoordinator->IsVRInitialised ? TEXT("Yes") : TEXT("No"));
    //   // Get variable ControllerRight from the subsystem
    //   AVRControllerRight* ControllerRight = VRCoordinator ? VRCoordinator->ControllerRight : nullptr;
    //   UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("VR Controller Right: %s"), ControllerRight ? TEXT("Found") : TEXT("Not Found"));
    // }
    // }

    // UGameInstance* GameInstance = ChargedWeapon->GetWorld()->GetGameInstance();
    // AVRCoordinatorSystem* VRCoordinator = GameInstance->GetSubsystem<AVRCoordinatorSystem>();

    if (UUEVREnhancements_VREquipment::HasVRThrowParameters()) {
      spawnTransform = UUEVREnhancements_VREquipment::GetVRThrowTransform();
      throwForce = UUEVREnhancements_VREquipment::GetVRThrowForce();
      // ChargedWeapon->SetCurrentDispersion(0.0f); // No dispersion for VR throws
      // ChargedWeapon->mCurrentDispersion = 0.0f; // No dispersion for VR throws
      UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakSpawnChargedProjectile: Using stored VR parameters: Transform: %s Force: %d"),
             *spawnTransform.ToString(), throwForce));
    }
    Scope(ChargedWeapon, spawnTransform, throwForce);
  }

};

class VREquipmentCameraAnimHooks {
public:
  static void TweakPlayCameraAnimation(
      TCallScope<void (__cdecl *)(AFGEquipment *,UCameraAnimationSequence *)> &Scope,
      AFGEquipment *Equipment, UCameraAnimationSequence* cameraAnimationSequence) {
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakPlayCameraAnimation: Suppressing equipment camera animation for VR...")));
    // Suppress camera animations for VR
    Scope.Cancel();
  }
};

// Static variable definitions for UUEVREnhancements_VREquipment
bool UUEVREnhancements_VREquipment::bHasParams;
FTransform UUEVREnhancements_VREquipment::spawnTransform;
int32 UUEVREnhancements_VREquipment::throwForce;
AActor* UUEVREnhancements_VREquipment::originActor = nullptr;
AActor* UUEVREnhancements_VREquipment::targetActor = nullptr;

UUEVREnhancements_VREquipment::UUEVREnhancements_VREquipment() {
  UUEVREnhancements_VREquipment::bHasParams = false;
  UUEVREnhancements_VREquipment::throwForce = 0;
}

void UUEVREnhancements_VREquipment::DebugLog(FString DebugString) {
  UE_LOG(UEVREnhancements, Verbose, TEXT("[VREquipment] %s"), *DebugString);
}

void UUEVREnhancements_VREquipment::RegisterVREquipmentHooks() {
  if (!WITH_EDITOR) {
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Adding Hooks to adjust Nobelisk projectile spawn for VR...")));
    AFGChargedWeapon_SpawnChargedProjectile =
        SUBSCRIBE_METHOD(AFGChargedWeapon::SpawnChargedProjectile, &VRNobeliskThrowHooks::TweakSpawnChargedProjectile);
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Adding Hooks to override equipment camera animations for VR...")));
    AFGEquipment_PlayCameraAnimation =
        SUBSCRIBE_METHOD(AFGEquipment::PlayCameraAnimation, &VREquipmentCameraAnimHooks::TweakPlayCameraAnimation);
  }
}

void UUEVREnhancements_VREquipment::UnregisterVREquipmentHooks() {
  if (!WITH_EDITOR) {
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Removing Nobelisk projectile spawn Hooks...")));
    if (AFGChargedWeapon_SpawnChargedProjectile.IsValid()) {
      UNSUBSCRIBE_METHOD(AFGChargedWeapon::SpawnChargedProjectile, AFGChargedWeapon_SpawnChargedProjectile);
    }
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Removing equipment camera animation Hooks...")));
    if (AFGEquipment_PlayCameraAnimation.IsValid()) {
      UNSUBSCRIBE_METHOD(AFGEquipment::PlayCameraAnimation, AFGEquipment_PlayCameraAnimation);
    }
  }
}

void UUEVREnhancements_VREquipment::SetVRThrowParameters(FTransform fSpawnTransform, int32 iThrowForce) {
  // Store parameters for use in the hook
  UUEVREnhancements_VREquipment::spawnTransform = fSpawnTransform;
  UUEVREnhancements_VREquipment::throwForce = iThrowForce;
  UUEVREnhancements_VREquipment::bHasParams = true;
  UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("SetVRThrowParameters called: Transform: %s Force: %d"),
         *fSpawnTransform.ToString(), iThrowForce));
}

void UUEVREnhancements_VREquipment::ClearVRThrowParameters() {
  UUEVREnhancements_VREquipment::bHasParams = false;
  UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("ClearVRThrowParameters called")));
}

bool UUEVREnhancements_VREquipment::HasVRThrowParameters() {
  return UUEVREnhancements_VREquipment::bHasParams;
}

FTransform UUEVREnhancements_VREquipment::GetVRThrowTransform() {
  FTransform newTransform = UUEVREnhancements_VREquipment::spawnTransform;
  if (UUEVREnhancements_VREquipment::originActor) {
    newTransform.SetLocation(UUEVREnhancements_VREquipment::originActor->GetActorLocation());
  }
  // if (UUEVREnhancements_VREquipment::targetActor) {
  //   FVector targetLocation = UUEVREnhancements_VREquipment::targetActor->GetActorLocation();
  //   FVector direction = (targetLocation - newTransform.GetLocation()).GetSafeNormal();
  //   FRotator newRotation = direction.Rotation();
  //   this->DebugLog(FString::Printf(TEXT("Nobelisk Throw: Adjusting throw rotation: %s"), *newRotation.ToString());
  //   newRotation.Pitch += (3000.0f - throwForce) / 100.0f; // Slightly adjust pitch for arc
  //   this->DebugLog(FString::Printf(TEXT("Nobelisk Throw: Adding Pitch: %f => %s"), (3000.0f - throwForce) / 100.0f, *newRotation.ToString());
  //   newTransform.SetRotation(FQuat(newRotation));
  // }

  return newTransform;
}

int32 UUEVREnhancements_VREquipment::GetVRThrowForce() {
  return UUEVREnhancements_VREquipment::throwForce;
}

void UUEVREnhancements_VREquipment::SetVRThrowOrigin(AActor* origin) {
  UUEVREnhancements_VREquipment::originActor = origin;
}

void UUEVREnhancements_VREquipment::SetVRThrowTarget(AActor* target) {
  UUEVREnhancements_VREquipment::targetActor = target;
}
