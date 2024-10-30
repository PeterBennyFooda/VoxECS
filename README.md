# VoxECS
VoxECS is a voxel game engine designed with an Entity Component System (ECS) architecture, enabling efficient and modular management of complex voxel-based worlds (work in progress).

## Technical Details

### Simple Terrain Generation
Employed 2D and 3D Perlin noise to generate smooth, natural terrains, and utilized Poisson Disc Sampling for tree placement.

<img src= "https://github.com/user-attachments/assets/8a980a4d-dd6d-4c58-b805-bd30aebb0d20" width ="382.5" height="288">
<img src= "https://github.com/user-attachments/assets/3a00deed-da57-46d5-a76c-ea31e9021094" width ="382.5" height="288">
<img src= "https://github.com/user-attachments/assets/f4aa6cd4-c993-4b1a-88ff-9b87dd0cfc7f" width ="382.5" height="288">
<img src= "https://github.com/user-attachments/assets/687af15d-7f3f-4fd7-93d8-aab2ccaf2b8f" width ="382.5" height="288">

### Chunk Management
Combined voxels into chunks mesh and culled unseen faces to optimize the meshing process.  
I plan to implement greedy meshing to further reduce the number of vertices.  
<img src= "https://github.com/user-attachments/assets/1db7d377-dab8-42f2-b955-86451db1bad0" width ="405" height="406.8">
<img src= "https://github.com/user-attachments/assets/2a6fbb9a-8a50-4b48-9622-d4085d4c7b08" width ="405" height="406.8">

### Character Controls
Enabled player movement using both mouse and keyboard, along with block placement and removal functionality.  
<img src= "https://github.com/user-attachments/assets/b03fd4dd-1597-4b9a-ae0a-7f9cc8ec6599" width ="383.4" height="216">
<img src= "https://github.com/user-attachments/assets/b0603b61-2be7-4fd0-a652-0f664efca6e6" width ="383.4" height="216">

### Ambient Occlusion
Calculated ambient occlusion for each vertex using information solely from adjacent cubes, rather than relying on post-processing.

<img src= "https://github.com/user-attachments/assets/bc00f6d7-7013-48a2-92e4-a49a8e4f60f2" width ="390" height="360">  
<img src= "https://github.com/user-attachments/assets/2e5cbf3a-f367-4631-948c-6f9004277c5c" width ="390" height="360">  
<img src= "https://github.com/user-attachments/assets/f00ffb75-2921-4cde-b98f-81ba6b998fa1" width ="390" height="360">  
<img src= "https://github.com/user-attachments/assets/1bb7ee73-94d1-41a7-9240-31839f0fa0ba" width ="390" height="360">  
