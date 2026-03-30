# Core Engine
A lightweight game engine written in C++ capable of 2d and 3d rendering
!!! Engine is in development. Nothing works 

# Table of Content
- [Core Engine](#core-engine)
- [Table of Content](#table-of-content)
- [Features](#features)
- [Building Engine](#building-engine)
  - [Dependencies](#dependencies)
  - [Building](#building)
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
## Building
```bash
mkdir out
cd out
cmake ..
make -j12
```
#### Single Line version
```bash
mkdir out && cd out && cmake .. && make -j12
```

<!-- # Running Editor
- Goto project root directory
```bash
cd ..
```
- move Editor executable from `out/Project/Editor/` to `Project/Editor` (This is temporary and will be removed in the future)
```bash
mv out/Project/Editor/Editor Project/Editor
```
- Goto `Project/Editor`
```bash
cd Project/Editor
```
- Run the executable
```bash
./Editor
```
### Single Line version
```bash
cd .. && mv out/Project/Editor/Editor Project/Editor && cd Project/Editor && ./Editor
``` -->
# TODO List (More will be added in the future)
- [ ] Abstract renderer and graphic (Currently Renderer is implemented directly in Vulkan.
- [ ] Model Loading
- [ ] Add Entity Component System(ECS)
- [ ] Functional editor (Can make anything with current editor)
- [ ] Scripting language support
- [ ] Editor
