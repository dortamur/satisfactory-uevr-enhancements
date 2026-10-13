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

	/** Called by UEVR plugin. Updates state of a single button. InputButton is an EVRInputButton value. */
	UFUNCTION(BlueprintCallable)
	void UpdateInputButton(int32 InputButton, bool Pressed);

	/** Called by UEVR plugin. Updates the state of a stick. InputStick is an EVRInputStick value, X/Y are raw XInput values. */
	UFUNCTION(BlueprintCallable)
	void UpdateInputStick(int32 InputStick, int32 X, int32 Y);

	/** Deprecated (pre 1.2.3 UEVR profile): use UpdateInputButton. */
	UFUNCTION(BlueprintCallable)
	void UpdateButtonState(bool A, bool B, bool X, bool Y, bool LS, bool LG, bool RS, bool RG, bool Start);

	/** Deprecated (pre 1.2.3 UEVR profile): use UpdateInputStick. */
	UFUNCTION(BlueprintCallable)
	void UpdateLeftStickState(int32 X, int32 Y);

	/** Deprecated (pre 1.2.3 UEVR profile): use UpdateInputStick. */
	UFUNCTION(BlueprintCallable)
	void UpdateRightStickState(int32 X, int32 Y);

	/** Deprecated (pre 1.2.3 UEVR profile): use UpdateInputButton. */
	UFUNCTION(BlueprintCallable)
	void UpdateLeftTriggerState(bool LT);

	/** Deprecated (pre 1.2.3 UEVR profile): use UpdateInputButton. */
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

	/** Manually enable/disable UObjectHook updates. */
	UFUNCTION(BlueprintCallable)
	void SetUObjectHookDisabled(bool disabled);

	/** Set UEVR mod value. */
	UFUNCTION(BlueprintCallable)
	void SetUEVRModValue(FString property, FString value);

	/** Update the UEVR state based on new player state. */
	UFUNCTION(BlueprintCallable)
	void UpdateVRPlayerState(EVRPlayerState NewPlayerState, int32 DefaultMovementMode);

	/** Function for UEVR hook to Recenter View. */
	UFUNCTION(BlueprintCallable)
	void RecenterView();

	/** Called by UEVR plugin when UEVR injects (switching game to VR mode).  */
	UFUNCTION(BlueprintCallable)
	void InitUEVRBridge(FString Profile, FString UEVR, FString BuildDate = FString(""), FString CommitHash = FString(""), FString Branch = FString(""));

	/** Called on Tick to check button states and call input actions as required. */
	UFUNCTION(BlueprintCallable)
	void InputActionsTick();

	/** Translate a button state into an Input Action call. Slot is the (already mirrored) action slot. */
	void CheckInputButtonAction(bool Condition, EVRInputButton Slot);

	/** Called on Tick to translate a stick position into an Input Action. Slot is the (already mirrored) action slot. */
	void CheckInputStickAction(const FVector2D& Pos, EVRInputStick Slot);

	/** Returns the left/right swapped equivalent of an input button (used for Left Hand Mode). */
	static EVRInputButton MirrorInputButton(EVRInputButton InputButton);

	/** Loads the Input Action assets (once). */
	void LoadInputActions();

	/** Sends End events for any held actions and clears tracked state (used when the mapping changes). */
	void ReleaseHeldActions();

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
	void CheckUEVRVersion(FString MinVersion, FString CommitHash, FString BuildDate, bool& Valid);

	/** Given a semantic version as a string, converts and returns it to as an array of integers for numeric comparison. */
	UFUNCTION(BlueprintCallable)
	void VersionStringToArrayInt(FString Version, TArray<int32>& VersionInts);

	// Given a string of format DD.MM.YY, return an Integer representation for simple comparison
	UFUNCTION(BlueprintCallable)
	int32 DateStringToInt(FString Date);

	virtual void DispatchLifecycleEvent(ELifecyclePhase Phase) override;

public:
	/** Indicates if UEVR has been initialised aka injected. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="UEVR")
	bool IsInitialised;

	/** Called when an Input Action event needs to be handled, to pass to a Blueprint for processing. */
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDoInputAction, bool, Condition, const UInputAction*, Action, EVRInputActionState, ActionState);
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

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	FString UEVRBuildDate;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	FString UEVRCommitHash;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	FString UEVRBranch;

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

	/** Whether UEVR UObjectHook is enabled or disabled (by this mod) */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
  bool UObjectHookDisabled;

	/** Name of the UEVR mod property to set. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	FString UEVRModPropName;

	/** Value of the UEVR mod property to set. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="UEVR")
	FString UEVRModPropValue;

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
	static constexpr int32 NumInputButtons = static_cast<int32>(EVRInputButton::Count);
	static constexpr int32 NumInputSticks = static_cast<int32>(EVRInputStick::Count);

	/** Current physical states, indexed by EVRInputButton / EVRInputStick. */
	bool ButtonStates[NumInputButtons] = {};
	FVector2D StickStates[NumInputSticks];

	/** Last states broadcast, indexed by (mirrored) action slot. */
	bool LastButtonStates[NumInputButtons] = {};
	bool LastStickStates[NumInputSticks] = {};

	EVRPlayerState PlayerState;

	// Input Action asset paths, indexed by EVRInputButton / EVRInputStick.
	static TArray<FName> InputButtonActionPaths;
	static TArray<FName> InputStickActionPaths;

	// Loaded Input Actions, indexed by EVRInputButton / EVRInputStick. Entries may be null if an asset failed to load.
	UPROPERTY()
	TArray<TObjectPtr<UInputAction>> InputButtonActions;
	UPROPERTY()
	TArray<TObjectPtr<UInputAction>> InputStickActions;

	bool InputActionsLoaded = false;

};
