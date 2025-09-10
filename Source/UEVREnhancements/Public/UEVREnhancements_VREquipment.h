#pragma once

#include "CoreMinimal.h"
#include "Patching/NativeHookManager.h"
#include "Module/GameInstanceModule.h"
#include "UEVREnhancements_VREquipment.generated.h"

UCLASS()
class UEVRENHANCEMENTS_API UUEVREnhancements_VREquipment : public UGameInstanceModule
{
  GENERATED_BODY()

public:
	UUEVREnhancements_VREquipment();

	UFUNCTION(BlueprintCallable)
	static void DebugLog(FString DebugString);

	UFUNCTION(BlueprintCallable)
	static void RegisterVREquipmentHooks();

	UFUNCTION(BlueprintCallable)
	static void UnregisterVREquipmentHooks();

	UFUNCTION(BlueprintCallable)
	static void SetVRThrowParameters(FTransform spawnTransform, int32 throwForce);

	UFUNCTION(BlueprintCallable)
	static void ClearVRThrowParameters();

	UFUNCTION(BlueprintCallable)
	static bool HasVRThrowParameters();

	UFUNCTION(BlueprintCallable)
	static FTransform GetVRThrowTransform();

	UFUNCTION(BlueprintCallable)
	static int32 GetVRThrowForce();

	UFUNCTION(BlueprintCallable)
	static void SetVRThrowOrigin(AActor* origin);

	UFUNCTION(BlueprintCallable)
	static void SetVRThrowTarget(AActor* target);

private:
	static bool bHasParams;
	static FTransform spawnTransform;
	static AActor* originActor;
	static AActor* targetActor;
	static int32 throwForce;
};