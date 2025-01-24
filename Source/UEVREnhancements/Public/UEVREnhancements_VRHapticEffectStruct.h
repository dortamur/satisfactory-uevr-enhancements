#pragma once
#include "CoreMinimal.h"
#include "UEVREnhancements_VRHapticEffectStruct.generated.h"

/** Structure for Haptic event for VR controllers via UEVR. */
USTRUCT(BlueprintType)
struct FS_VRHapticEffect
{
	GENERATED_BODY()
public:
	/** Duration in seconds */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="duration", MakeStructureDefaultValue="0.01"))
	double duration = 0.01f;

	/** Frequency in Hz */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="frequency", MakeStructureDefaultValue="800.0"))
	double frequency = 800.0f;

	/** Between 0.0 and 1.0 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="amplitude", MakeStructureDefaultValue="0.5"))
	double amplitude = 0.5f;

	/** Times to repeat the effect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="repeat", MakeStructureDefaultValue="0"))
	int32 repeat = 0;

	/** Delay in seconds between repeats */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="delay", MakeStructureDefaultValue="0.0"))
	double delay = 0.0f;
};
