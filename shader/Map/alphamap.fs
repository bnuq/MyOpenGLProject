#version 460 core

in vec4 LightClipPos;
in vec3 TileColor;
in float curStory;


// 리턴되는 최종 프레그먼트 색깔
out vec4 fragColor;


// 0 -> 1 로 바뀌면서 들어온다
uniform float TimeRatio;
uniform sampler2D shadowMap;        // 똑같이 광원 기준에서 그린 shadow map 을 받는다
uniform uint charStory;



float ShadowCalculation(vec4 fragPosLight)
{
    // 빛 클립 공간 NDC
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;

    // NDC 좌표를 [0,1] 로 변환
    projCoords = projCoords * 0.5 + 0.5;

    // 변환된 좌표가 [0, 1] 범위를 벗어난다면, 그림자는 없다
    if(projCoords.x < 0 || projCoords.x > 1 || projCoords.y < 0 || projCoords.y > 1)
        return 0;



    // shadow map 에서 깊이 값 읽어오고
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // 현재 fragment 의 깊이 값
    float currentDepth = projCoords.z;

    // 시간이 지날수록 bias 가 커지면서 => 그림자가 작아지게 하자
    float bias = 0.02 * (0.1 + TimeRatio);
    
    if((currentDepth - bias) > closestDepth)
        return 1;
    else
        return 0;
}


void main()
{
    float shadow;
    
    // 앞면에 해당하는 fragment 에 대해서만 그림자를 계산한다
    // 캐릭터와 같은 층수에 있어야 그림자를 계산한다
    if(gl_FrontFacing && uint(curStory) == charStory)
        shadow = ShadowCalculation(LightClipPos);
    else
        shadow = 0.0;


    // 사라지는 순간 반투명이 갑자기 사라지게 하고 싶다 => 기본 알파값을 줘서, 있는 동안은 불투명한게 보이도록
    fragColor = vec4(TileColor * (1 - shadow), 0.5 - TimeRatio * 0.5 + 0.2);
}