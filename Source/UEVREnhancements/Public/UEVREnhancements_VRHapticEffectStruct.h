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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="duration", MakeStructureDefaultValue="0.050000"))
	double duration;

	/** Frequency in Hz */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="frequency", MakeStructureDefaultValue="1000.000000"))
	double frequency;

	/** Between 0.0 and 1.0 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="amplitude", MakeStructureDefaultValue="0.500000"))
	double amplitude;

	/** Times to repeat the effect */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="repeat", MakeStructureDefaultValue="0"))
	int32 repeat;

	/** Delay in seconds between repeats */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="delay"))
	double delay;
};
