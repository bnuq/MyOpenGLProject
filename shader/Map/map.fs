#version 460 core

in vec3 WorldNormal;
in vec3 WorldPosition;
in vec3 DiffColor;


out vec4 fragColor;


// ī�޶��� ���� ��ǥ
uniform vec3 viewPos;


// Directional Light �� ����
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
    // 1. Ambient Color, �⺻������ ������ ���� ���� ����Ѵ�

        vec3 ambient = DiffColor * light.ambient;


    // 2. �̰� Directional Light => ��� Fragment �� ���� �������κ��� ���� �޴´�
    
        vec3 LightVec = -light.direction;


    // 3. Diffuse Color ���� <= �����׸�Ʈ�� ������ ���� �ѷ��� �˾ƾ� �Ѵ�

        vec3 pixelNorm = normalize(WorldNormal);
        float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
        vec3 diffuse = LightAmount * DiffColor * light.diffuse;


    // 4. Specular Color ���� <= �����׸�Ʈ���� �ݻ�� ���� ī�޶�� �󸶳� ������
    // ���� Phong Model ����� �ƴ�, Blinn-Phong ������� ������
    // ��, �����׸�Ʈ���� ������ ī�޶� ��θ� �ٶ� ��, �� �߰� ���͸� �̿�
    // �߰� ���Ϳ� ��� ���͸� �̿�

        vec3 viewDir = normalize(viewPos - WorldPosition);
        vec3 halfDir = normalize(viewDir + LightVec);

        float spec = pow( max(dot(halfDir, pixelNorm), 0.0), 75.0);
        
        vec3 specular = spec * light.specular;



    fragColor = vec4(ambient + diffuse + specular, 1.0);
}