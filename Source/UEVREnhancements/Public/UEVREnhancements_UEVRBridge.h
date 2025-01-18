

#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "UEVREnhancements_UEVRBridge.generated.h"

/**
 *
 */
UCLASS()
class UEVRENHANCEMENTS_API UUEVREnhancements_UEVRBridge : public UGameInstanceModule
{
	GENERATED_BODY()

public:
	/** Log debug message */
	UFUNCTION(BlueprintCallable, Category="Default")
	void DebugLog(FString Label, FText Text);

	/** Called by UEVR plugin. Updates state of main controller buttons. */
	void UpdateButtonState(bool A, bool B, bool X, bool Y, bool LS, bool LG, bool RS, bool RG, bool Start);

	/** Called by UEVR plugin. Updates state of Left Stick. */
	void UpdateLeftStickState(int32 X, int32 Y);

	/** Called by UEVR plugin. Updates state of Right Stick. */
	void UpdateRightStickState(int32 X, int32 Y);

	/** Called by UEVR plugin. Updates state of Left Trigger. */
	void UpdateLeftTriggerState(bool LT);

	/** Called by UEVR plugin. Updates state of Right Trigger. */
	void UpdateRightTriggerState(bool RT);

	/** Called by UEVR plugin when UEVR injects (switching game to VR mode).  */
	void InitUEVRBridge(FString Profile, FString UEVR);

	/** Called on Tick to translate a button state into an Input Action. */
	void CheckButtonAction(bool Condition, const UInputAction* Action);

	/** Called on Tick to translate a stick position into an Input Action. */
	void CheckStickPosAction(double X, double Y, const UInputAction* Action, bool _isNotZero);

	/** Starts a new haptics effect for the right controller. */
	UFUNCTION(BlueprintCallable)
	void SetHapticsRightEffect(FS_VRHapticEffect effect);

	/** Continues a repeated haptics effect for the right controller. */
	void RepeatHapticsRightEffect(int32 _repeat);

	/** Starts a new haptics effect for the left controller. */
	UFUNCTION(BlueprintCallable, Category="Default")
	void SetHapticsLeftEffect(FS_VRHapticEffect effect);

	/** Continues a repeated haptics effect for the left controller. */
	void RepeatHapticsLeftEffect(int32 _repeat);

	/** Compares the current UEVR API version against the provided version and returns true if the same or newer. */
	UFUNCTION(BlueprintPure)
	void CheckAPIVersion(FString MinVersion, bool& Valid, TArray<int32> _minVersion, TArray<int32> _actualVersion, bool _result);

	/** Given a semantic version as a string, converts and returns it to as an array of integers for numeric comparison. */
	UFUNCTION(BlueprintCallable)
	void VersionStringToArrayInt(FString Version, TArray<int32>& VersionInts, TArray<int32> _arrayInts, FString _stringRemaining);

public:
	/** Indicates if UEVR has been initialised aka injected. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Default")
	bool IsInitialised;

	/** Please add a variable description */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUEVRBridgeInitialised);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="Default")
	FUEVRBridgeInitialised UEVRBridgeInitialised;

	/** Please add a variable description */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateUIInteraction, bool, UIInteraction);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="Default")
	FUpdateUIInteraction UpdateUIInteraction;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	FString ProfileVersion;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default", meta=(MultiLine="true"))
	FString APIVersion;

private:
	/** Current button and stick states */
	bool button_x;
	bool button_y;
	bool button_a;
	bool button_b;
	bool button_left_stick;
	bool button_left_grip;
	bool button_left_trigger;
	bool button_right_stick;
	bool button_right_grip;
	bool button_right_trigger;
	bool button_start;
	double stick_left_x;
	double stick_left_y;
	double stick_right_x;
	double stick_right_y;

	/** Map to track state of active actions */
	TMap<UInputAction*,bool> LastActions;

	/** Please add a variable description */
	bool UIInteractMode;

	/** Please add a variable description */
	bool GameAimMode;

	/** Read by UEVR to change Movement Mode. 0 = Game, 1 = HMD */
	int32 MovementMode;

	/** Read by UEVR to detect if there is a Haptic event to process. */
	bool HapticsLeftPending;
	bool HapticsRightPending;

	/** Please add a variable description */
	FS_VRHapticEffect HapticsLeftEffect;
	FS_VRHapticEffect HapticsRightEffect;

	/** Haptics event breakdown read by UEVR - can be phased out if UEVR can read HapticsLeft/RightEffect struct? */
	double haptics_left_duration;
	double haptics_left_frequency;
	double haptics_left_amplitude;
	double haptics_right_duration;
	double haptics_right_frequency;
	double haptics_right_amplitude;
};
