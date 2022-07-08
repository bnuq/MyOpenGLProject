#version 460 core

in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;
in float state;


out vec4 fragColor;

// 카메라의 월드 좌표
uniform vec3 viewPos;

struct Light
{
    vec3 position;
    vec3 direction;
    vec2 cutoff;
    vec3 attenuation;
    
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


void main()
{
    vec3 DiffuseTexColor = texture2D(material.diffuse, texCoord).xyz;
    fragColor = vec4(DiffuseTexColor, 1.0);

    
/*   {
        // 0. Ambient Color, 기본적으로 가지는 색을 먼저 계산한다
            // Diffuse 텍스처에서 색깔을 가져온다
            vec3 DiffuseTexColor = texture2D(material.diffuse, texCoord).xyz;
            // Diffuse Color 는 Light 의 Ambient 와 조화 => 기본 색깔을 가진다
            vec3 ambient = DiffuseTexColor * light.ambient;
            // 최종 색깔을 의미하는 ResultColor
            vec3 ResultColor = ambient;
        
        // 1. Attenuation, 거리에 따른 감쇠 계산
            // 광원으로부터 떨어진 거리를 계산
            float dist = length(light.position - WorldPosition);
            vec3 distPoly = vec3(1, dist, dist * dist);
            float attenuation = 1.0f / dot(light.attenuation, distPoly);
        
        // 2. Intensity, 광원에서 나오는 빛과 광원을 바라보는 시선 사이의 각도를 계산
            // 빛 벡터, Fragment 에서 광원을 바라보는 벡터, 정규화
            vec3 LightVec = (light.position - WorldPosition) / dist;
            // 광원으로 들어가는 빛과 프레그먼트에서 광원을 바라보는 시선 사이의 각도 => cosine
            float theta = dot(LightVec, normalize(-light.direction));
            /*
                theta 의 범위가 100% 빛을 받을 수 있는 각도에 속하는 지
                아니면 조금의 빛이라도 받을 수 있는 위치에 속하는 지
                아니면 아예 빛을 받지 못하는 위치인 지 계산한다
            *
            float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0f, 1.0f);
            // 빛을 조금이라도 받는 프레그먼트인 경우, 빛에 의한 Diffuse, Specular 색을 계산한다
            if (intensity > 0.0f)
            {
                // 3. Diffuse Color 결정 <= 프레그먼트에 들어오는 빛의 총량을 알아야 한다
                    vec3 pixelNorm = normalize(WorldNormal);
                    float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
                    vec3 diffuse = LightAmount * DiffuseTexColor * light.diffuse;

                // 4. Specular Color 결정 <= 프레그먼트에서 반사된 빛이 카메라로 얼마나 들어가느냐
                    vec3 SpecularTexColor = texture2D(material.specular, texCoord).xyz;
                    vec3 viewDir = normalize(viewPos - WorldPosition);
                    vec3 reflectDir = reflect(-LightVec, pixelNorm);
                    // shininess 가 클수록, 반사 빛을 받는 각도가 작아진다
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    vec3 specular = spec * SpecularTexColor * light.specular;

                // 두 색깔의 총합을 구하고, intensity 만큼 값을 내린다
                ResultColor += (diffuse + specular) * intensity;
            }

        // 앞서 구한 감쇠를 최종 색에 적용한다
        //ResultColor *= attenuation;
        fragColor = vec4(ResultColor, 1.0);
    }*/
}