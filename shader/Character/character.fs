#version 460 core


// Vertex Shader => Rasterizer �� ��ģ ��, ������ ����
in vec2 texCoord;
in vec3 WorldNormal;
in vec3 WorldPosition;
in vec4 LightClipPos;



// ���ϵǴ� ���� �����׸�Ʈ ����
out vec4 fragColor;


// Camera �� ���� ��ǥ
uniform vec3 viewPos;

uniform sampler2D shadowMap;



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



// ���� fragment �� �׸��ڰ� ���� ��, �׸��ڰ� �ƴ� �� �Ǵ��ϴ� �Լ�
// 0 = �׸��� �ƴ�, 1 = �׸��ڰ� ����
float ShadowCalculation(vec4 fragPosLight, vec3 normal, vec3 lightDir)
{
    /* 
        frag Pos Ligth => �����׸�Ʈ�� �ش��ϴ� ��ǥ�� ��~Ŭ�� ���������� ��ǥ
        Ŭ�� ������ǥ => w �� ������ => �� �������� NDC ��ǥ = [-1, +1] ������ 3���� ��ǥ�� ���� �� �ִ�
     */
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;


    // transform to [0,1] range
    // NDC ��ǥ�� [0,1] �� ��ȯ
    projCoords = projCoords * 0.5 + 0.5;


    // get closest depth value from light��s perspective (using [0,1] range fragPosLight as coords)
    // ������ ���� ��ǥ�� �ؽ�ó ��ǥó�� ���, shadow map ���� ���� ���� �о�� �� �ִ�
    // ���� �����׸�Ʈ�� �������忡�� �׷��� ��, shadow map ���� �ش� ��ġ�� �׷����� ���̶� �����Ͽ�
    float closestDepth = texture(shadowMap, projCoords.xy).r;


    // get depth of current fragment from light��s perspective
    float currentDepth = projCoords.z;


    // check whether current frag pos is in shadow
    // �����׸�Ʈ�� ��ֺ��Ϳ� �� ���Ͱ� ��ġ�Ҽ��� ���� bias �� ����ȴ�
    float bias = max(0.02 * (1.0 - dot(normal, lightDir)), 0.001);
    float shadow = 0.0;

    if((currentDepth - bias) > closestDepth)
        return 1;
    else
        return 0;
}



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



    vec3 result = ambient + diffuse + specular;
    float shadow = ShadowCalculation(LightClipPos, pixelNorm, LightVec);

    fragColor = vec4(result * (1 - shadow), 1.0);
}