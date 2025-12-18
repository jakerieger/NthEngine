--- Behavior: PlayerController.lua
--- Created by Astera
---

local speed = 500

--- Called at entity initialization
---@param this Entity
function OnAwake(this)
    Log:Debug(string.format("OnAwake() called for '%s'", this.name))
end

---@param transform Transform
---@param dT number
function HandleMovement(transform, dT)
    local delta = Vec2(0, 0)

    if Input:GetAction("MoveLeft") then
        delta.x = -(speed * dT)
    end

    if Input:GetAction("MoveRight") then
        delta.x = speed * dT
    end

    transform:Translate(delta)
end

---@param transform Transform
function WrapBounds(transform)
    if transform.position.x > Game:GetScreenSize().x then
        transform.position.x = 0
    end

    if transform.position.x < 0 then
        transform.position.x = Game:GetScreenSize().x
    end
end

local loaded = false

--- Called every frame, before rendering
---@param this Entity
---@param clock Clock
function OnUpdate(this, clock)
    local transform = this.transform
    local dT = clock:GetDeltaTime()

    -- Handle player movement
    HandleMovement(transform, dT)

    -- Wrap around when going out of screen bounds
    WrapBounds(transform)

    if Input:GetKeyDown(KeyCode.Space) and not loaded then
        Game:LoadScene("Level_1")
        loaded = true
    end
end

--- Called every frame, after rendering
---@param this Entity
function OnLateUpdate(this)

end

--- Called at entity destruction
---@param this Entity
function OnDestroyed(this)
    Log:Debug(string.format("OnDestroyed() called for '%s'", this.name))
end
