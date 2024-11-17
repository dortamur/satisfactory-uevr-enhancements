-- Profile version to match against UEVR Enhancements mod expected version
local uevr_profile_version = 'v0.9.6-1'

local log_functions = uevr.params.functions

local function vr_log(text)
    log_functions.log_info("[UEVREnhancements] " .. text )
end

vr_log("Initializing UEVREnhancements.lua "..uevr_profile_version)

UEVR_UObjectHook.activate()

local api = uevr.api;
local uobjects = uevr.types.FUObjectArray.get()
local last_gamepad = {}
local vrcoordinator
local uevr_bridge
local last_world
local last_level
local player_state
local game_aim_mode = false
local movement_mode = -1

local function update_aim_mode()
  -- Interaction mode changed!! Update UEVR Input Aim mode
  game_aim_mode = uevr_bridge.GameAimMode
  vr_log('Aim Method changed: '..tostring(game_aim_mode))

  if (game_aim_mode == true) then
    -- uevr.params.vr:set_mod_value("VR_AimMethod","1")
    uevr.params.vr.set_aim_method(0)
    vr_log("Aim Method: Game")
  else
    uevr.params.vr.set_aim_method(2)
    vr_log("Aim Method: Right Hand")
  end
end

local function update_movement_mode()
  -- Interaction mode changed!! Update UEVR Input Aim mode
  movement_mode = uevr_bridge.MovementMode
  vr_log('Movement Mode changed: '..tostring(movement_mode))

  -- uevr.params.vr:set_mod_value("VR_AimMethod","1")
  uevr.params.vr.set_mod_value("VR_MovementOrientation",movement_mode)
  -- uevr.params.vr.set_movement_orientation()
end

local function init_bridge()
  local mod_subsystem_c = api:find_uobject("Class /Script/SML.ModSubsystem")
  if mod_subsystem_c == nil then
      vr_log("Class ModSubsystem not found")
  else
    vr_log("Subsystem Class: "..mod_subsystem_c:get_full_name())
    local subsystems = UEVR_UObjectHook.get_objects_by_class(mod_subsystem_c, false)

    vr_log("Subsystems: ")
    for i, subsystem in ipairs(subsystems) do
      vr_log(" - "..subsystem:get_fname():to_string()..' / '..subsystem:get_full_name())
      if (subsystem:get_fname():to_string() == 'UEVREnhancements_VRCoordinatorSystem_C') then
        vrcoordinator = subsystem
      end
    end
  end

  if vrcoordinator ~= nil then
    -- if true then return end -- Breakpoint
    uevr_bridge = vrcoordinator.UEVRBridge
    if uevr_bridge == nil then
      vr_log("UEVRBridge not found")
    else
      vr_log("UEVRBridge found!")
      local pv = uevr.params.version
      vr_log("PV: "..tostring(pv)..' / '..tostring(pv.major)..'.'..tostring(pv.minor)..'.'..tostring(pv.patch))
      uevr_bridge:InitUEVRBridge(uevr_profile_version, tostring(pv.major)..'.'..tostring(pv.minor)..'.'..tostring(pv.patch))
      -- uevr_bridge.IsInitialised = true
      vr_log("UEVRBridge: "..uevr_bridge:get_fname():to_string()..' / '..uevr_bridge:get_full_name())

      -- Update Aim mode to initial state
      update_aim_mode()

      -- Update movement mode to initial state
      update_movement_mode()

      if false then
      -- vr_print("UEVRBridge Event Dispatch: "..uevr_bridge:as_class(UEVR_BlueprintGeneratedClass))
      -- Init UI Interaction hook
      -- local uevr_bridge_bpc = api:find_uobject("BlueprintGeneratedClass /UEVREnhancements/Blueprints/UEVRBridge.UEVRBridge_C")


      -- ui_interact_fn = uevr_bridge_bpc:find_function("UpdateUIInteraction2")
      -- ui_interact_fn = uevr_bridge_bpc:find_function("DebugLog")
      ui_interact_fn = uevr_bridge.UpdateUIInteraction

      if ui_interact_fn ~= nil then
          vr_print("Found UIInteraction function")

          ui_interact_fn:hook_ptr(function(fn, obj, locals, result)
              vr_print('Foo')
              vr_print("UIInteraction! " .. tostring(locals).." ------------------------------------------------------")
          end, nil)
      else
          vr_print("Failed to find UIInteraction function")
      end
      end
    end
  end
end

local function check_state()
  local game_engine_class = api:find_uobject("Class /Script/Engine.GameEngine")
  local game_engine = UEVR_UObjectHook.get_first_object_by_class(game_engine_class)

  local viewport = game_engine.GameViewport
  if viewport == nil then
      vr_log("Viewport is nil")
      return
  end
  local world = viewport.World

  if world == nil then
      vr_log("World is nil")
      -- Reset in-game mod connections
      if (vrcoordinator ~= nil) then
        vr_log("Clearing previous UEVR mod references")
        vrcoordinator = nil
        uevr_bridge = nil
      end
      return
  end

  if world ~= last_world then
    vr_log("World changed")
    last_world = world
    -- Reset in-game mod connections
    vrcoordinator = nil
    uevr_bridge = nil
    return
  end

  if (vrcoordinator == nil) then
    init_bridge()
  end
end

