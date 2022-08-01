#version 460 core

in vec3 WorldNormal;
in vec3 WorldPosition;
in vec3 DiffColor;


out vec4 fragColor;


// 카메라의 월드 좌표
uniform vec3 viewPos;


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



void main()
{
    // 1. Ambient Color, 기본적으로 가지는 색을 먼저 계산한다

        vec3 ambient = DiffColor * light.ambient;


    // 2. 이건 Directional Light => 모든 Fragment 가 같은 방향으로부터 빛을 받는다
    
        vec3 LightVec = -light.direction;


    // 3. Diffuse Color 결정 <= 프레그먼트에 들어오는 빛의 총량을 알아야 한다

        vec3 pixelNorm = normalize(WorldNormal);
        float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
        vec3 diffuse = LightAmount * DiffColor * light.diffuse;


    // 4. Specular Color 결정 <= 프레그먼트에서 반사된 빛이 카메라로 얼마나 들어가느냐
    // 기존 Phong Model 방식이 아닌, Blinn-Phong 방식으로 구하자
    // 즉, 프레그먼트에서 광원과 카메라 모두를 바라볼 때, 그 중간 벡터를 이용
    // 중간 벡터와 노멀 벡터를 이용

        vec3 viewDir = normalize(viewPos - WorldPosition);
        vec3 halfDir = normalize(viewDir + LightVec);

        float spec = pow( max(dot(halfDir, pixelNorm), 0.0), 75.0);
        
        vec3 specular = spec * light.specular;



    fragColor = vec4(ambient + diffuse + specular, 1.0);
}