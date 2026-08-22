#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "UEVREnhancements_Enumerators.h"
#include "UEVREnhancements_SloopedUpExplorer.generated.h"

/**
 *
 */
UCLASS()
class UEVRENHANCEMENTS_API UUEVREnhancements_SloopedUpExplorer : public UGameInstanceModule
{
	GENERATED_BODY()

public:
	UUEVREnhancements_SloopedUpExplorer();

	/** Log debug message */
	UFUNCTION(BlueprintCallable, Category="Default")
	void DebugLog(FString DebugString);

	UFUNCTION(BlueprintCallable, Category="Default")
	UInputAction* GetFrontHydraulicsAction();

  UFUNCTION(BlueprintCallable, Category="Default")
	UInputAction* GetYStickAxisAction();

	/** Called when an Input Action event needs to be handled, to pass to a Blueprint for processing. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDoInputAction, bool, Condition, const UInputAction*, Action, EVRInputActionState, ActionState);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="UEVR")
	FDoInputAction DoInputAction;

	/** Called when an Input Action with vector data needs to be handled, to pass to a Blueprint for processing. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDoVectorInputAction, FVector2D, StickPos, const UInputAction*, Action);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="UEVR")
	FDoVectorInputAction DoVectorInputAction;

public:

private:
	// Array of Input Action asset paths to load in the constructor.
	static TArray<FName> InputActionPaths;

	// Array of Input Actions to the list of above input state variables.
	TArray<UInputAction*> InputActions;

	/** Map to track state of active actions */
	TMap<UInputAction*,bool> LastActions;

};
