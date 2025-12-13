![](Docs/Images/banner.png)

<p align="center">
    <img src="https://github.com/jakerieger/Astera/actions/workflows/BuildLinux.yaml/badge.svg?branch=master&event=push"/>
    <img src="https://github.com/jakerieger/Astera/actions/workflows/BuildWindows.yaml/badge.svg" />
</p>

<p align="center">
    <a href="https://jakerieger.github.io/Astera/">Documentation</a> | <a href="#getting-started">Getting Started</a> | <a href="#examples">Examples</a>
</p>

---

A cross-platform 2D game engine written in C++, designed for performance and ease of use. Astera
combines modern rendering techniques with scripting flexibility to provide a comprehensive
development environment for 2D games.

## Overview

Astera is built with a focus on simplicity without sacrificing power. The engine leverages OpenGL
for graphics rendering, Lua for gameplay scripting, and implements an Entity Component System (ECS)
architecture for efficient scene management. Whether you're prototyping a new game concept or
building a complete project, Astera provides the tools you need.

## Key Features

### Architecture

- **Entity Component System (ECS)** - Flexible, composition-based entity management
- **Cross-platform** - Supports Windows and Linux
- **OpenGL Rendering** - Hardware-accelerated 2D graphics with sprite batching
- **Lua Scripting** - Full engine API exposed to Lua with complete type definitions

### Scripting

- Fully typed Lua API with IntelliSense support
- Entity lifecycle callbacks (OnAwake, OnUpdate, OnDestroyed)
- Direct access to engine systems (Input, Audio, Transform, etc.)
- Type stubs included for enhanced IDE support

### Rendering

- Sprite rendering with texture support
- Transform system (position, rotation, scale)
- Component-based renderer architecture

### Physics

- 2D Rigidbody physics
- Support for dynamic and static bodies
- Configurable mass, inertia, friction, and restitution
- Gravity and damping controls

### Scene Management

- XML-based scene serialization
- Component composition system
- Entity hierarchy support
- Hot-reloadable assets

### Audio

- Sound playback with looping support
- Volume controls
- Background music and sound effects

### Input

- Keyboard input system
- Polling-based input handling
- *Configurable key mappings

> * work in progress

## Getting Started

### Prerequisites

- CMake 3.14 or higher
- C++20 compatible compiler
- OpenGL 4.6
- Lua >= 5.1

### Building

Clone the repository:

```bash
git clone https://github.com/jakerieger/Astera.git
cd Astera
```

Build using CMake:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Alternatively, use the provided build script:

```bash
lua build.lua
```

> [build.lua](build.lua) contains a lot of helpful commands. Run `lua build.lua --help` for a full
> list.

## Examples

### Lua Script Example

The following script demonstrates a basic entity controller with movement and audio:

```lua
--- BallController.lua
--- Movement controller with background music

ballSpeed = 500
bgMusicID = 1

function OnAwake(this)
    Log:Debug(string.format("OnAwake() called for entity: %s", this.name))

    if AudioPlayer:IsInitialized() then
        AudioPlayer:SetSoundVolume(bgMusicID, 0.5)
        AudioPlayer:PlaySound(bgMusicID, true)
    end
end

function OnUpdate(this, clock)
    local transform = this.transform
    local dT = clock:GetDeltaTime()
    local newPosition = Vec2(0, 0)

    if Input:GetKeyDown(KeyCode.W) then
        newPosition.y = ballSpeed * dT
    end
    if Input:GetKeyDown(KeyCode.S) then
        newPosition.y = -ballSpeed * dT
    end
    if Input:GetKeyDown(KeyCode.A) then
        newPosition.x = -ballSpeed * dT
    end
    if Input:GetKeyDown(KeyCode.D) then
        newPosition.x = ballSpeed * dT
    end

    transform:Translate(newPosition)
end

function OnDestroyed(this)
    Log:Debug(string.format("OnDestroyed() called for entity: %s", this.name))
end
```

### Scene Definition Example

Scenes are defined using XML format:

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
                    <Script id="0">BallController.lua</Script>
                </Behavior>
                <Rigidbody2D>
                    <BodyType>Dynamic</BodyType>
                    <Mass>1.0</Mass>
                    <Restitution>0.5</Restitution>
                    <Friction>0.3</Friction>
                    <GravityScale>1.0</GravityScale>
                </Rigidbody2D>
            </Components>
        </Entity>
    </Entities>
</Scene>
```

> Scene serialization is currently in development.

## Documentation

Full API documentation is generated using Doxygen and hosted at:
[https://jakerieger.github.io/Astera/](https://jakerieger.github.io/Astera/)

Lua type definitions can be found in [EngineContent/Scripts/types/](EngineContent/Scripts/types/)
for IDE autocomplete support.

## Development Status

Astera is an ongoing personal project and is provided as-is. The engine is functional but under
active development. Features and APIs may change as the project evolves.

**Current Status:**

- Core engine systems functional
- Scripting API complete for basic 2D games
- Asset loading in original format only (no compression/packing yet)

**Known Limitations:**

- No built-in editor
- Asset pipeline requires manual organization
- Limited documentation coverage
- Scene format may change in future versions
- Mix of asserts and exceptions, consistent error handling is a work-in-progress

## Contributing

While Astera is primarily a personal project, issue reports are welcome. However, please note that
issues may not be addressed until the engine reaches an official release milestone.

## License

Astera is licensed under the Apache License 2.0. See [LICENSE](LICENSE) for details.

## Acknowledgments

See [THIRD_PARTY.md](THIRD_PARTY.md) for a list of third-party licenses for libraries used by
Astera.

## Contact

For questions or feedback, please open an issue on GitHub.

---

**Note:** This is a work-in-progress project. Use at your own risk for production purposes.