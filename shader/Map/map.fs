#version 330 core

// vertex shader out ?��?�� ?��치해?�� ?��?��
in vec3 normal;
in vec2 texCoord;
in vec3 position;

// 최종 ?���?
out vec4 fragColor;


uniform bool contact;
uniform vec3 viewPos;


struct Light
{
    vec3 position;      // 광원?�� ?��?�� 좌표�? ?���?
    //vec3 direction;

    vec3 direction;
    vec2 cutoff;

    vec3 attenuation;
    
    vec3 ambient;       // �? ?��체의 ?���?
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;



struct Material{
    
    // ?��?���? Binding ?�� ?��?��?�� ?��?��?��?��
    sampler2D diffuse;      // ?��?���?
    sampler2D specular;     // 반사 ?��깔도, ?��?��처에?�� 값을 ?��?��????�� ?��?��?��?��
    
    float shininess;
};
uniform Material material;



void main() {
    
    if(contact)
        fragColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    else
    {
        fragColor = glm::vec4(0.0, 1.0, 0.0, 1.0);
    }



    // // ?��?��처�?? ?��?��?��?��
    // vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    // // ?��?��처�?? ?��?��?��?�� ambient 값을 ?��?��?��?��
    // vec3 ambient = texColor * light.ambient;
 

    // /* 
    //     거리?�� ?���? Attenuation ?�� 계산?��?��

    //     광원?��?�� ?��?���? 만큼 감쇠
    //  */
    // float dist = length(light.position - position);
    // vec3 distPoly = vec3(1, dist, dist * dist);

    // float attenuation = 1.0f / dot(light.attenuation, distPoly);
    // vec3 lightDir = (light.position - position) / dist; // normalize


    // //vec3 lightDir = normalize(light.position - position);
    // // directional light �? �?�?
    // //vec3 lightDir = normalize(-light.direction);


    // // vec3 pixelNorm = normalize(normal);
    // // float diff = max(dot(pixelNorm, lightDir), 0.0);        // 빛을 받는 ?��?��
    // // //vec3 diffuse = diff * material.diffuse * light.diffuse;
    // // vec3 diffuse = diff * texColor * light.diffuse;
 
    // // // ?��?��처에?�� 반사 �? 값을 ?��?��????��
    // // vec3 specColor = texture2D(material.specular, texCoord).xyz;

    // // vec3 viewDir = normalize(viewPos - position);
    // // vec3 reflectDir = reflect(-lightDir, pixelNorm);
    // // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // // // 반사 �? 계산
    // // vec3 specular = spec * specColor * light.specular;


    // // vec3 result = (ambient + diffuse + specular) * attenuation;


    // // ?��?��그먼?���? 빛을 바라보는 방향�? 빛이 바라보는 방향 ?��?��?�� �? -> 코사?��?�� 구해
    // float theta = dot(lightDir, normalize(-light.direction));
    // vec3 result = ambient;


    // // 빛을 받는 ?��????�� ?��치�?? 계산
    // float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0f, 1.0f);


    // // 빛을 받는 ?��치에 ?��?�� ?��
    // if (intensity > 0.0f) {
    //     vec3 pixelNorm = normalize(normal);
    //     float diff = max(dot(pixelNorm, lightDir), 0.0);
    //     vec3 diffuse = diff * texColor * light.diffuse;

    //     vec3 specColor = texture2D(material.specular, texCoord).xyz;
    //     vec3 viewDir = normalize(viewPos - position);
    //     vec3 reflectDir = reflect(-lightDir, pixelNorm);
    //     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //     vec3 specular = spec * specColor * light.specular;


    //     // ?��?��?��?�� 계산
    //     // 빛을 받는 ?��기�?? 고려
    //     result += (diffuse + specular) * intensity;
    // }


    // // 감쇠?�� ?��같이 계산
    // result *= attenuation;
    // fragColor = vec4(result, 1.0);


    // //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}