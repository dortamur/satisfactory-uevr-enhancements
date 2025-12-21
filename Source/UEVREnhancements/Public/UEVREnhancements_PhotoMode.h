#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "UEVREnhancements_PhotoMode.generated.h"

UCLASS()
class UEVRENHANCEMENTS_API UUEVREnhancements_PhotoMode : public UGameInstanceModule {
  GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Default")
	void DebugLog(FString DebugString);

	UFUNCTION(BlueprintCallable)
  void RegisterPhotoModeHooks();

  DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVRChangePhotoMode, bool, ModeEnabled);
  UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category = "VR")
  FVRChangePhotoMode VRChangePhotoMode;

	UFUNCTION(BlueprintCallable)
  void UpdateDecoupledMovement(float ForwardBack, float LeftRight, float UpDown, float Turn, float DeltaTime,
                             float DeadZone, float Scale, class UFGPhotoModeComponent *PhotoModeComponent);
};
