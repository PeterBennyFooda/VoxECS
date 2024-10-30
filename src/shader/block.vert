#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in float aOcclusion;
layout (location = 4) in int aTextureID;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos; 
out float Occlusion;
flat out int TextureID;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));

    TexCoord = aTexCoord;
    Normal = aNormal;
    Occlusion = aOcclusion;
    TextureID = aTextureID;
}