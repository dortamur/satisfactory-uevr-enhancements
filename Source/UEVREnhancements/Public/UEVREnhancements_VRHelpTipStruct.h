#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UEVREnhancements_VRHelpTipStruct.generated.h"

/** Structure for Haptic event for VR controllers via UEVR. */
USTRUCT(BlueprintType)
struct FS_VRHelpTip : public FTableRowBase
{
	GENERATED_BODY()
public:
	/** Help Tip ID */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="ID"))
	FText ID;

	/** Help Tip Category */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Category"))
	FText Category;

	/** Help Tip Icon (relating to typical menu representation) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="Icon"))
	TObjectPtr<UTexture2D> Icon;

	/** Help Tip Descriptive Text */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HelpTipText", MultiLine="true"))
	FText HelpTipText;

	/** Help Tip Descriptive Text for Left-Handed Mode */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(DisplayName="HelpTipTextLH", MultiLine="true"))
	FText HelpTipTextLH;

};
