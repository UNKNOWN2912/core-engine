# Core Engine
A lightweight game engine written in C++ capable of 2d and 3d rendering
!!! Engine is in development. Nothing works 

# Table of Content
- [Core Engine](#core-engine)
- [Table of Content](#table-of-content)
- [Features](#features)
- [Building Engine](#building-engine)
  - [Dependencies](#dependencies)
  - [Building editor](#building-editor)
- [TODO List (More will be added in the future)](#todo-list-more-will-be-added-in-the-future)
# Features
<!-- - Editor(work in progress) -->
- Rendering using Vulkan(working in progress)
<!-- - Node base scene -->
# Building Engine
## Dependencies
- `cmake`
- `g++`
- `vulkan sdk`
## Building editor
```bash
./buildAndRunEditor
```
#### Single Line version
```bash
mkdir out && cd out && cmake .. && make -j12
```
# TODO List (More will be added in the future)
- [x] Abstract renderer and graphic (Currently Renderer is implemented directly in Vulkan.
- [ ] Model Loading
- [x] Add Entity Component System(ECS)
- [ ] Scripting language support
- [x] Editor
