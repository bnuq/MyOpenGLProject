#version 460 core


// Vertex Shader => Rasterizer �� ��ģ ��, ������ ����
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;


// ���ϵǴ� ���� �����׸�Ʈ ����
out vec4 fragColor;


// Camera �� ���� ��ǥ
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

struct Material
{
    sampler2D diffuse;    
    sampler2D specular;   
    float shininess;
};
uniform Material material;



void main()
{
    // 1. Ambient Color, �⺻������ ������ ���� ���� ����Ѵ�

        vec3 DiffuseTexColor = texture2D(material.diffuse, texCoord).xyz;
        // light �� ambient ���� ���ľ� ��¥ Ambient Color
        vec3 ambient = DiffuseTexColor * light.ambient;

    
    // 2. �̰� Directional Light => ��� Fragment �� ���� �������κ��� ���� �޴´�
        vec3 LightVec = -light.direction;


    // 3. Diffuse Color ���� <= �����׸�Ʈ�� ������ ���� �ѷ��� �˾ƾ� �Ѵ�
        vec3 pixelNorm = normalize(WorldNormal);
        float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
        vec3 diffuse = LightAmount * DiffuseTexColor * light.diffuse;


    // 4. Specular Color ���� <= �����׸�Ʈ���� �ݻ�� ���� ī�޶�� �󸶳� ������
    // ���� Phong Model ����� �ƴ�, Blinn-Phong ������� ������
    // ��, �����׸�Ʈ���� ������ ī�޶� ��θ� �ٶ� ��, �� �߰� ���͸� �̿�
    // �߰� ���Ϳ� ��� ���͸� �̿�
        vec3 SpecularTexColor = texture2D(material.specular, texCoord).xyz;
        
        vec3 viewDir = normalize(viewPos - WorldPosition);
        vec3 halfDir = normalize(viewDir + LightVec);

        float spec = pow( max(dot(halfDir, pixelNorm), 0.0), material.shininess);
        
        vec3 specular = spec * SpecularTexColor * light.specular;




    fragColor = vec4(ambient + diffuse + specular, 1.0);
}