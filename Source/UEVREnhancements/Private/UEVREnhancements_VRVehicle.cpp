
#include "UEVREnhancements_VRVehicle.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "WheeledVehicles/FGWheeledVehicle.h"

void UUEVREnhancements_VRVehicle::DebugLog(FString DebugString) {
  UE_LOG(UEVREnhancements, Verbose, TEXT("[VRVehicle] %s"), *DebugString);
}

// Special input actions for optional mod SloopedUpExplorer
TArray<FName> UUEVREnhancements_VRVehicle::InputActionPaths = {
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_FrontHydraulics"),  // 0
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_ShiftDown"),  // 1
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_ShiftUp"), // 2
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_TransmissionMode"), // 3
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_GamepadInAirYAxis"),    // 4
    TEXT("/SloopedUpExplorer/Inputs/IA_SloopedUpExplorer_GamepadInAirXAxis"),    // 5
};

UUEVREnhancements_VRVehicle::UUEVREnhancements_VRVehicle() {
  bHasSloopedUpExplorerInputActions = false;
  for (const FName& Path : InputActionPaths)
  {
    UInputAction* Action = LoadObject<UInputAction>(nullptr, *Path.ToString());
    if (Action)
    {
      this->InputActions.Add(Action);
      this->DebugLog(FString::Printf(TEXT("Loaded Input Action: %s"), *Path.ToString()));
      bHasSloopedUpExplorerInputActions = true;
    }
    else
    {
      // this->DebugLog(FString::Printf(TEXT("Failed to load Input Action: %s"), *Path.ToString()));
    }
  }
}

// Mod SloopedUpExplorer support for Hydraulics input action
UInputAction* UUEVREnhancements_VRVehicle::GetFrontHydraulicsAction() {
    // Check if InputActions has at least 1 element to avoid out-of-bounds access
  if (InputActions.Num() < 1) {
    return nullptr;
  }
  UInputAction *Action = InputActions[0];  // Front hydraulics action is index 0
  return Action;
}

// Mod SloopedUpExplorer support for StickAxis actions for air control
UInputAction* UUEVREnhancements_VRVehicle::GetAirControlAction() {
  // Check if InputActions has at least 5 elements to avoid out-of-bounds access
  if (InputActions.Num() < 5) {
    return nullptr;
  }
  UInputAction *Action = InputActions[4];  // Y stick action is index 4
  return Action;
}

void UUEVREnhancements_VRVehicle::ThrottleSteering(class AFGWheeledVehicle *vehicle, float throttle, float brake, float steeringAxis) {
  // this->DebugLog(FString::Printf(TEXT("ThrottleSteering: Throttle=%f, Brake=%f, SteeringAxis=%f"), throttle, brake, steeringAxis));
  if (vehicle) {
    vehicle->MoveForward(throttle, brake);
    vehicle->MoveRight(steeringAxis);
  }
}
