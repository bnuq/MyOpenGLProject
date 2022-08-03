#version 460 core

in vec4 LightClipPos;
in vec3 TileColor;
in float curStory;


// ���ϵǴ� ���� �����׸�Ʈ ����
out vec4 fragColor;


// 0 -> 1 �� �ٲ�鼭 ���´�
uniform float TimeRatio;
uniform sampler2D shadowMap;        // �Ȱ��� ���� ���ؿ��� �׸� shadow map �� �޴´�
uniform uint charStory;



float ShadowCalculation(vec4 fragPosLight)
{
    // �� Ŭ�� ���� NDC
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;

    // NDC ��ǥ�� [0,1] �� ��ȯ
    projCoords = projCoords * 0.5 + 0.5;

    // ��ȯ�� ��ǥ�� [0, 1] ������ ����ٸ�, �׸��ڴ� ����
    if(projCoords.x < 0 || projCoords.x > 1 || projCoords.y < 0 || projCoords.y > 1)
        return 0;



    // shadow map ���� ���� �� �о����
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // ���� fragment �� ���� ��
    float currentDepth = projCoords.z;

    // �ð��� �������� bias �� Ŀ���鼭 => �׸��ڰ� �۾����� ����
    float bias = 0.02 * (0.1 + TimeRatio);
    
    if((currentDepth - bias) > closestDepth)
        return 1;
    else
        return 0;
}


void main()
{
    float shadow;
    
    // �ո鿡 �ش��ϴ� fragment �� ���ؼ��� �׸��ڸ� ����Ѵ�
    // ĳ���Ϳ� ���� ������ �־�� �׸��ڸ� ����Ѵ�
    if(gl_FrontFacing && uint(curStory) == charStory)
        shadow = ShadowCalculation(LightClipPos);
    else
        shadow = 0.0;


    // ������� ���� �������� ���ڱ� ������� �ϰ� �ʹ� => �⺻ ���İ��� �༭, �ִ� ������ �������Ѱ� ���̵���
    fragColor = vec4(TileColor * (1 - shadow), 0.5 - TimeRatio * 0.5 + 0.2);
}