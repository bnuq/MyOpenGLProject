#version 460 core

// vertex shader �� ���� ������ ����
in float IsVisible;

in float curStory;

in vec3 WorldNormal;
in vec3 WorldPosition;
in vec3 DiffColor;

in vec4 LightClipPos;


out vec4 fragColor;


uniform vec3 viewPos;               // ī�޶��� ���� ��ǥ
uniform float diffRatio;            // ���� diffuse color ����

uniform sampler2D shadowMap;        // ���� ���ؿ��� �׸� shadow map

uniform uint charStory;

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


    // �ش� tile fragment �� �� NDC ��ǥ�� ��ȯ�� ����� [0,1] �� ����ٸ�, �� Ŭ�� ������ ��� ���̶�� ��
    // �׸��ڴ� ����
    if(projCoords.x < 0 || projCoords.x > 1 || projCoords.y < 0 || projCoords.y > 1)
        return 0;


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
    // non - visible �� Ÿ���� �̷�� fragment ���, ���� ���� �ʴ´�
    if(IsVisible == 0.0)
        discard;




    // 1. Ambient Color, �⺻������ ������ ���� ���� ����Ѵ�

        vec3 ambient = DiffColor * diffRatio * light.ambient;


    // 2. �̰� Directional Light => ��� Fragment �� ���� �������κ��� ���� �޴´�
    
        vec3 LightVec = -light.direction;


    // 3. Diffuse Color ���� <= �����׸�Ʈ�� ������ ���� �ѷ��� �˾ƾ� �Ѵ�

        vec3 pixelNorm = normalize(WorldNormal);
        float LightAmount = max(dot(pixelNorm, LightVec), 0.0);
        vec3 diffuse = LightAmount * DiffColor * diffRatio * light.diffuse;


    // 4. Specular Color ���� <= �����׸�Ʈ���� �ݻ�� ���� ī�޶�� �󸶳� ������
    // ���� Phong Model ����� �ƴ�, Blinn-Phong ������� ������
    // ��, �����׸�Ʈ���� ������ ī�޶� ��θ� �ٶ� ��, �� �߰� ���͸� �̿�
    // �߰� ���Ϳ� ��� ���͸� �̿�

        vec3 viewDir = normalize(viewPos - WorldPosition);
        vec3 halfDir = normalize(viewDir + LightVec);

        float spec = pow( max(dot(halfDir, pixelNorm), 0.0), 75.0);
        
        vec3 specular = spec * light.specular;



    vec3 result = (ambient + diffuse + specular);
    
    float shadow = 0.0;    
    // ĳ���Ϳ� ���� ���� �ִ� Ÿ�Ͽ� ���ؼ��� �׸��ڸ� ����Ѵ�
    if(uint(curStory) == charStory)
        shadow = ShadowCalculation(LightClipPos, pixelNorm, LightVec);

    fragColor = vec4(result * (1.0 - shadow), 1.0);
}