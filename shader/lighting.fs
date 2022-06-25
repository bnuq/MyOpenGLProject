#version 330 core

// vertex shader out 이랑 일치해야 한다
in vec3 normal;
in vec2 texCoord;
in vec3 position;

// 최종 색깔
out vec4 fragColor;



uniform vec3 viewPos;           // 바라보는, 카메라의 위치


struct Light
{
    vec3 position;      // 광원의 월드 좌표계 위치
    //vec3 direction;

    vec3 direction;
    vec2 cutoff;

    vec3 attenuation;
    
    vec3 ambient;       // 빛 자체의 색깔
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;



struct Material{
    
    // 텍스처 Binding 을 통해서 넘어온다
    sampler2D diffuse;      // 텍스처
    sampler2D specular;     // 반사 색깔도, 텍스처에서 값을 읽어와서 적용한다
    
    float shininess;
};
uniform Material material;



void main() {
    

    // 텍스처를 이용한다
    vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    // 텍스처를 이용해서 ambient 값을 얻어낸다
    vec3 ambient = texColor * light.ambient;
 

    /* 
        거리에 따른 Attenuation 을 계산한다

        광원에서 떨어진 만큼 감쇠
     */
    float dist = length(light.position - position);
    vec3 distPoly = vec3(1, dist, dist * dist);

    float attenuation = 1.0f / dot(light.attenuation, distPoly);
    vec3 lightDir = (light.position - position) / dist; // normalize


    //vec3 lightDir = normalize(light.position - position);
    // directional light 로 변경
    //vec3 lightDir = normalize(-light.direction);


    // vec3 pixelNorm = normalize(normal);
    // float diff = max(dot(pixelNorm, lightDir), 0.0);        // 빛을 받는 정도
    // //vec3 diffuse = diff * material.diffuse * light.diffuse;
    // vec3 diffuse = diff * texColor * light.diffuse;
 
    // // 텍스처에서 반사 빛 값을 읽어와서
    // vec3 specColor = texture2D(material.specular, texCoord).xyz;

    // vec3 viewDir = normalize(viewPos - position);
    // vec3 reflectDir = reflect(-lightDir, pixelNorm);
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // // 반사 빛 계산
    // vec3 specular = spec * specColor * light.specular;


    // vec3 result = (ambient + diffuse + specular) * attenuation;


    // 프레그먼트가 빛을 바라보는 방향과 빛이 바라보는 방향 사이의 각 -> 코사인을 구해
    float theta = dot(lightDir, normalize(-light.direction));
    vec3 result = ambient;


    // 빛을 받는 상대적 위치를 계산
    float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0f, 1.0f);


    // 빛을 받는 위치에 있을 때
    if (intensity > 0.0f) {
        vec3 pixelNorm = normalize(normal);
        float diff = max(dot(pixelNorm, lightDir), 0.0);
        vec3 diffuse = diff * texColor * light.diffuse;

        vec3 specColor = texture2D(material.specular, texCoord).xyz;
        vec3 viewDir = normalize(viewPos - position);
        vec3 reflectDir = reflect(-lightDir, pixelNorm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = spec * specColor * light.specular;


        // 라이팅을 계산
        // 빛을 받는 세기를 고려
        result += (diffuse + specular) * intensity;
    }


    // 감쇠는 똑같이 계산
    result *= attenuation;
    fragColor = vec4(result, 1.0);


    //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}