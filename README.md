# SplitScreen for O3DE

An experimental O3DE gem that renders two to four local player cameras into a single game window.

Each player camera renders into its own offscreen Atom render pipeline. A compositor pass then combines the resulting images into the final window viewport. The gem does not require LyShine to arrange the camera views.

## Requirements

- O3DE 26.05
- Atom renderer
- Currently tested on Windows with DX12

## Installation

1. Clone this repository into an O3DE gem directory.
2. Register the gem with O3DE if the directory is not already registered.
3. Enable the `SplitScreen` gem in your project.
4. Configure and build the project again.

## Usage

1. Create two to four entities with Camera components.
2. Add the **Player Viewport** component to every camera that belongs to a local player.
3. Leave **Activated** enabled on the cameras that should participate in split screen.
4. Enter Game Mode or run the project.

Split screen starts only when at least two active **Player Viewport** components are present. With zero or one active component, the project keeps its normal single-camera rendering.

The current layouts are:

- 2 players: vertical split, left and right.
- 3 players: 2x2 grid with the lower-right area unused.
- 4 players: 2x2 grid.

## Current limitations

- The gem is experimental and has not been tested across a wide range of projects or render pipeline configurations.
- LyShine and ImGui overlays are not isolated per player yet. UI passes may be rendered by player pipelines as well as by the final window pipeline.
- Per-player HUD canvases, per-player UI state, and viewport-aware input routing are not implemented.
- ImGui is currently expected to behave as a global overlay. Separate per-player ImGui contexts are not supported.
- Rendering cost increases with every player because each camera uses a separate Atom render pipeline.
- Runtime camera activation, viewport resizing, post-processing combinations, and non-DX12 backends need more testing.
- Only the built-in layouts listed above are available; custom layout configuration is not implemented.

## License

MIT
