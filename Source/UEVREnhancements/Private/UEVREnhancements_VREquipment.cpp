#include "UEVREnhancements_VREquipment.h"
#include "Equipment/FGChargedWeapon.h"
#include "Equipment/FGAmmoType.h"
#include "UEVREnhancements.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

static FDelegateHandle AFGChargedWeapon_SpawnChargedProjectile;
static FDelegateHandle AFGEquipment_PlayCameraAnimation;
static FDelegateHandle AFGEquipment_AmmoTypeFiringTransform;

class VRNobeliskThrowHooks {
public:
  static void TweakSpawnChargedProjectile(
      TCallScope<void (__cdecl *)(AFGChargedWeapon *,FTransform,int32)> &Scope,
      AFGChargedWeapon *ChargedWeapon, FTransform spawnTransform, int32 throwForce) {
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakSpawnChargedProjectile: Adjusting Nobelisk spawn for VR... Force: %d Transform: %s"),
           throwForce, *spawnTransform.ToString()));

    if (UUEVREnhancements_VREquipment::HasVRThrowParameters()) {
      // Override the spawn transform and force to use parameters based on VR throw
      spawnTransform = UUEVREnhancements_VREquipment::GetVRThrowTransform();
      throwForce = UUEVREnhancements_VREquipment::GetVRThrowForce();
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
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Suppressing equipment camera animation for VR...")));
    // Suppress camera animations for VR
    Scope.Cancel();
  }
};

class VRAmmoTypeHooks {
public:
  // Hook UFGAmmoType::SetFiringTransform to override the firing transform for Nobelisk projectiles when we have VR throw parameters set
  // This overrides the random angle offset normally added to a nobelisk throw
  static void TweakSetFiringTransform(
      TCallScope<void (__cdecl *)(UFGAmmoType *, const FTransform&)> &Scope,
      UFGAmmoType *AmmoType, const FTransform& firingTransform) {
    if (!AmmoType) {
      // AmmoType should not be null, but just in case...
      UUEVREnhancements_VREquipment::DebugLog(TEXT("TweakSetFiringTransform: AmmoType is null!"));
      Scope(AmmoType, firingTransform);
      return;
    }
    AFGWeapon* Weapon = AmmoType->GetWeapon();
    if (!Weapon) {
      // Weapon can be null! eg; if the ammo is shot from a Hog/Spitter
      UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakSetFiringTransform: Weapon is null for Ammo Type %s!"), *AmmoType->GetName()));
      Scope(AmmoType, firingTransform);
      return;
    }
    // UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakSetFiringTransform: Owner: %s Instigator: %s Player: %s"),
    //        *Weapon->GetOwner()->GetName(),
    //        *Weapon->GetInstigator()->GetName(),
    //        *UGameplayStatics::GetPlayerCharacter(Weapon->GetWorld(), 0)->GetName()));
    if (UUEVREnhancements_VREquipment::HasVRThrowParameters() && AmmoType->HasAuthority()
      && Weapon->IsA(AFGChargedWeapon::StaticClass())
      && Weapon->GetInstigator() == UGameplayStatics::GetPlayerCharacter(Weapon->GetWorld(), 0)) {
      // Override the firing transform for the Nobelisk when we have VR parameters set (to set actual rotation instead of dispersed rotation)
      FTransform throwTransform = UUEVREnhancements_VREquipment::GetVRThrowTransform();
      UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("TweakSetFiringTransform: Overriding firing transform: %s vs %s"), *firingTransform.Rotator().ToString(), *throwTransform.Rotator().ToString()));
      Scope(AmmoType, throwTransform);
    } else {
      Scope(AmmoType, firingTransform);
    }
  }
};

// Static variable definitions for UUEVREnhancements_VREquipment
bool UUEVREnhancements_VREquipment::bHasParams;
FTransform UUEVREnhancements_VREquipment::spawnTransform;
int32 UUEVREnhancements_VREquipment::throwForce;
AActor* UUEVREnhancements_VREquipment::originActor = nullptr;
AActor* UUEVREnhancements_VREquipment::targetActor = nullptr;
// TODO: Track parameters per specific players
// AFGCharacterPlayer* UUEVREnhancements_VREquipment::originPlayer = nullptr;

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
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Adding Hooks to override ammo type transform...")));
    AFGEquipment_AmmoTypeFiringTransform =
        SUBSCRIBE_METHOD(UFGAmmoType::SetFiringTransform, &VRAmmoTypeHooks::TweakSetFiringTransform);
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
    UUEVREnhancements_VREquipment::DebugLog(FString::Printf(TEXT("Removing ammo type transform Hooks...")));
    if (AFGEquipment_AmmoTypeFiringTransform.IsValid()) {
      UNSUBSCRIBE_METHOD(UFGAmmoType::SetFiringTransform, AFGEquipment_AmmoTypeFiringTransform);
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
