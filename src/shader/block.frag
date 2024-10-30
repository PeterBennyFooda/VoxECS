#version 330 core

struct Material
{
    vec3 ambient;
    sampler2D diffuse;
    vec3      specular;
    float     shininess;
};

struct PointLight 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct DirLight
{
	vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;
  
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;  
in float Occlusion;
flat in int TextureID;

uniform sampler2DArray textureArray;

uniform PointLight pointLight;
uniform DirLight dirLight;
uniform Material material;

vec4 GetTextureLayer();
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  

void main()
{
    vec3 result = vec3(0);

    result += CalcDirLight(dirLight, Normal, normalize(-FragPos));
    result += CalcPointLight(pointLight, Normal, FragPos, normalize(-FragPos));

    result *= Occlusion;

    FragColor = vec4(result, 1.0);
}

// get the texture layer based on the height
vec4 GetTextureLayer()
{
    vec4 tex = texture(textureArray, vec3(TexCoord, TextureID));

    if(tex.a < 0.1)
        discard;

    // Sample the appropriate texture layer from the texture array
    return tex;
}

// calculates the color when using a directional light
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // the direction is from the fragment towards the light source for our calculations
    // but we prefer to have the input direction from the light source to the fragment
    // so we need to invert the input direction
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // get texture based on the height
    vec4 tex = GetTextureLayer();
        
    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    //vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoord));
    //vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse,  TexCoord));
    //vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    vec3 ambient  = light.ambient  * vec3(tex);
    vec3 diffuse  = light.diffuse  * diff * vec3(tex);
    
    //return (ambient + diffuse + specular);
    return (ambient + diffuse);
}  

// calculates the color when using a point light
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // get texture based on the height
    vec4 tex = GetTextureLayer();
    
    // calculate direction vector between light source and fragment
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);  

    // ambient
    //vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 ambient = light.ambient * vec3(tex);

    // calculate the diffuse impact of the light on the current fragment
    // if the angle between both vectors is greater than 90 degrees
    // diff will be less than 0 so we clamp it to 0
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord)); 
    vec3 diffuse = light.diffuse * diff * vec3(tex); 

    // anttenuation
    // linear equation looks fake so we use quadratic instead
    // source: https://learnopengl.com/Lighting/Light-casters
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
    		    light.quadratic * (distance * distance));  

    // apply attenuation
    ambient  *= attenuation;
    diffuse  *= attenuation;

    return (ambient + diffuse);
}