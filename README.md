# Nth Engine

Nth Engine (named after the fact that I've lost count of what iteration I'm on) is a cross-platform,
2D game engine
written in C++. It uses OpenGL for rendering, Lua for scripting, and an Entity Component System for
scene management.

The bulk of the engine's API is exposed to Lua and available to scripts. Here's an example script
from the
Sandbox demo project:

```lua
--- Behavior: BallController.lua
--- Created by Nth Engine
---

ballSpeed = 500
bgMusicID = 1

---@param this Entity
function OnAwake(this)
    Log:Debug(string.format("OnAwake() called for entity: %s", this.name))

    if AudioPlayer:IsInitialized() then
        AudioPlayer:SetSoundVolume(bgMusicID, 0.5)
        AudioPlayer:PlaySound(bgMusicID, true)
    end
end

---Handle movement of ball sprite based on input
---@param transform Transform
---@param dT number
function HandleMovement(transform, dT)
    local newPosition = Vec2(0, 0)

    -- Check is one of our movement keys is pressed
    if Input:GetKeyDown(KeyCode.W) then
        -- Apply a new value to the correct axis scaled by delta time
        newPosition.y = (ballSpeed * dT)
    end
    if Input:GetKeyDown(KeyCode.S) then
        newPosition.y = -(ballSpeed * dT)
    end
    if Input:GetKeyDown(KeyCode.A) then
        newPosition.x = -(ballSpeed * dT)
    end
    if Input:GetKeyDown(KeyCode.D) then
        newPosition.x = (ballSpeed * dT)
    end

    -- Translate our entity by the corresponding new amount
    transform:Translate(newPosition)
end

---@param this Entity
---@param clock Clock
function OnUpdate(this, clock)
    HandleMovement(this.transform, clock:GetDeltaTime())
end

---@param this Entity
function OnDestroyed(this)
    Log:Debug(string.format("OnDestroyed() called for entity: %s", this.name))
end

```

The Lua API is fully typed and documented (stubs can be found
in [EngineContent/Scripts/types](EngineContent/Scripts/types)).

Nth Engine uses XML to describe scenes and currently only supports loading content in original
authored form (no packing or compression yet).

Sandbox demo scene:

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<Scene name="Sandbox">
    <Entities>
        <Entity id="0" name="Ball">
            <Components>
                <Transform>
                    <Position x="640" y="360"/>
                    <Rotation x="0" y="0"/>
                    <Scale x="64" y="64"/>
                </Transform>
                <SpriteRenderer>
                    <Texture>ball.png</Texture>
                </SpriteRenderer>
                <Behavior>
                    <!-- TODO: Support multiple scripts per entity -->
                    <Script id="0">BallController.lua</Script>
                </Behavior>
            </Components>
        </Entity>
    </Entities>
</Scene>
```

This is an ongoing project and a license will be attached once the project has reached a presentable
state.