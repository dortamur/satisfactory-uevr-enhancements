-- Profile version to match against UEVR Enhancements mod expected version
local uevr_profile_version = 'v0.9.10-1'

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
local aim_mode = 0 -- 2
local ui_interact_mode = nil -- false
local movement_mode = 0 -- -1
local roomscale_mode = false
local tick_countdown = 0
local aim_methods = {'Game', 'HMD', 'Right Hand', 'Left Hand'}

local function update_aim_mode()
  -- Aim mode changed!! Update UEVR Input Aim mode
  aim_mode = uevr_bridge.AimMode
  uevr.params.vr.set_aim_method(aim_mode)
  vr_log('Aim Method changed: '..tostring(aim_mode)..' => '..tostring(aim_methods[aim_mode or 0]))

end

local function update_roomscale_mode()
  -- Roomscale mode changed!!
  roomscale_mode = uevr_bridge.RoomscaleMode
  vr_log('Roomscale Mode changed: '..tostring(roomscale_mode))

  uevr.params.vr.set_mod_value("VR_RoomscaleMovement",tostring(roomscale_mode))
end

local function update_movement_mode()
  -- Movement mode changed!!
  movement_mode = uevr_bridge.MovementMode
  vr_log('Movement Mode changed: '..tostring(movement_mode))

  -- uevr.params.vr:set_mod_value("VR_AimMethod","1")
  uevr.params.vr.set_mod_value("VR_MovementOrientation",movement_mode)
  -- uevr.params.vr.set_movement_orientation()
end

local function update_haptics_left()
  local vr = uevr.params.vr
  -- vr_log('Haptics Left Update!')
  local source_handle = vr.get_left_joystick_source()
  -- uevr_bridge.HapticsLeftPending = false
  local duration = uevr_bridge.haptics_left_duration
  local frequency = uevr_bridge.haptics_left_frequency
  local amplitude = uevr_bridge.haptics_left_amplitude
  vr_log('Haptics Left: '..tostring(duration)..', '..tostring(frequency)..', '..tostring(amplitude))
  vr.trigger_haptic_vibration(0.0, duration, frequency, amplitude, source_handle)
end

local function update_haptics_right()
  local vr = uevr.params.vr
  -- vr_log('Haptics Right Update!')
  local source_handle = vr.get_right_joystick_source()
  -- uevr_bridge.HapticsRightPending = false
  local duration = uevr_bridge.haptics_right_duration
  local frequency = uevr_bridge.haptics_right_frequency
  local amplitude = uevr_bridge.haptics_right_amplitude
  vr_log('Haptics Right: '..tostring(duration)..', '..tostring(frequency)..', '..tostring(amplitude))
  vr.trigger_haptic_vibration(0.0, duration, frequency, amplitude, source_handle)
end

