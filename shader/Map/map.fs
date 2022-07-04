#version 460 core

in vec3 normal;
in vec2 texCoord;
in vec3 position;
in vec2 bBool;

int instanceID;

out vec4 fragColor;


struct FloorStr
{
    vec3 FloorPos;
    int FloorTouched;
};


layout (std430, binding = 8) buffer FloorBuffer
{
    FloorStr floorstr[];
};



uniform vec3 viewPos;


struct Light
{
    vec3 position;
    //vec3 direction;

    vec3 direction;
    vec2 cutoff;

    vec3 attenuation;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;



struct Material{
   
    sampler2D diffuse;
    sampler2D specular;
    
    float shininess;
};
uniform Material material;



void main() {

    if(floorstr[instanceID].FloorTouched == 1)
        fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
        fragColor = vec4(1.0, 1.0, 0.0, 1.0);



    // // ??€μ²λ?? ?΄?©??€
    // vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    // // ??€μ²λ?? ?΄?©?΄? ambient κ°μ ?»?΄?Έ?€
    // vec3 ambient = texColor * light.ambient;
 

    // /* 
    //     κ±°λ¦¬? ?°λ₯? Attenuation ? κ³μ°??€

    //     κ΄μ?? ?¨?΄μ§? λ§νΌ κ°μ 
    //  */
    // float dist = length(light.position - position);
    // vec3 distPoly = vec3(1, dist, dist * dist);

    // float attenuation = 1.0f / dot(light.attenuation, distPoly);
    // vec3 lightDir = (light.position - position) / dist; // normalize


    // //vec3 lightDir = normalize(light.position - position);
    // // directional light λ‘? λ³?κ²?
    // //vec3 lightDir = normalize(-light.direction);


    // // vec3 pixelNorm = normalize(normal);
    // // float diff = max(dot(pixelNorm, lightDir), 0.0);        // λΉμ λ°λ ? ?
    // // //vec3 diffuse = diff * material.diffuse * light.diffuse;
    // // vec3 diffuse = diff * texColor * light.diffuse;
 
    // // // ??€μ²μ? λ°μ¬ λΉ? κ°μ ?½?΄????
    // // vec3 specColor = texture2D(material.specular, texCoord).xyz;

    // // vec3 viewDir = normalize(viewPos - position);
    // // vec3 reflectDir = reflect(-lightDir, pixelNorm);
    // // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // // // λ°μ¬ λΉ? κ³μ°
    // // vec3 specular = spec * specColor * light.specular;


    // // vec3 result = (ambient + diffuse + specular) * attenuation;


    // // ?? κ·Έλ¨Ό?Έκ°? λΉμ λ°λΌλ³΄λ λ°©ν₯κ³? λΉμ΄ λ°λΌλ³΄λ λ°©ν₯ ?¬?΄? κ°? -> μ½μ¬?Έ? κ΅¬ν΄
    // float theta = dot(lightDir, normalize(-light.direction));
    // vec3 result = ambient;


    // // λΉμ λ°λ ?????  ?μΉλ?? κ³μ°
    // float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0f, 1.0f);


    // // λΉμ λ°λ ?μΉμ ?? ?
    // if (intensity > 0.0f) {
    //     vec3 pixelNorm = normalize(normal);
    //     float diff = max(dot(pixelNorm, lightDir), 0.0);
    //     vec3 diffuse = diff * texColor * light.diffuse;

    //     vec3 specColor = texture2D(material.specular, texCoord).xyz;
    //     vec3 viewDir = normalize(viewPos - position);
    //     vec3 reflectDir = reflect(-lightDir, pixelNorm);
    //     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //     vec3 specular = spec * specColor * light.specular;


    //     // ?Ό?΄?? κ³μ°
    //     // λΉμ λ°λ ?ΈκΈ°λ?? κ³ λ €
    //     result += (diffuse + specular) * intensity;
    // }


    // // κ°μ ? ?κ°μ΄ κ³μ°
    // result *= attenuation;
    // fragColor = vec4(result, 1.0);


    // //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}