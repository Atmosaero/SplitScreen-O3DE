# SplitScreen for O3DE

Render 2–4 local cameras in one window, each with its own optional LyShine HUD.

## Setup

1. Clone this repository, register and enable the **SplitScreen** gem in your project. Rebuild the project and let Asset Processor finish.
2. Create 2–4 camera entities. Add **Camera** and **Lua Script** components to each; select [PlayerViewport.lua](Assets/Scripts/SplitScreen/PlayerViewport.lua).
3. Give each script a unique **Slot** (`0`–`3`). Optionally set **HudCanvas** to a UI canvas path, such as `UI/PlayerHud.uicanvas`. Each script loads its own instance; use anchors/device scaling for responsive HUD layouts.
4. Move each camera with your game's player controller and enter Game Mode. Two players split left/right; three or four use a 2×2 grid. The script handles registration and cleanup.

Custom scripts can use `SplitScreenRequestBus.Broadcast`: `RegisterLocalPlayer(camera, active)`, `SetLocalPlayerCanvas(camera, canvas, slot)`, `SetLocalPlayerActivated(camera, active)` and `UnregisterLocalPlayer(camera)`. Calls return success/failure; retry registration after scene activation. Unregister before unloading the HUD or destroying the camera.

Requires the standard Atom/LyShine pipeline. Input routing belongs to your game. Tested with O3DE SDK 2.7.0 on Windows/DX12.
