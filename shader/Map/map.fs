#version 460 core

in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;
in float state;


out vec4 fragColor;

// ī�޶��� ���� ��ǥ
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
        // 0. Ambient Color, �⺻������ ������ ���� ���� ����Ѵ�
            // Diffuse �ؽ�ó���� ������ �����´�
            vec3 DiffuseTexColor = texture2D(material.diffuse, texCoord).xyz;
            // Diffuse Color �� Light �� Ambient �� ��ȭ => �⺻ ������ ������
            vec3 ambient = DiffuseTexColor * light.ambient;
            // ���� ������ �ǹ��ϴ� ResultColor
            vec3 ResultColor = ambient;
        
        // 1. Attenuation, �Ÿ��� ���� ���� ���
            // �������κ��� ������ �Ÿ��� ���
            float dist = length(light.position - WorldPosition);
            vec3 distPoly = vec3(1, dist, dist * dist);
            float attenuation = 1.0f / dot(light.attenuation, distPoly);
        
        // 2. Intensity, �������� ������ ���� ������ �ٶ󺸴� �ü� ������ ������ ���
            // �� ����, Fragment ���� ������ �ٶ󺸴� ����, ����ȭ
            vec3 LightVec = (light.position - WorldPosition) / dist;
            // �������� ���� ���� �����׸�Ʈ���� ������ �ٶ󺸴� �ü� ������ ���� => cosine
            float theta = dot(LightVec, normalize(-light.direction));
            /*
                theta �� ������ 100% ���� ���� �� �ִ� ������ ���ϴ� ��
                �ƴϸ� ������ ���̶� ���� �� �ִ� ��ġ�� ���ϴ� ��
                �ƴϸ� �ƿ� ���� ���� ���ϴ� ��ġ�� �� ����Ѵ�
            *
            float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0f, 1.0f);
            // ���� �����̶� �޴� �����׸�Ʈ�� ���, ���� ���� Diffuse, Specular ���� ����Ѵ�
            if (intensity > 0.0f)
            {
                // 3. Diffuse Color ���� <= �����׸�Ʈ�� ������ ���� �ѷ��� �˾ƾ� �Ѵ�
                    vec3 pixelNorm = normalize(WorldNormal);
                    float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
                    vec3 diffuse = LightAmount * DiffuseTexColor * light.diffuse;

                // 4. Specular Color ���� <= �����׸�Ʈ���� �ݻ�� ���� ī�޶�� �󸶳� ������
                    vec3 SpecularTexColor = texture2D(material.specular, texCoord).xyz;
                    vec3 viewDir = normalize(viewPos - WorldPosition);
                    vec3 reflectDir = reflect(-LightVec, pixelNorm);
                    // shininess �� Ŭ����, �ݻ� ���� �޴� ������ �۾�����
                    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
                    vec3 specular = spec * SpecularTexColor * light.specular;

                // �� ������ ������ ���ϰ�, intensity ��ŭ ���� ������
                ResultColor += (diffuse + specular) * intensity;
            }

        // �ռ� ���� ���踦 ���� ���� �����Ѵ�
        //ResultColor *= attenuation;
        fragColor = vec4(ResultColor, 1.0);
    }*/
}