local function init_bridge()
  -- Get UEVR Bridge via GameInstanceModule UEVRBridge
  local game_instance_module_c = api:find_uobject('Class /Script/SML.GameInstanceModule')
  if game_instance_module_c == nil then
      vr_log("Class GameInstanceModule not found")
  else
    vr_log("GameInstanceModule Class: "..game_instance_module_c:get_full_name())
    local game_instance_modules = UEVR_UObjectHook.get_objects_by_class(game_instance_module_c, false)

    vr_log("GameInstanceModules: ")
    for i, module in ipairs(game_instance_modules) do
      vr_log(" - "..module:get_fname():to_string()..' / '..module:get_full_name())
      if (module:get_fname():to_string() == 'UEVREnhancements_UEVRBridge') then
        vr_log('Found UEVRBridge module as GameInstanceModule!')
        uevr_bridge = module
      end
    end
  end

  -- Legacy Support for Get UEVR Bridge via ModSubsystem VRCoordinator
  if uevr_bridge == nil then
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
      vr_log('Found legacy UEVRBridge module via VRCoordinator Subsystem!')
      uevr_bridge = vrcoordinator.UEVRBridge
    end
  end

  if uevr_bridge == nil then
    vr_log("UEVRBridge not found")
  else
    vr_log("UEVRBridge found!")
    local pv = uevr.params.version
    vr_log("PV: "..tostring(pv)..' / '..tostring(pv.major)..'.'..tostring(pv.minor)..'.'..tostring(pv.patch))
    vr_log("UEVRBridge: "..uevr_bridge:get_fname():to_string()..' / '..uevr_bridge:get_full_name())
    uevr_bridge:DebugLog('Test')
    uevr_bridge:InitUEVRBridge(uevr_profile_version, tostring(pv.major)..'.'..tostring(pv.minor)..'.'..tostring(pv.patch))
    -- uevr_bridge.IsInitialised = true

    uevr_bridge.SetHapticsRightEffect:hook_ptr(nil, function(fn, obj, locals, result)
        update_haptics_right()
        vr_log('SetHapticsRightEffect: '..obj:get_fname():to_string() .. " called")
      end
    )

    uevr_bridge.SetHapticsLeftEffect:hook_ptr(nil, function(fn, obj, locals, result)
        update_haptics_left()
        vr_log('SetHapticsLeftEffect: '..obj:get_fname():to_string() .. " called")
      end
    )

    uevr_bridge.UpdateVRPlayerState:hook_ptr(nil, function(fn, obj, locals, result)
        if (uevr_bridge.AimMode ~= aim_mode) then
          -- Interaction/Vehicle mode changed!! Update UEVR Input Aim mode
          update_aim_mode()
        end

        if (uevr_bridge.MovementMode ~= movement_mode) then
          -- Interaction mode changed!! Update UEVR Input Aim mode
          update_movement_mode()
        end

        if (uevr_bridge.RoomscaleMode ~= roomscale_mode) then
          -- Interaction mode changed!! Update UEVR Input Aim mode
          update_roomscale_mode()
        end
      end
    )

    -- Update Aim mode to initial state
    update_aim_mode()

    -- Update movement mode to initial state
    update_movement_mode()

    -- update roomscale mode to initial state
    update_roomscale_mode()
  end
end

local function check_state()
  -- local game_engine_class = api:find_uobject("Class /Script/Engine.GameEngine")
  -- local game_engine = UEVR_UObjectHook.get_first_object_by_class(game_engine_class)

  -- local viewport = game_engine.GameViewport
  -- if viewport == nil then
  --     vr_log("Viewport is nil")
  --     return
  -- end
  -- local world = viewport.World

  -- if world == nil then
  --     vr_log("World is nil")
  --     -- Reset in-game mod connections
  --     if (vrcoordinator ~= nil) then
  --       vr_log("Clearing previous UEVR mod references")
  --       vrcoordinator = nil
  --       uevr_bridge = nil
  --     end
  --     return
  -- end

  -- if world ~= last_world then
  --   vr_log("World changed")
  --   last_world = world
  --   -- Reset in-game mod connections
  --   vrcoordinator = nil
  --   uevr_bridge = nil
  --   return
  -- end

  -- if (uevr_bridge == nil) then
  --   init_bridge()
  -- end
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

  if vrcoordinator ~= null then
    local new_state = vrcoordinator.VRPlayerState.CurrentState
    if (new_state ~= player_state) then
      player_state = new_state
      vr_log('Player State: '..tostring(player_state))
      if (player_state == 7 or player_state == 8) then
        -- Recenter View on entering a vehicle
        uevr.params.vr.recenter_view()
      end
    end
  end
end)

uevr.sdk.callbacks.on_post_engine_tick(function(engine, delta)

end)

uevr.sdk.callbacks.on_pre_engine_tick(function(engine, delta)
  -- check_state()
  -- if true then return end -- Breakpoint
  if (uevr_bridge == nil) then
    init_bridge()
    if (uevr_bridge == nil) then
      return
    end
  end

  tick_countdown = tick_countdown - 1
  if (tick_countdown <= 0) then
    -- vr_log('States: AM='..tostring(uevr_bridge.AimMode)..' UII='..tostring(uevr_bridge.UIInteractMode)..' MM='..tostring(uevr_bridge.MovementMode)..' RS='..tostring(uevr_bridge.RoomscaleMode))
    tick_countdown = 200
  end

end)
