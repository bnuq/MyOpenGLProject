#version 460 core


// Vertex Shader => Rasterizer 를 거친 후, 보간된 값들
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;
in vec4 LightClipPos;



// 리턴되는 최종 프레그먼트 색깔
out vec4 fragColor;


// Camera 의 월드 좌표
uniform vec3 viewPos;

uniform sampler2D shadowMap;



// Directional Light 라 생각
struct Light
{
    vec3 position;
    vec3 direction;

    vec3 ambient; 
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

struct Material
{
    sampler2D diffuse;    
    sampler2D specular;   
    float shininess;
};
uniform Material material;



// 현재 fragment 가 그림자가 졌는 지, 그림자가 아닌 지 판단하는 함수
// 0 = 그림자 아님, 1 = 그림자가 졌다
float ShadowCalculation(vec4 fragPosLight, vec3 normal, vec3 lightDir)
{
    /* 
        frag Pos Ligth => 프레그먼트에 해당하는 좌표의 빛~클립 공간에서의 좌표
        클립 공간좌표 => w 로 나누면 => 빛 공간에서 NDC 좌표 = [-1, +1] 범위의 3차원 좌표를 얻을 수 있다
     */
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;


    // transform to [0,1] range
    // NDC 좌표를 [0,1] 로 변환
    projCoords = projCoords * 0.5 + 0.5;


    // get closest depth value from light’s perspective (using [0,1] range fragPosLight as coords)
    // 위에서 구한 좌표를 텍스처 좌표처럼 사용, shadow map 에서 깊이 값을 읽어올 수 있다
    // 현재 프레그먼트를 광원입장에서 그렸을 때, shadow map 에서 해당 위치에 그려졌을 것이란 가정하에
    float closestDepth = texture(shadowMap, projCoords.xy).r;


    // get depth of current fragment from light’s perspective
    float currentDepth = projCoords.z;


    // check whether current frag pos is in shadow
    // 프레그먼트의 노멀벡터와 빛 벡터가 일치할수록 작은 bias 가 적용된다
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.001);
    float shadow = 0.0;

    if((currentDepth - bias) > closestDepth)
        return 1;
    else
        return 0;
}



void main()
{
    // 1. Ambient Color, 기본적으로 가지는 색을 먼저 계산한다

        vec3 DiffuseTexColor = texture2D(material.diffuse, texCoord).xyz;
        // light 의 ambient 까지 합쳐야 진짜 Ambient Color
        vec3 ambient = DiffuseTexColor * light.ambient;

    
    // 2. 이건 Directional Light => 모든 Fragment 가 같은 방향으로부터 빛을 받는다
        vec3 LightVec = -light.direction;


    // 3. Diffuse Color 결정 <= 프레그먼트에 들어오는 빛의 총량을 알아야 한다
        vec3 pixelNorm = normalize(WorldNormal);
        float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
        vec3 diffuse = LightAmount * DiffuseTexColor * light.diffuse;


    // 4. Specular Color 결정 <= 프레그먼트에서 반사된 빛이 카메라로 얼마나 들어가느냐
    // 기존 Phong Model 방식이 아닌, Blinn-Phong 방식으로 구하자
    // 즉, 프레그먼트에서 광원과 카메라 모두를 바라볼 때, 그 중간 벡터를 이용
    // 중간 벡터와 노멀 벡터를 이용
        vec3 SpecularTexColor = texture2D(material.specular, texCoord).xyz;
        
        vec3 viewDir = normalize(viewPos - WorldPosition);
        vec3 halfDir = normalize(viewDir + LightVec);

        float spec = pow( max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        
        vec3 specular = spec * SpecularTexColor * light.specular;



    vec3 result = ambient + diffuse + specular;
    float shadow = ShadowCalculation(LightClipPos, pixelNorm, LightVec);

    fragColor = vec4(result * (1 - shadow), 1.0);
}