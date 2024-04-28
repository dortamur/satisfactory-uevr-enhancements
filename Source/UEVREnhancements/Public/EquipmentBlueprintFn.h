
#pragma once

#include "Equipment/FGEquipment.h"
#include "FGInventoryComponent.h"
#include "FGInventoryComponentEquipment.h"
#include "EquipmentBlueprintFn.generated.h"


UCLASS()
class UEquipmentBlueprintFn : public UBlueprintFunctionLibrary
{
  GENERATED_BODY()
public:
  UFUNCTION(BlueprintCallable, Category = "Equipment")
  static void ActivateEquipmentByIndex( UFGInventoryComponentEquipment* Equipment, int32 index);

  UFUNCTION(BlueprintCallable, Category = "Equipment")
  static void SetActiveEquipment( UFGInventoryComponentEquipment* Equipment, AFGEquipment* to_equip);

  UFUNCTION(BlueprintCallable, Category = "Equipment")
  static void MultiCycleEquipment( UFGInventoryComponentEquipment* Equipment, int32 total );
};
