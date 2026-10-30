-- Attach to an entity with a Camera component. Each camera owns its HUD instance.
local PlayerViewport = {
    Properties = {
        Slot = { default = 0, description = "Unique local player slot: 0, 1, 2 or 3" },
        HudCanvas = { default = "", description = "Optional HUD path, e.g. UI/PlayerHud.uicanvas" }
    }
}

function PlayerViewport:OnActivate()
    self.canvas = EntityId()
    self.registered = false
    self.tick = TickBus.Connect(self)
end

function PlayerViewport:OnTick()
    -- Wait until the camera and its render scene have activated.
    if not SplitScreenRequestBus.Broadcast.RegisterLocalPlayer(self.entityId, true) then
        return
    end
    self.registered = true
    self.tick:Disconnect()
    self.tick = nil

    local slot = self.Properties.Slot
    if slot < 0 or slot > 3 or slot ~= math.floor(slot) then
        Debug.Log("SplitScreen: Slot must be an integer from 0 to 3")
        self:OnDeactivate()
        return
    end

    if self.Properties.HudCanvas ~= "" then
        self.canvas = UiCanvasManagerBus.Broadcast.LoadCanvas(self.Properties.HudCanvas)
        if not self.canvas:IsValid() then
            Debug.Log("SplitScreen: Could not load HUD " .. self.Properties.HudCanvas)
            self:OnDeactivate()
            return
        end
        UiCanvasBus.Event.SetIsPositionalInputSupported(self.canvas, false)
        UiCanvasBus.Event.SetIsNavigationSupported(self.canvas, false)
    end

    if not SplitScreenRequestBus.Broadcast.SetLocalPlayerCanvas(self.entityId, self.canvas, slot) then
        Debug.Log("SplitScreen: Could not bind player; check unique slots and canvases")
        self:OnDeactivate()
    end
end

function PlayerViewport:OnDeactivate()
    if self.tick then
        self.tick:Disconnect()
        self.tick = nil
    end
    if self.registered then
        SplitScreenRequestBus.Broadcast.UnregisterLocalPlayer(self.entityId)
        self.registered = false
    end
    if self.canvas and self.canvas:IsValid() then
        UiCanvasManagerBus.Broadcast.UnloadCanvas(self.canvas)
        self.canvas = EntityId()
    end
end

return PlayerViewport
