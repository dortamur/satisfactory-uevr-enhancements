
- CFGCharacter Player -> Get Build Gun
	- Get Current State -> Get Display Name = 
		- "BP_BuildGun_C_nnnnn_BuildState" - Building, including nudge
		- "BP_BuildGun_C_nnnnn_DismantleState" - Dismantling
	- Get Menu State Section =
		- MSS_BUILD - Build Menu & building
		- MSS_NONE - Dismantle and other states
		- MSS_CUSTOMIZE - Customisation Menu & painting
		- MSS_BLUEPRINT - Blueprints Menu
		- ...also some In Menu values
	- Get Current Build Gun Mode = 
		- "BuildGun\_\[Mode\]\_S"? Split on _ and get second parameter, and you get Zoop/Diagonal/Noodle/Blueprint/etc modes...