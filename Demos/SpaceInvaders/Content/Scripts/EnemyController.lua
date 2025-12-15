--- Behavior: EnemyController.lua
--- Created by Astera
---

---Called at entity initialization
---@param this Entity
function OnAwake(this)
    Log:Debug("Test message from EnemyController.lua")
end

---Called every frame, before rendering
---@param this Entity
---@param clock Clock
function OnUpdate(this, clock)
    this.transform:Translate(ImVec2(1, 0))
end

---Called every frame, after rendering
---@param this Entity
function OnLateUpdate(this)

end

---Called at entity destruction
---@param this Entity
function OnDestroyed(this)

end
