#include "EquipmentBlueprintFn.h"
#include "Equipment/FGEquipment.h"
#include "FGInventoryComponent.h"
#include "FGInventoryComponentEquipment.h"

void UEquipmentBlueprintFn::ActivateEquipmentByIndex( UFGInventoryComponentEquipment* Equipment, int32 idx ) {
    AFGEquipment* to_equip = Equipment->GetStackEquipmentActorAtIdx(idx);
    Equipment->SetActiveEquipment(to_equip);
}

void UEquipmentBlueprintFn::SetActiveEquipment( UFGInventoryComponentEquipment* Equipment, AFGEquipment* to_equip ) {
    Equipment->SetActiveEquipment(to_equip);
}

void UEquipmentBlueprintFn::MultiCycleEquipment( UFGInventoryComponentEquipment* Equipment, int32 total ) {
    int32 dir;
    if (total > 0) {
        dir = 1;
    } else {
        dir = -1;
    }

    while (total != 0) {
        Equipment->CycleEquipment(dir);
        total = total - dir;
    }
}
