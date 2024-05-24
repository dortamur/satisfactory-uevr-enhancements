## Overview

The left controller is focussed on movement, and most movement controls are handled by this controller.

The right controller is focussed on interactions, including weapon aiming & firing, build aiming and adjustments (including nudging), and menu selection.

The controller face buttons are configured as short-cuts for actions also available in the wrist panels.

## Wrist Panel Controls

Each controller has a wrist panel with information and additional actions. Rotate your wrist (like looking at a watch) to see the wrist panels.
Most actions are selectable from these menus without requiring additional face buttons. 

The right wrist has a menu of selectable options, and also shows build information. It changes based on the current build state.
This can be interacted with a number of different ways:
- Point at it with the left controller, and push left trigger to select an option
- Press Up on the right controller stick, roll the stick left or right to highlight an option, and push right trigger to select

The left wrist has a number of tabbed panels with status information, equipment, build quick bar, and scanner.

## Default Satisfactory UI

Due to how the UEVR Injector works, the default UI screens will move about as the right controller moves. 
To interact with these screens, keep the right controller facing forward, and use the left controller as an emulated mouse to interact with the screen.
Note: The current implementation is not a complete mouse replacement, and sometimes not reliable, although most mouse-like actions such as clicking buttons and drag and drop *should* work.

## Default Controls

- Left Controller stick
	- Up/Down: move forward/backward
	- Left/Right: strafe left/right
	- Click while moving: toggle Sprint
- Left Controller Trigger
	- Jump/Up
- Left Controller Grip
	- Crouch
- Left Controller Buttons:
	- X: Holster
	- Y: Flashlight

- Right Controller Trigger
	- Use Equipment
- Right Controller Grip
	- Interact/Cancel
- Right Controller Stick:
	- Left/Right: Smooth Turn left/right
	- Up: Wrist Menu Select
	- Down: Reload/Ammo - proposed (not implemented)
	- Click: Building Sample
- Right Controller Buttons:
	- A: Secondary Fire
	- B: Cycle through equipment

## Building

- Right Controller Trigger
	- Build item
- Right Controller Grip
	- Cancel building
- Right Controller Stick:
	- Down then left/right: Rotate/Adjust build item
- Right Controller Buttons:
	- A: Build Mode
	- B: Switch build parts
- Left Controller Buttons:
	- X: Snap to Grid
	- Y: Hologram Lock

## Nudge Mode

- Right Controller Stick:
	- Left/Right/Up/Down: nudge build part
- Right Controller Grip
	- Unlock Hologram

## Dismantle Mode

- Right Controller Buttons:
	- A: Dismantle Mode (Normal/Blueprint)
	- B: Multi-Select
- Left Controller Buttons:
	- X: Filter

## Paint Mode

- Right Controller Buttons:
	- A: Dismantle Mode (Normal/Blueprint)
	- B: Switch
- Left Controller Buttons:
	- X: Filter

## Right Wrist Menu Stick-Selection

- Right Controller Stick:
	- Up then left/right: Highlight wrist menu selection
- Right Controller Trigger
	- Select highlighted item

## Default Satisfactory UI

- Left Controller stick
	- Emulated mouse position
- Left Controller Trigger
	- Emulated mouse left-click
- Left Controller Grip
	- Emulated mouse right-click
- Left Controller Buttons:
	- X: Scroll down
	- Y: Scroll up
- Right Controller A Button: Alternate "click" that sometimes works better than left trigger 🤷‍♂️
