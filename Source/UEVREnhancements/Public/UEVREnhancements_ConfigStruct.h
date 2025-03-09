#pragma once
#include "CoreMinimal.h"
#include "Configuration/ConfigManager.h"
#include "Engine/Engine.h"
#include "UEVREnhancements_ConfigStruct.generated.h"

struct FUEVREnhancements_ConfigStruct_General;
struct FUEVREnhancements_ConfigStruct_ModState;
struct FUEVREnhancements_ConfigStruct_VRMovement;
struct FUEVREnhancements_ConfigStruct_Experimental;
struct FUEVREnhancements_ConfigStruct_Experimental_TestTransform;

USTRUCT(BlueprintType)
struct FUEVREnhancements_ConfigStruct_Experimental_TestTransform {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    float TestX{};

    UPROPERTY(BlueprintReadWrite)
    float TestY{};

    UPROPERTY(BlueprintReadWrite)
    float TestZ{};

    UPROPERTY(BlueprintReadWrite)
    float TestS{};

    UPROPERTY(BlueprintReadWrite)
    float TestP{};
};

USTRUCT(BlueprintType)
struct FUEVREnhancements_ConfigStruct_General {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 ReadyMsgTimeout{};

    UPROPERTY(BlueprintReadWrite)
    int32 PointerColour{};
};

USTRUCT(BlueprintType)
struct FUEVREnhancements_ConfigStruct_ModState {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    FString LastReleaseVersion{};
};

USTRUCT(BlueprintType)
struct FUEVREnhancements_ConfigStruct_VRMovement {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 MovementOrientation{};

    UPROPERTY(BlueprintReadWrite)
    int32 VRTurnType{};

    UPROPERTY(BlueprintReadWrite)
    int32 SmoothTurnRate{};

    UPROPERTY(BlueprintReadWrite)
    int32 SnapTurnAngle{};

    UPROPERTY(BlueprintReadWrite)
    bool AutoSprint{};

    UPROPERTY(BlueprintReadWrite)
    bool AutoSlide{};
};

USTRUCT(BlueprintType)
struct FUEVREnhancements_ConfigStruct_Experimental {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    bool DebugLog{};

    UPROPERTY(BlueprintReadWrite)
    bool SpringArmCamera{};

    UPROPERTY(BlueprintReadWrite)
    FUEVREnhancements_ConfigStruct_Experimental_TestTransform TestTransform{};
};

/* Struct generated from Mod Configuration Asset '/UEVREnhancements/UEVREnhancements_Config' */
USTRUCT(BlueprintType)
struct FUEVREnhancements_ConfigStruct {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    FUEVREnhancements_ConfigStruct_General General{};

    UPROPERTY(BlueprintReadWrite)
    FUEVREnhancements_ConfigStruct_ModState ModState{};

    UPROPERTY(BlueprintReadWrite)
    FUEVREnhancements_ConfigStruct_VRMovement VRMovement{};

    UPROPERTY(BlueprintReadWrite)
    FUEVREnhancements_ConfigStruct_Experimental Experimental{};

    /* Retrieves active configuration value and returns object of this struct containing it */
    static FUEVREnhancements_ConfigStruct GetActiveConfig(UObject* WorldContext) {
        FUEVREnhancements_ConfigStruct ConfigStruct{};
        FConfigId ConfigId{"UEVREnhancements", ""};
        if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull)) {
            UConfigManager* ConfigManager = World->GetGameInstance()->GetSubsystem<UConfigManager>();
            ConfigManager->FillConfigurationStruct(ConfigId, FDynamicStructInfo{FUEVREnhancements_ConfigStruct::StaticStruct(), &ConfigStruct});
        }
        return ConfigStruct;
    }
};

