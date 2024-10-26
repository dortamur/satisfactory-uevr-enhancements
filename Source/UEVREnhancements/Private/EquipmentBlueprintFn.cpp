#include "EquipmentBlueprintFn.h"
#include "Equipment/FGEquipment.h"
#include "FGInventoryComponent.h"
#include "FGInventoryComponentEquipment.h"

/* No longer needed, as FGInventoryComponentEquipment now has SetActiveIndexWithDefaultItem - but it's equally bugged as multiplayer client. */
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
