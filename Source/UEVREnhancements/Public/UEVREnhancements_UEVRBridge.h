#pragma once

#include "CoreMinimal.h"
#include "Module/GameInstanceModule.h"
#include "UEVREnhancements_VRHapticEffectStruct.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "UEVREnhancements_Enumerators.h"
#include "UEVREnhancements_UEVRBridge.generated.h"

/**
 *
 */
UCLASS()
class UEVRENHANCEMENTS_API UUEVREnhancements_UEVRBridge : public UGameInstanceModule
{
	GENERATED_BODY()

public:
	UUEVREnhancements_UEVRBridge();

	/** Log debug message */
	UFUNCTION(BlueprintCallable, Category="Default")
	void DebugLog(FString DebugString);

	/** Called by UEVR plugin. Updates state of main controller buttons. */
	UFUNCTION(BlueprintCallable)
	void UpdateButtonState(bool A, bool B, bool X, bool Y, bool LS, bool LG, bool RS, bool RG, bool Start);

	/** Called by UEVR plugin. Updates state of Left Stick. */
	UFUNCTION(BlueprintCallable)
	void UpdateLeftStickState(int32 X, int32 Y);

	/** Called by UEVR plugin. Updates state of Right Stick. */
	UFUNCTION(BlueprintCallable)
	void UpdateRightStickState(int32 X, int32 Y);

	/** Called by UEVR plugin. Updates state of Left Trigger. */
	UFUNCTION(BlueprintCallable)
	void UpdateLeftTriggerState(bool LT);

	/** Called by UEVR plugin. Updates state of Right Trigger. */
	UFUNCTION(BlueprintCallable)
	void UpdateRightTriggerState(bool RT);

	UFUNCTION(BlueprintCallable)
	void SetLeftHandMode(bool left_hand_mode);

	/** Manually set UEVR Aim Mode. Used for manual camera transitions eg; over-the-shoulder gesture inventory opening. */
	UFUNCTION(BlueprintCallable)
	void SetAimMode(int32 aim_mode);

	/** Manually set UEVR Movement Mode. */
	UFUNCTION(BlueprintCallable)
	void SetMovementMode(int32 movement_mode);

	/** Manually set UEVR Roomscale Mode. */
	UFUNCTION(BlueprintCallable)
	void SetRoomscaleMode(bool roomscale_mode);

	/** Update the UEVR state based on new player state. */
	UFUNCTION(BlueprintCallable)
	void UpdateVRPlayerState(EVRPlayerState PlayerState, int32 DefaultMovementMode);

	/** Function for UEVR hook to Recenter View. */
	UFUNCTION(BlueprintCallable)
	void RecenterView();

	/** Called by UEVR plugin when UEVR injects (switching game to VR mode).  */
	UFUNCTION(BlueprintCallable)
	void InitUEVRBridge(FString Profile, FString UEVR);

	/** Called on Tick to check button states and call input actions as required. */
	UFUNCTION(BlueprintCallable)
	void InputActionsTick();

	/** Translate a button state into an Input Action call. */
	void CheckButtonAction(bool Condition, UInputAction* Action);

	/** Called on Tick to translate a stick position into an Input Action. */
	void CheckStickPosAction(double X, double Y, UInputAction* Action);

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
	void CheckAPIVersion(FString MinVersion, bool& Valid);

	/** Given a semantic version as a string, converts and returns it to as an array of integers for numeric comparison. */
	UFUNCTION(BlueprintCallable)
	void VersionStringToArrayInt(FString Version, TArray<int32>& VersionInts);

	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;

public:
	/** Indicates if UEVR has been initialised aka injected. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="UEVR")
	bool IsInitialised;

	/** Called when an Input Action event needs to be handled, to pass to a Blueprint for processing. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDoInputAction, bool, Condition, const UInputAction*, Action);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="UEVR")
	FDoInputAction DoInputAction;

	/** Called when an Input Action with vector data needs to be handled, to pass to a Blueprint for processing. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDoVectorInputAction, FVector2D, StickPos, const UInputAction*, Action);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="UEVR")
	FDoVectorInputAction DoVectorInputAction;

	/** Called when UEVR is first injected and the module is initialised. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FUEVRBridgeInitialised);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="UEVR")
	FUEVRBridgeInitialised UEVRBridgeInitialised;

	/** Please add a variable description */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateUIInteraction, bool, UIInteraction);
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="UEVR")
	FUpdateUIInteraction UpdateUIInteraction;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	FString ProfileVersion;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR", meta=(MultiLine="true"))
	FString APIVersion;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	bool UIInteractMode;

	/** True if Left Handed Mode is active (swaps controllers/tracking/aim). */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	bool LeftHandMode;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	int32 AimMode;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	bool RoomscaleMode;

	/** Read by UEVR to change Movement Mode. 0 = Game, 1 = HMD */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	int32 MovementMode;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	FS_VRHapticEffect HapticsLeftEffect;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	FS_VRHapticEffect HapticsRightEffect;

	/** Haptics event breakdown read by UEVR - can be phased out if UEVR can read HapticsLeft/RightEffect struct? */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	double haptics_left_duration;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	double haptics_left_frequency;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	double haptics_left_amplitude;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	double haptics_right_duration;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	double haptics_right_frequency;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="UEVR")
	double haptics_right_amplitude;

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

	EVRPlayerState PlayerState;

	// Array of Input Actions to the list of above input state variables.
	TArray<UInputAction*> InputActions;

	/** Map to track state of active actions */
	TMap<UInputAction*,bool> LastActions;

};