uevr.sdk.callbacks.on_xinput_get_state(function(retval, user_index, state)
  -- if true then return end -- Breakpoint

  if uevr_bridge == nil then
    return
  end

  local gamepad = state.Gamepad

  -- vr_log('Comparing '..gamepad.wButtons..' to '..last_gamepad.wButtons)
  -- Compare gamepad states to last_gamepad
  if gamepad.wButtons ~= last_gamepad.wButtons then
    local btn_a = gamepad.wButtons & XINPUT_GAMEPAD_A ~= 0
    local btn_x = gamepad.wButtons & XINPUT_GAMEPAD_B ~= 0
    local btn_b = gamepad.wButtons & XINPUT_GAMEPAD_X ~= 0
    local btn_y = gamepad.wButtons & XINPUT_GAMEPAD_Y ~= 0
    local btn_left_stick = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ~= 0
    local btn_right_stick = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ~= 0
    local btn_left_grip = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ~= 0
    local btn_right_grip = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ~= 0
    local btn_start = gamepad.wButtons & XINPUT_GAMEPAD_START ~= 0
    local btn_back = gamepad.wButtons & XINPUT_GAMEPAD_BACK ~= 0
    -- vr_log('Buttons A='..tostring(btn_a)..' B='..tostring(btn_b)..' X='..tostring(btn_x)..' Y='..tostring(btn_y)..' LS='..tostring(btn_left_stick)..' LG='..tostring(btn_left_grip)..' RS='..tostring(btn_right_stick)..' RG='..tostring(btn_right_grip))
    vr_log('Buttons wButtons='..tostring(gamepad.wButtons)..' Start='..tostring(btn_start)..' Back='..tostring(btn_back))
    last_gamepad.wButtons = gamepad.wButtons
    uevr_bridge:UpdateButtonState(btn_a, btn_b, btn_x, btn_y, btn_left_stick, btn_left_grip, btn_right_stick, btn_right_grip, btn_start)
  end
  if (gamepad.sThumbLX or 0) ~= last_gamepad.sThumbLX or (gamepad.sThumbLY or 0) ~= last_gamepad.sThumbLY then
    local stick_left_x = gamepad.sThumbLX or 0
    local stick_left_y = gamepad.sThumbLY or 0
    -- vr_log('Left Stick X='..tostring(stick_left_x)..' Y='..tostring(stick_left_y)..' UI='..tostring(user_index)..' RV='..tostring(retval)..' PN='..tostring(state.dwPacketNumber))
    last_gamepad.sThumbLX = gamepad.sThumbLX or 0
    last_gamepad.sThumbLY = gamepad.sThumbLY or 0
    uevr_bridge:UpdateLeftStickState(gamepad.sThumbLX, gamepad.sThumbLY)
  end
  if (gamepad.sThumbRX or 0) ~= last_gamepad.sThumbRX or (gamepad.sThumbRY or 0) ~= last_gamepad.sThumbRY then
    local stick_right_x = gamepad.sThumbRX or 0
    local stick_right_y = gamepad.sThumbRY or 0
    -- vr_log('Right Stick X='..tostring(stick_right_x)..' Y='..tostring(stick_right_y))
    last_gamepad.sThumbRX = gamepad.sThumbRX or 0
    last_gamepad.sThumbRY = gamepad.sThumbRY or 0
    uevr_bridge:UpdateRightStickState(gamepad.sThumbRX, gamepad.sThumbRY)
  end

  if gamepad.bLeftTrigger ~= last_gamepad.bLeftTrigger then
    local left_trigger = gamepad.bLeftTrigger
    -- vr_log('Left Trigger LT='..tostring(left_trigger))
    last_gamepad.bLeftTrigger = gamepad.bLeftTrigger
    uevr_bridge:UpdateLeftTriggerState(gamepad.bLeftTrigger ~= 0)
  end
  if gamepad.bRightTrigger ~= last_gamepad.bRightTrigger then
    local right_trigger = gamepad.bRightTrigger
    -- vr_log('Right Trigger RT='..tostring(right_trigger))
    last_gamepad.bRightTrigger = gamepad.bRightTrigger
    uevr_bridge:UpdateRightTriggerState(gamepad.bRightTrigger ~= 0)
  end

  -- vr_log('XState: '..tostring(user_index)..' / '..tostring(retval)..' / '..tostring(gamepad.wButtons)..' / '..tostring(left_trigger)..' / '..tostring(stick_left_x)..' / '..tostring(stick_left_y)..' / '..tostring(right_trigger)..' / '..tostring(stick_right_x)..' / '..tostring(stick_right_y))

  local new_state = vrcoordinator.VRPlayerState.CurrentState
  if (new_state ~= player_state) then
    player_state = new_state
    vr_log('Player State: '..tostring(player_state))
    if (player_state == 7 or player_state == 8) then
      -- Recenter View on entering a vehicle
      uevr.params.vr.recenter_view()
    end
  end
end)

uevr.sdk.callbacks.on_post_engine_tick(function(engine, delta)

end)

uevr.sdk.callbacks.on_pre_engine_tick(function(engine, delta)
  check_state()
  -- if true then return end -- Breakpoint
  if (uevr_bridge == nil) then
    return
  end

  if (uevr_bridge.GameAimMode ~= game_aim_mode) then
    -- Interaction/Vehicle mode changed!! Update UEVR Input Aim mode
    update_aim_mode()
  end

  if (uevr_bridge.MovementMode ~= movement_mode) then
    -- Interaction mode changed!! Update UEVR Input Aim mode
    update_movement_mode()
  end

end)
