# Wave
## Tessellation 
- Pixels per edge approach(sphere diameter in clip space). Not LOD by distance. (https://developer.nvidia.com/content/dynamic-hardware-tessellation-basics)
- Discard faces outside frustum. (working)

## Normal mapping & Displacement
- Used 2 textures for normal mapping and height displacement.
- RGB for normal, alpha chennel for height.
- Texture coordinates are scaled and translated.

## Cubemap
- working...

## Lighting
- One directional light with diffuse, Fresnel effect, Specular.

## Controls
- W, A, S, D for camera move (Hold LShift for slow moving)
- 1 - switch tessellation camera to teapot.
- 2 - switch tessellation camera to real camera.
- 3 - wireframe drawing mode.
- 4 - cull counter clockwise drawing mode.

## Etc
- Need DirectXTK in $(SolutionDir)/../DirectXTK to build & run.
