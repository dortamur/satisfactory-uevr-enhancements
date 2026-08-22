
#include "UEVREnhancements_SloopedUpExplorer.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"

void UUEVREnhancements_SloopedUpExplorer::DebugLog(FString DebugString) {
  UE_LOG(UEVREnhancements, Verbose, TEXT("[VRSloopedUpExplorer] %s"), *DebugString);
}

TArray<FName> UUEVREnhancements_SloopedUpExplorer::InputActionPaths = {
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_FrontHydraulics"),  // 0
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_ShiftDown"),  // 1
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_ShiftUp"), // 2
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_TransmissionMode"), // 3
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_GamepadInAirXAxis"),    // 4
};

UUEVREnhancements_SloopedUpExplorer::UUEVREnhancements_SloopedUpExplorer() {
  for (const FName& Path : InputActionPaths)
  {
    UInputAction* Action = LoadObject<UInputAction>(nullptr, *Path.ToString());
    if (Action)
    {
      this->InputActions.Add(Action);
      this->DebugLog(FString::Printf(TEXT("Loaded Input Action: %s"), *Path.ToString()));
    }
    else
    {
      this->DebugLog(FString::Printf(TEXT("Failed to load Input Action: %s"), *Path.ToString()));
    }
  }
}

UInputAction* UUEVREnhancements_SloopedUpExplorer::GetFrontHydraulicsAction() {
    // Check if InputActions has at least 1 element to avoid out-of-bounds access
  if (InputActions.Num() < 1) {
    return nullptr;
  }
  UInputAction *Action = InputActions[0];  // Front hydraulics action is index 0
  return Action;
}

UInputAction* UUEVREnhancements_SloopedUpExplorer::GetYStickAxisAction() {
  // Check if InputActions has at least 5 elements to avoid out-of-bounds access
  if (InputActions.Num() < 5) {
    return nullptr;
  }
  UInputAction *Action = InputActions[4];  // Y stick action is index 4
  return Action;
}

