# 3D Renderer

Writing a 3D software renderer to learn the basics of 3D graphics.

Uses SDL for window and input management and for drawing pixels to a color buffer.

## Compiling
Following the following steps to build and run the project:
1. Open `/projects/Pikuma3DGraphics.sln` in Visual Studio 2022.
2. Right click on **3DRenderer** project and go to **Properties**.
3. Go to **Debuggin0g** -> And set `Working Directory` to `$(SolutionDir)..\data\` for **All Configurations**.
4. Click OK.

## Controls
- `Escape` to quit.
- `F9` to take a screenshot. Saved as `screenshot-{timestamp}.bmp` in the executable directory.
- `p` to pause.
- `f` to toggle logging of FPS and average frame time.
- `1` to draw only wireframe and vertices.
- `2` to display only wireframe.
- `3` to draw filled.
- `4` to draw both filled and wireframe.
- `d` to disable backface culling.
- `c` to enable backface culling.
