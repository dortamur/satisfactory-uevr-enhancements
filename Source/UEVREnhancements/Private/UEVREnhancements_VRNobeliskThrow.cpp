#include "UEVREnhancements_VRNobeliskThrow.h"
#include "FGChargedWeapon.h"
#include "UEVREnhancements.h"
#include "Kismet/GameplayStatics.h"

static FDelegateHandle AFGChargedWeapon_SpawnChargedProjectile;

class VRNobeliskThrowHooks {
public:
  static void TweakSpawnChargedProjectile(
      TCallScope<void (__cdecl *)(AFGChargedWeapon *,FTransform,int32)> &Scope,
      AFGChargedWeapon *ChargedWeapon, FTransform spawnTransform, int32 throwForce) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("TweakSpawnChargedProjectile: Adjusting Nobelisk spawn for VR... Force: %d Transform: %s"),
           throwForce, *spawnTransform.ToString());

    // UWorld* World = ChargedWeapon->GetWorld();
    // UGameplayStatics::GetAllActorsOfClass(World, AVRCoordinatorSystem::StaticClass(), Found);
    // if (Found.Num() > 0)
    // {
    //     AVRCoordinatorSystem* VRCoordinator = Cast<AVRCoordinatorSystem>(Found[0]);
    //     UE_LOG(UEVREnhancements, Verbose, TEXT("VR Coordinator Subsystem: %s"), VRCoordinator ? TEXT("Found") : TEXT("Not Found"));
    // if (VRCoordinator) {
    //   UE_LOG(UEVREnhancements, Verbose, TEXT("VR Initialised: %s"), VRCoordinator->IsVRInitialised ? TEXT("Yes") : TEXT("No"));
    //   // Get variable ControllerRight from the subsystem
    //   AVRControllerRight* ControllerRight = VRCoordinator ? VRCoordinator->ControllerRight : nullptr;
    //   UE_LOG(UEVREnhancements, Verbose, TEXT("VR Controller Right: %s"), ControllerRight ? TEXT("Found") : TEXT("Not Found"));
    // }
    // }

    // UGameInstance* GameInstance = ChargedWeapon->GetWorld()->GetGameInstance();
    // AVRCoordinatorSystem* VRCoordinator = GameInstance->GetSubsystem<AVRCoordinatorSystem>();

    if (UUEVREnhancements_VRNobeliskThrow::HasVRThrowParameters()) {
      spawnTransform = UUEVREnhancements_VRNobeliskThrow::GetVRThrowTransform();
      throwForce = UUEVREnhancements_VRNobeliskThrow::GetVRThrowForce();
      UE_LOG(UEVREnhancements, Verbose, TEXT("TweakSpawnChargedProjectile: Using stored VR parameters: Transform: %s Force: %d"),
             *spawnTransform.ToString(), throwForce);
    }
    Scope(ChargedWeapon, spawnTransform, throwForce);
  }

};

// Static variable definitions for UUEVREnhancements_VRNobeliskThrow
bool UUEVREnhancements_VRNobeliskThrow::bHasParams;
FTransform UUEVREnhancements_VRNobeliskThrow::spawnTransform;
int32 UUEVREnhancements_VRNobeliskThrow::throwForce;
AActor* UUEVREnhancements_VRNobeliskThrow::originActor = nullptr;
AActor* UUEVREnhancements_VRNobeliskThrow::targetActor = nullptr;

UUEVREnhancements_VRNobeliskThrow::UUEVREnhancements_VRNobeliskThrow() {
  UUEVREnhancements_VRNobeliskThrow::bHasParams = false;
  UUEVREnhancements_VRNobeliskThrow::throwForce = 0;
}

void UUEVREnhancements_VRNobeliskThrow::RegisterVRNobeliskThrowHooks() {
  if (!WITH_EDITOR) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Adding Hooks to adjust Nobelisk projectile spawn for VR..."));
    AFGChargedWeapon_SpawnChargedProjectile =
        SUBSCRIBE_METHOD(AFGChargedWeapon::SpawnChargedProjectile, &VRNobeliskThrowHooks::TweakSpawnChargedProjectile);
  }
}

void UUEVREnhancements_VRNobeliskThrow::UnregisterVRNobeliskThrowHooks() {
  if (!WITH_EDITOR) {
    UE_LOG(UEVREnhancements, Verbose, TEXT("Removing Nobelisk projectile spawn Hooks..."));
    if (AFGChargedWeapon_SpawnChargedProjectile.IsValid()) {
      UNSUBSCRIBE_METHOD(AFGChargedWeapon::SpawnChargedProjectile, AFGChargedWeapon_SpawnChargedProjectile);
    }
  }
}

void UUEVREnhancements_VRNobeliskThrow::SetVRThrowParameters(FTransform fSpawnTransform, int32 iThrowForce) {
  // Store parameters for use in the hook
  UUEVREnhancements_VRNobeliskThrow::spawnTransform = fSpawnTransform;
  UUEVREnhancements_VRNobeliskThrow::throwForce = iThrowForce;
  UUEVREnhancements_VRNobeliskThrow::bHasParams = true;
  UE_LOG(UEVREnhancements, Verbose, TEXT("SetVRThrowParameters called: Transform: %s Force: %d"),
         *fSpawnTransform.ToString(), iThrowForce);
}

void UUEVREnhancements_VRNobeliskThrow::ClearVRThrowParameters() {
  UUEVREnhancements_VRNobeliskThrow::bHasParams = false;
  UE_LOG(UEVREnhancements, Verbose, TEXT("ClearVRThrowParameters called"));
}

bool UUEVREnhancements_VRNobeliskThrow::HasVRThrowParameters() {
  return UUEVREnhancements_VRNobeliskThrow::bHasParams;
}

FTransform UUEVREnhancements_VRNobeliskThrow::GetVRThrowTransform() {
  FTransform newTransform = UUEVREnhancements_VRNobeliskThrow::spawnTransform;
  if (UUEVREnhancements_VRNobeliskThrow::originActor) {
    newTransform.SetLocation(UUEVREnhancements_VRNobeliskThrow::originActor->GetActorLocation());
  }
  // if (UUEVREnhancements_VRNobeliskThrow::targetActor) {
  //   FVector targetLocation = UUEVREnhancements_VRNobeliskThrow::targetActor->GetActorLocation();
  //   FVector direction = (targetLocation - newTransform.GetLocation()).GetSafeNormal();
  //   FRotator newRotation = direction.Rotation();
  //   UE_LOG(UEVREnhancements, Verbose, TEXT("Nobelisk Throw: Adjusting throw rotation: %s"), *newRotation.ToString());
  //   newRotation.Pitch += (3000.0f - throwForce) / 100.0f; // Slightly adjust pitch for arc
  //   UE_LOG(UEVREnhancements, Verbose, TEXT("Nobelisk Throw: Adding Pitch: %f => %s"), (3000.0f - throwForce) / 100.0f, *newRotation.ToString());
  //   newTransform.SetRotation(FQuat(newRotation));
  // }

  return newTransform;
}

int32 UUEVREnhancements_VRNobeliskThrow::GetVRThrowForce() {
  return UUEVREnhancements_VRNobeliskThrow::throwForce;
}

void UUEVREnhancements_VRNobeliskThrow::SetVRThrowOrigin(AActor* origin) {
  UUEVREnhancements_VRNobeliskThrow::originActor = origin;
}

void UUEVREnhancements_VRNobeliskThrow::SetVRThrowTarget(AActor* target) {
  UUEVREnhancements_VRNobeliskThrow::targetActor = target;
}
