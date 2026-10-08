#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "UEVREnhancements_Enumerators.h"
#include "UEVREnhancements_VRVehicle.generated.h"

/**
 *
 */
UCLASS()
class UEVRENHANCEMENTS_API UUEVREnhancements_VRVehicle : public UGameInstanceModule
{
	GENERATED_BODY()

public:
	UUEVREnhancements_VRVehicle();

	/** Log debug message */
	UFUNCTION(BlueprintCallable, Category="Default")
	void DebugLog(FString DebugString);

	UFUNCTION(BlueprintCallable, Category="Default")
	UInputAction* GetFrontHydraulicsAction();

  UFUNCTION(BlueprintCallable, Category="Default")
	UInputAction* GetAirControlAction();

	UFUNCTION(BlueprintCallable, Category="Default")
	void ThrottleSteering(class AFGWheeledVehicle *vehicle, float throttle, float brake, float steeringAxis);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	bool bHasSloopedUpExplorerInputActions;

private:
	// Array of Input Action asset paths to load in the constructor.
	static TArray<FName> InputActionPaths;

	// Array of Input Actions to the list of above input state variables.
	TArray<UInputAction*> InputActions;

	/** Map to track state of active actions */
	TMap<UInputAction*,bool> LastActions;

};
