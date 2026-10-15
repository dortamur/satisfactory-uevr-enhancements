-- Profile version to match against UEVR Enhancements mod expected version
local uevr_profile_version = 'v1.2.3-1'

local log_functions = uevr.params.functions

local function vr_log(text)
    log_functions.log_info("[UEVREnhancements] " .. text )
end

vr_log("Initializing UEVREnhancements.lua "..uevr_profile_version)

UEVR_UObjectHook.activate()

local vr = uevr.params.vr
local api = uevr.api;
local uobjects = uevr.types.FUObjectArray.get()
local last_gamepad = {}
local vrcoordinator
local uevr_bridge
local last_world
local last_level
local aim_mode = 0 -- 2
local ui_interact_mode = nil -- false
local movement_mode = 0 -- -1
local roomscale_mode = false
local tick_countdown = 0
local aim_methods = {'Game', 'HMD', 'Right Hand', 'Left Hand'}
local button_labels = {'A', 'X', 'B', 'Y', 'LeftStick', 'RightStick', 'LeftGrip', 'RightGrip', 'Start', 'Back'}
local active_controller = nil

-- Input button ids. MUST match EVRInputButton in UEVREnhancements_Enumerators.h (physical button names).
local INPUT_BUTTON = {
  LeftX = 0, LeftY = 1, RightA = 2, RightB = 3,
  LeftClick = 4, LeftGrip = 5, LeftTrigger = 6,
  RightClick = 7, RightGrip = 8, RightTrigger = 9,
  Start = 10, Back = 11, LeftShoulder = 12, RightShoulder = 13,
  DPadUp = 14, DPadDown = 15, DPadLeft = 16, DPadRight = 17
}
-- Input stick ids. MUST match EVRInputStick in UEVREnhancements_Enumerators.h
local INPUT_STICK = { Left = 0, Right = 1 }

-- Controller types. MUST match EVRControllerType in UEVREnhancements_Enumerators.h
local CONTROLLER_TYPE = { None = 0, Touch = 1, Frame = 2 }

