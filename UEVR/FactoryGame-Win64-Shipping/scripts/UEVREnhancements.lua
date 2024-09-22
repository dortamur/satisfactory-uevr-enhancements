local function vr_print(text)
    print("[UEVREnhancements Lua] " .. text )
end

vr_print("Initializing UEVREnhancements.lua")

UEVR_UObjectHook.activate()

local api = uevr.api;
local uobjects = uevr.types.FUObjectArray.get()
local last_gamepad = {}
local vrcoordinator
local vrcontrolleractions

local function init_bridge()
  local mod_subsystem_c = api:find_uobject("Class /Script/SML.ModSubsystem")
  if mod_subsystem_c == nil then
      vr_print("Class ModSubsystem not found")
  else
    vr_print("Subsystem Class: "..mod_subsystem_c:get_full_name())
    local subsystems = UEVR_UObjectHook.get_objects_by_class(mod_subsystem_c, false)

    vr_print("Subsystems: ")
    for i, subsystem in ipairs(subsystems) do
      vr_print(" - "..subsystem:get_fname():to_string()..' / '..subsystem:get_full_name())
      if (subsystem:get_fname():to_string() == 'UEVREnhancements_VRCoordinatorSystem_C') then
        vrcoordinator = subsystem
      end
    end
  end

  if vrcoordinator ~= nil then
    uevr_bridge = vrcoordinator.UEVRBridge
    -- local actor_component_c = api:find_uobject("Class /Script/Engine.ActorComponent")
    if uevr_bridge == nil then
      vr_print("UEVRBridge not found")
    else
      vr_print("UEVRBridge found!")
      uevr_bridge:InitUEVRBridge()
    end
  end
end


local function debug_test()
  local game_engine_class = api:find_uobject("Class /Script/Engine.GameEngine")
  local game_engine = UEVR_UObjectHook.get_first_object_by_class(game_engine_class)

  local viewport = game_engine.GameViewport
  if viewport == nil then
      vr_print("Viewport is nil")
      return
  end
  local world = viewport.World

  if world == nil then
      vr_print("World is nil")
      return
  end

  if world ~= last_world then
      vr_print("World changed")
  end

  last_world = world

  local level = world.PersistentLevel

  vr_print("GameEngine class: " .. game_engine_class:get_full_name())
  vr_print("GameEngine object: " .. game_engine:get_full_name())

  local mod_subsystem_c = api:find_uobject("Class /Script/SML.ModSubsystem")
  if mod_subsystem_c == nil then
      vr_print("Class ModSubsystem not found")
  else
    vr_print("Subsystem Class: "..mod_subsystem_c:get_full_name())
    local subsystems = UEVR_UObjectHook.get_objects_by_class(mod_subsystem_c, false)

    vr_print("Subsystems: ")
    for i, subsystem in ipairs(subsystems) do
      vr_print(" - "..subsystem:get_fname():to_string()..' / '..subsystem:get_full_name())
      if (subsystem:get_fname():to_string() == 'UEVREnhancements_VRCoordinatorSystem_C') then
        vrcoordinator = subsystem
      end
    end
  end

  if vrcoordinator ~= nil then
    uevr_bridge = vrcoordinator.UEVRBridge
    -- local actor_component_c = api:find_uobject("Class /Script/Engine.ActorComponent")
    if uevr_bridge == nil then
      vr_print("UEVRBridge not found")
    else
      vr_print("UEVRBridge found!")
    end
  end

  last_level = level

end

uevr.sdk.callbacks.on_xinput_get_state(function(retval, user_index, state)
  if uevr_bridge == nil then
    return
  end

  local gamepad = state.Gamepad

  -- vr_print('Comparing '..gamepad.wButtons..' to '..last_gamepad.wButtons)
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
    vr_print('Buttons A='..tostring(btn_a)..' B='..tostring(btn_b)..' X='..tostring(btn_x)..' Y='..tostring(btn_y)..' LS='..tostring(btn_left_stick)..' LG='..tostring(btn_left_grip)..' RS='..tostring(btn_right_stick)..' RG='..tostring(btn_right_grip))
    last_gamepad.wButtons = gamepad.wButtons
    if (btn_x) then
      debug_test()
    end

    uevr_bridge:UpdateButtonState(btn_a, btn_b, btn_x, btn_y, btn_left_stick, btn_left_grip, btn_right_stick, btn_right_grip)
  end
  if (gamepad.sThumbLX or 0) ~= last_gamepad.sThumbLX or (gamepad.sThumbLY or 0) ~= last_gamepad.sThumbLY then
    local stick_left_x = gamepad.sThumbLX or 0
    local stick_left_y = gamepad.sThumbLY or 0
    vr_print('Left Stick X='..tostring(stick_left_x)..' Y='..tostring(stick_left_y)..' UI='..tostring(user_index)..' RV='..tostring(retval)..' PN='..tostring(state.dwPacketNumber))
    last_gamepad.sThumbLX = gamepad.sThumbLX or 0
    last_gamepad.sThumbLY = gamepad.sThumbLY or 0
    uevr_bridge:UpdateLeftStickState(gamepad.sThumbLX, gamepad.sThumbLY)
  end
  if (gamepad.sThumbRX or 0) ~= last_gamepad.sThumbRX or (gamepad.sThumbRY or 0) ~= last_gamepad.sThumbRY then
    local stick_right_x = gamepad.sThumbRX or 0
    local stick_right_y = gamepad.sThumbRY or 0
    vr_print('Right Stick X='..tostring(stick_right_x)..' Y='..tostring(stick_right_y))
    last_gamepad.sThumbRX = gamepad.sThumbRX or 0
    last_gamepad.sThumbRY = gamepad.sThumbRY or 0
    uevr_bridge:UpdateRightStickState(gamepad.sThumbRX, gamepad.sThumbRY)
  end

  if gamepad.bLeftTrigger ~= last_gamepad.bLeftTrigger then
    local left_trigger = gamepad.bLeftTrigger
    vr_print('Left Trigger LT='..tostring(left_trigger))
    last_gamepad.bLeftTrigger = gamepad.bLeftTrigger
    uevr_bridge:UpdateLeftTriggerState(gamepad.bLeftTrigger ~= 0)
  end
  if gamepad.bRightTrigger ~= last_gamepad.bRightTrigger then
    local right_trigger = gamepad.bRightTrigger
    vr_print('Right Trigger RT='..tostring(right_trigger))
    last_gamepad.bRightTrigger = gamepad.bRightTrigger
    uevr_bridge:UpdateRightTriggerState(gamepad.bRightTrigger ~= 0)
  end

  -- vr_print('XState: '..tostring(user_index)..' / '..tostring(retval)..' / '..tostring(gamepad.wButtons)..' / '..tostring(left_trigger)..' / '..tostring(stick_left_x)..' / '..tostring(stick_left_y)..' / '..tostring(right_trigger)..' / '..tostring(stick_right_x)..' / '..tostring(stick_right_y))
end)

uevr.sdk.callbacks.on_post_engine_tick(function(engine, delta)

end)

local spawn_once = true

uevr.sdk.callbacks.on_pre_engine_tick(function(engine, delta)
  if (uevr_bridge == nil) then
    init_bridge()
  end

end)