-- Analog trigger values above this count as pressed (XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
local TRIGGER_THRESHOLD = 30

-- Last values sent to the bridge, keyed by input button id / input stick id
local last_input_buttons = {}
local last_input_sticks = {}

-- Set true once we know the bridge supports UpdateInputButton/UpdateInputStick (newer mod); otherwise use legacy Update*State calls
local bridge_new_input_api = false

-- Default to "touch" controller type
local controller_type = CONTROLLER_TYPE.Touch
local controller_is_frame = false

-- Capacitive touch actions (VR.hpp s_action_*_touch_*)
local CONTROLLER_ACTIONS = {
  -- Touch actions
  "/actions/default/in/AButtonTouchLeft",
  "/actions/default/in/BButtonTouchLeft",
  "/actions/default/in/ThumbrestTouchLeft",
  "/actions/default/in/AButtonTouchRight",
  "/actions/default/in/BButtonTouchRight",
  "/actions/default/in/ThumbrestTouchRight",
  "/actions/default/in/TriggerTouch",
  "/actions/default/in/BumperTouch",
  "/actions/default/in/GripTouch",
  "/actions/default/in/JoystickTouch",
  "/actions/default/in/StartButtonTouch",
  "/actions/default/in/BackButtonTouch",
  -- "/actions/default/in/DPad_UpTouch",
  -- "/actions/default/in/DPad_DownTouch",
  -- "/actions/default/in/DPad_LeftTouch",
  -- "/actions/default/in/DPad_RightTouch",
  -- Button press actions (for Steam Frame)
  "/actions/default/in/Grip",
  "/actions/default/in/Squeeze"
}

-- Touch and button action tracking
local action_handles = {}
local action_state = {}

local function aim_method_label(aim_mode)
  return tostring(aim_methods[(aim_mode or 0)+1])
end

local function update_aim_mode()
  -- Aim mode changed!! Update UEVR Input Aim mode
  aim_mode = uevr_bridge.AimMode
  vr.set_aim_method(aim_mode)
  vr_log('Aim Mode changed: '..tostring(aim_mode)..' => '..aim_method_label(aim_mode))
end

local function update_roomscale_mode()
  -- Roomscale mode changed!!
  roomscale_mode = uevr_bridge.RoomscaleMode
  vr_log('Roomscale Mode changed: '..tostring(roomscale_mode))

  vr.set_mod_value("VR_RoomscaleMovement",tostring(roomscale_mode))
end

local function update_movement_mode()
  -- Movement mode changed!!
  movement_mode = uevr_bridge.MovementMode
  vr_log('Movement Mode changed: '..tostring(movement_mode))

  -- uevr.params.vr:set_mod_value("VR_AimMethod","1")
  vr.set_mod_value("VR_MovementOrientation",movement_mode)
  -- uevr.params.vr.set_movement_orientation()
end

local function update_haptics_left()
  -- local vr = uevr.params.vr
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
  -- local vr = uevr.params.vr
  -- vr_log('Haptics Right Update!')
  local source_handle = vr.get_right_joystick_source()
  -- uevr_bridge.HapticsRightPending = false
  local duration = uevr_bridge.haptics_right_duration
  local frequency = uevr_bridge.haptics_right_frequency
  local amplitude = uevr_bridge.haptics_right_amplitude
  vr_log('Haptics Right: '..tostring(duration)..', '..tostring(frequency)..', '..tostring(amplitude))
  vr.trigger_haptic_vibration(0.0, duration, frequency, amplitude, source_handle)
end

local function recenter_view()
  -- local vr = uevr.params.vr
  local hmd_position = UEVR_Vector3f.new()
  local hmd_rotation = UEVR_Quaternionf.new()
  vr.get_pose(vr.get_hmd_index(), hmd_position, hmd_rotation)
  vr.set_standing_origin(hmd_position)
  vr.recenter_view()
  vr_log('Recenter View and Reset Origin done')
end

local function check_controller_type()
  if (vr.get_controller_type ~= nil) then
    local source = vr.get_right_joystick_source()
    local uevr_controller_type = vr.get_controller_type(source)
    controller_type = uevr_controller_type == "frame" and CONTROLLER_TYPE.Frame or CONTROLLER_TYPE.Touch
    controller_is_frame = (controller_type == CONTROLLER_TYPE.Frame)
    uevr_bridge:SetControllerType(controller_type)
    vr_log("Controller type: "..format("%d (%s)", controller_type, uevr_controller_type))
  else
    vr_log("Controller type: unknown (get_controller_type not available)")
  end
end

local function init_controllers()
  check_controller_type()

  -- Init touch and button action tracking
  for _, path in ipairs(CONTROLLER_ACTIONS) do
    local handle = vr.get_action_handle(path)
    if handle == 0 then
        vr_log("Skipping controller action: " .. path)
    else
        -- Add supported action handle
        action_handles[path] = handle
        vr_log("Supported controller action: " .. path)
    end
  end
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

  if uevr_bridge == nil then
    vr_log("UEVRBridge not found")
  else
    vr_log("UEVRBridge found!")
    bridge_new_input_api = (uevr_bridge.UpdateInputButton ~= nil and uevr_bridge.UpdateInputStick ~= nil)
    vr_log("UEVRBridge input API: "..(bridge_new_input_api and "UpdateInputButton/UpdateInputStick" or "legacy Update*State"))
    local pv = uevr.params.version
    local fn = uevr.params.functions
    vr_log("UEVR API: "..tostring(pv)..' / '..tostring(pv.major)..'.'..tostring(pv.minor)..'.'..tostring(pv.patch))
    vr_log("UEVR Build: "..tostring(fn.get_build_date())..' / '..tostring(fn.get_commit_hash())..' / '..tostring(fn.get_branch()))
    vr_log("UEVRBridge: "..uevr_bridge:get_fname():to_string()..' / '..uevr_bridge:get_full_name())
    -- uevr_bridge:DebugLog('Test')
    uevr_bridge:InitUEVRBridge(uevr_profile_version, tostring(pv.major)..'.'..tostring(pv.minor)..'.'..tostring(pv.patch), tostring(fn.get_build_date()), tostring(fn.get_commit_hash()), tostring(fn.get_branch()))
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

    uevr_bridge.SetAimMode:hook_ptr(nil, function(fn, obj, locals, result)
        vr_log('SetAimMode called: '..tostring(uevr_bridge.AimMode)..' => '..aim_method_label(uevr_bridge.AimMode))
        if (uevr_bridge.AimMode ~= aim_mode) then
          -- Interaction/Vehicle mode changed! Update UEVR Input Aim mode
          update_aim_mode()
        end
      end
    )

    uevr_bridge.SetMovementMode:hook_ptr(nil, function(fn, obj, locals, result)
        -- vr_log('SetMovementMode called: '..tostring(aim_mode)..' '..tostring(uevr_bridge.AimMode))
        if (uevr_bridge.MovementMode ~= movement_mode) then
          -- Movement mode changed!
          update_movement_mode()
        end
      end
    )

    uevr_bridge.SetRoomscaleMode:hook_ptr(nil, function(fn, obj, locals, result)
        -- vr_log('SetRoomscaleMode called: '..tostring(aim_mode)..' '..tostring(uevr_bridge.AimMode))
        if (uevr_bridge.RoomscaleMode ~= roomscale_mode) then
          -- Roomscale mode changed!
          update_roomscale_mode()
        end
      end
    )

    uevr_bridge.RecenterView:hook_ptr(nil, function(fn, obj, locals, result)
        -- vr_log('RecenterView called')
        recenter_view()
      end
    )

    -- Check if SetUObjectHookDisabled can be hooked
    if (uevr_bridge.SetUObjectHookDisabled ~= nil) then
      uevr_bridge.SetUObjectHookDisabled:hook_ptr(nil, function(fn, obj, locals, result)
          vr_log('SetUObjectHookDisabled called: '..tostring(uevr_bridge.UObjectHookDisabled))
          -- Disable UObject Hook
          if (uevr_bridge.UObjectHookDisabled) then
            vr_log('Disabling UObject Hook')
            UEVR_UObjectHook.set_disabled(true)
          else
            -- vr_log('Enabling UObject Hook')
            vr_log('Reactivating UObject Hook')
            UEVR_UObjectHook.activate()
            UEVR_UObjectHook.set_disabled(false)
          end

        end
      )
    end

    -- Check if SetUEVRModValue can be hooked
    if (uevr_bridge.SetUEVRModValue ~= nil) then
      uevr_bridge.SetUEVRModValue:hook_ptr(nil, function(fn, obj, locals, result)
          vr_log("SetUEVRModValue: "..tostring(uevr_bridge.UEVRModPropName).."="..tostring(uevr_bridge.UEVRModPropValue))

          -- Set the UEVR property based on mod settings
          vr.set_mod_value(tostring(uevr_bridge.UEVRModPropName), tostring(uevr_bridge.UEVRModPropValue))
        end
      )
    end

    uevr_bridge.UpdateVRPlayerState:hook_ptr(nil, function(fn, obj, locals, result)
        if (uevr_bridge.AimMode ~= aim_mode) then
          -- Interaction/Vehicle mode changed! Update UEVR Input Aim mode
          update_aim_mode()
        end

        if (uevr_bridge.MovementMode ~= movement_mode) then
          -- Movement mode changed!
          update_movement_mode()
        end

        if (uevr_bridge.RoomscaleMode ~= roomscale_mode) then
          -- Roomscale mode changed!
          update_roomscale_mode()
        end

        if (uevr_bridge.UIInteractMode ~= ui_interact_mode) then
          --  UI Interaction mode changed - log for streaming stabilisation switch
          ui_interact_mode = uevr_bridge.UIInteractMode
          vr_log('UI Interact Change [UIInteract='..tostring(ui_interact_mode)..']')
        end

      end
    )

    -- Update Aim mode to initial state
    update_aim_mode()

    -- Update movement mode to initial state
    update_movement_mode()

    -- update roomscale mode to initial state
    update_roomscale_mode()

    init_controllers()
  end
end

-- Send a single input button state change to the bridge (only if changed).
local function update_input_button(id, pressed)
  pressed = pressed and true or false
  if last_input_buttons[id] == pressed then return end
  vr_log("UpdateInputButton: "..tostring(id).." => "..tostring(pressed))
  last_input_buttons[id] = pressed
  uevr_bridge:UpdateInputButton(id, pressed)
end

-- Send all button states if updated. New bridges get one UpdateInputButton call per changed button;
-- older bridges get the legacy all-in-one UpdateButtonState call.
local function update_input_buttons(s)
  if bridge_new_input_api then
    update_input_button(INPUT_BUTTON.RightA, s.a)
    update_input_button(INPUT_BUTTON.RightB, s.b)
    update_input_button(INPUT_BUTTON.LeftX, s.x)
    update_input_button(INPUT_BUTTON.LeftY, s.y)
    update_input_button(INPUT_BUTTON.LeftClick, s.left_stick)
    update_input_button(INPUT_BUTTON.RightClick, s.right_stick)
    update_input_button(INPUT_BUTTON.LeftGrip, s.left_grip)
    update_input_button(INPUT_BUTTON.RightGrip, s.right_grip)
    update_input_button(INPUT_BUTTON.LeftShoulder, s.left_shoulder)
    update_input_button(INPUT_BUTTON.RightShoulder, s.right_shoulder)
    update_input_button(INPUT_BUTTON.Start, s.start)
    update_input_button(INPUT_BUTTON.Back, s.back)
    update_input_button(INPUT_BUTTON.DPadUp, s.dpad_up)
    update_input_button(INPUT_BUTTON.DPadDown, s.dpad_down)
    update_input_button(INPUT_BUTTON.DPadLeft, s.dpad_left)
    update_input_button(INPUT_BUTTON.DPadRight, s.dpad_right)
  else
    uevr_bridge:UpdateButtonState(s.a, s.b, s.x, s.y, s.left_stick, s.left_grip, s.right_stick, s.right_grip, s.start)
  end
end

local function update_input_stick(id, x, y)
  if bridge_new_input_api then
    uevr_bridge:UpdateInputStick(id, x, y)
  elseif id == INPUT_STICK.Left then
    uevr_bridge:UpdateLeftStickState(x, y)
  else
    uevr_bridge:UpdateRightStickState(x, y)
  end
end

local function update_input_button_trigger(id, value)
  local pressed = value > TRIGGER_THRESHOLD
  if bridge_new_input_api then
    update_input_button(id, pressed)
  elseif id == INPUT_BUTTON.LeftTrigger then
    uevr_bridge:UpdateLeftTriggerState(pressed)
  else
    uevr_bridge:UpdateRightTriggerState(pressed)
  end
end

uevr.sdk.callbacks.on_xinput_get_state(function(retval, user_index, state)
  -- if true then return end -- Breakpoint

  if uevr_bridge == nil then
    return
  end

-- vr_log(string.format("[XInput] idx=%d wButtons=0x%04X", user_index, state.Gamepad.wButtons))

  local gamepad = state.Gamepad

  if active_controller == nil then
    -- Lock onto the first slot that ever reports input (just check buttons+triggers); ignore the rest.
    if gamepad.wButtons ~= 0 or gamepad.bLeftTrigger ~= 0 or gamepad.bRightTrigger ~= 0 then
      active_controller = user_index
      vr_log("Locked controller to index " .. user_index)

      -- Re-test Controller Type (since it's typically unknown on first test)
      check_controller_type()
    end
  end

  if active_controller ~= nil and user_index ~= active_controller then
      return -- ignore controller
  end

  -- Check touch and button action status (not covered by XInput)
  local controllers = {
      L = vr.get_left_joystick_source(),
      R = vr.get_right_joystick_source(),
  }

  local changed_buttons = false
  local changed_touch = false
  -- local changed_actions = {}
  for action_path, handle in pairs(action_handles) do
    for hand, src in pairs(controllers) do
      local key = hand..':'..action_path:match("([^/]+)$")
      -- vr_log(string.format("Touch Test: %s %-22s %s", hand, action_path:match("([^/]+)$"), key, tostring(src)))
      local now = vr.is_action_active(handle, src)
      if now ~= action_state[key] then
        action_state[key] = now
        -- Squeeze debugging
        -- if (string.find(key, "Squeeze")) then
        --   local src = (hand == "L") and vr.get_left_joystick_source() or vr.get_right_joystick_source()
        --   local squeeze_axis = vr.get_action_axis(squeeze, src)
        --   vr_log(string.format("Squeeze Action: %s %-22s %f %s", hand, action_path:match("([^/]+)$"), squeeze_axis.value, now and "start" or "stop"))
        -- end
        -- changed_actions.push(key)
        if (string.find(key, "Touch")) then
          changed_touch = true
        else
          changed_buttons = true
        end
        -- table.insert(changed_actions, key)
        vr_log(string.format("Action: %s %-22s %s", hand, action_path:match("([^/]+)$"), now and "start" or "stop"))
      end
    end
  end

  -- vr_log('Comparing '..gamepad.wButtons..' to '..last_gamepad.wButtons)
  -- Compare gamepad states to last_gamepad
  if gamepad.wButtons ~= last_gamepad.wButtons or changed_buttons then
    local btn_a = gamepad.wButtons & XINPUT_GAMEPAD_A ~= 0
    local btn_x = gamepad.wButtons & (controller_is_frame and XINPUT_GAMEPAD_X or XINPUT_GAMEPAD_B) ~= 0
    local btn_b = gamepad.wButtons & (controller_is_frame and XINPUT_GAMEPAD_B or XINPUT_GAMEPAD_X) ~= 0
    local btn_y = gamepad.wButtons & XINPUT_GAMEPAD_Y ~= 0
    local btn_left_stick = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB ~= 0
    local btn_right_stick = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB ~= 0
    local btn_left_grip = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER ~= 0
    local btn_right_grip = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER ~= 0
    local btn_start = gamepad.wButtons & XINPUT_GAMEPAD_START ~= 0
    local btn_back = gamepad.wButtons & XINPUT_GAMEPAD_BACK ~= 0
    local btn_left_shoulder = false
    local btn_right_shoulder = false

    -- If using Frame controllers, use Squeeze "action" instead of shoulder input, and reassign xinput to shoulders
    if controller_is_frame then
      btn_left_shoulder = btn_left_grip
      btn_right_shoulder = btn_right_grip
      btn_left_grip = action_state["L:Squeeze"]
      btn_right_grip = action_state["R:Squeeze"]
    end

    -- vr_log('Buttons A='..tostring(btn_a)..' B='..tostring(btn_b)..' X='..tostring(btn_x)..' Y='..tostring(btn_y)..' LS='..tostring(btn_left_stick)..' LG='..tostring(btn_left_grip)..' RS='..tostring(btn_right_stick)..' RG='..tostring(btn_right_grip))
    if (gamepad.wButtons ~= 0 or btn_left_grip or btn_right_grip) then
      local button_states = { btn_a, btn_x, btn_b, btn_y, btn_left_stick, btn_right_stick, btn_left_grip, btn_right_grip, btn_start, btn_back }
      -- Assemble string list of active buttons
      local active_buttons = {}
      for i, state in ipairs(button_states) do
          if state then
              table.insert(active_buttons, button_labels[i])
          end
      end
      vr_log(string.format('Buttons wButtons=0x%04X: %s', gamepad.wButtons, table.concat(active_buttons, ', ')))
    else
      vr_log('Buttons cleared')
    end
    last_gamepad.wButtons = gamepad.wButtons
    update_input_buttons({
      a = btn_a, b = btn_b, x = btn_x, y = btn_y,
      left_stick = btn_left_stick, right_stick = btn_right_stick,
      left_grip = btn_left_grip, right_grip = btn_right_grip,
      left_shoulder = btn_left_shoulder, right_shoulder = btn_right_shoulder,
      start = btn_start, back = btn_back,
      dpad_up = controller_is_frame and gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP ~= 0,
      dpad_down = controller_is_frame and gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ~= 0,
      dpad_left = controller_is_frame and gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ~= 0,
      dpad_right = controller_is_frame and gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ~= 0
    })
  end
  if (gamepad.sThumbLX or 0) ~= last_gamepad.sThumbLX or (gamepad.sThumbLY or 0) ~= last_gamepad.sThumbLY then
    local stick_left_x = gamepad.sThumbLX or 0
    local stick_left_y = gamepad.sThumbLY or 0
    -- vr_log('Left Stick X='..tostring(stick_left_x)..' Y='..tostring(stick_left_y)) -- ..' UI='..tostring(user_index)..' RV='..tostring(retval)..' PN='..tostring(state.dwPacketNumber))
    last_gamepad.sThumbLX = gamepad.sThumbLX or 0
    last_gamepad.sThumbLY = gamepad.sThumbLY or 0
    update_input_stick(INPUT_STICK.Left, gamepad.sThumbLX, gamepad.sThumbLY)
  end
  if (gamepad.sThumbRX or 0) ~= last_gamepad.sThumbRX or (gamepad.sThumbRY or 0) ~= last_gamepad.sThumbRY then
    local stick_right_x = gamepad.sThumbRX or 0
    local stick_right_y = gamepad.sThumbRY or 0
    -- vr_log('Right Stick X='..tostring(stick_right_x)..' Y='..tostring(stick_right_y))
    last_gamepad.sThumbRX = gamepad.sThumbRX or 0
    last_gamepad.sThumbRY = gamepad.sThumbRY or 0
    update_input_stick(INPUT_STICK.Right, gamepad.sThumbRX, gamepad.sThumbRY)
  end

  if gamepad.bLeftTrigger ~= last_gamepad.bLeftTrigger then
    local left_trigger = gamepad.bLeftTrigger
    -- vr_log('Left Trigger LT='..tostring(left_trigger))
    last_gamepad.bLeftTrigger = gamepad.bLeftTrigger
    update_input_button_trigger(INPUT_BUTTON.LeftTrigger, gamepad.bLeftTrigger)
  end
  if gamepad.bRightTrigger ~= last_gamepad.bRightTrigger then
    local right_trigger = gamepad.bRightTrigger
    -- vr_log('Right Trigger RT='..tostring(right_trigger))
    last_gamepad.bRightTrigger = gamepad.bRightTrigger
    update_input_button_trigger(INPUT_BUTTON.RightTrigger, gamepad.bRightTrigger)
  end

  -- vr_log('XState: '..tostring(user_index)..' / '..tostring(retval)..' / '..tostring(gamepad.wButtons)..' / '..tostring(left_trigger)..' / '..tostring(stick_left_x)..' / '..tostring(stick_left_y)..' / '..tostring(right_trigger)..' / '..tostring(stick_right_x)..' / '..tostring(stick_right_y))
end)

uevr.sdk.callbacks.on_post_engine_tick(function(engine, delta)

end)

uevr.sdk.callbacks.on_pre_engine_tick(function(engine, delta)
  if not vr.is_runtime_ready() then return end
  -- if true then return end -- Breakpoint

  -- Check every *n* ticks...
  tick_countdown = tick_countdown - 1
  if (tick_countdown <= 0) then
    tick_countdown = 500

    if (uevr_bridge == nil) then
      init_bridge()
      if (uevr_bridge == nil) then
        vr_log('UEVR Enhancement Mod not found! Did you install the mod?')
        return
      end
    end

    -- vr_log('States: AM='..tostring(uevr_bridge.AimMode)..' UII='..tostring(uevr_bridge.UIInteractMode)..' MM='..tostring(uevr_bridge.MovementMode)..' RS='..tostring(uevr_bridge.RoomscaleMode))
  end

  -- Detect Touch events
  -- local L, R = vr.get_left_joystick_source(), vr.get_right_joystick_source()
  -- local h = {}
  -- local function act(p) h[p] = h[p] or vr.get_action_handle(p); return h[p] end
  -- local function down(p, src) return vr.is_action_active(act(p), src) end

  -- -- e.g. right thumbrest capacitive touch
  -- if down("/actions/default/in/ThumbrestTouchRight", R) then
  --   vr_log("Right Thumbrest Touched")
  -- end

end)
