#version 330 core

// vertex shader out ?ù¥?ûë ?ùºÏπòÌï¥?ïº ?ïú?ã§
in vec3 normal;
in vec2 texCoord;
in vec3 position;

// ÏµúÏ¢Ö ?ÉâÍπ?
out vec4 fragColor;


uniform bool contact;
uniform vec3 viewPos;


struct Light
{
    vec3 position;      // Í¥ëÏõê?ùò ?õî?ìú Ï¢åÌëúÍ≥? ?úÑÏπ?
    //vec3 direction;

    vec3 direction;
    vec2 cutoff;

    vec3 attenuation;
    
    vec3 ambient;       // Îπ? ?ûêÏ≤¥Ïùò ?ÉâÍπ?
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;



struct Material{
    
    // ?Öç?ä§Ï≤? Binding ?ùÑ ?Üµ?ï¥?Ñú ?Ñò?ñ¥?ò®?ã§
    sampler2D diffuse;      // ?Öç?ä§Ï≤?
    sampler2D specular;     // Î∞òÏÇ¨ ?ÉâÍπîÎèÑ, ?Öç?ä§Ï≤òÏóê?Ñú Í∞íÏùÑ ?ùΩ?ñ¥????Ñú ?†Å?ö©?ïú?ã§
    
    float shininess;
};
uniform Material material;



void main() {
    
    if(contact)
        fragColor = glm::vec4(1.0, 0.0, 0.0, 1.0);
    else
    {
        fragColor = glm::vec4(0.0, 1.0, 0.0, 1.0);
    }



    // // ?Öç?ä§Ï≤òÎ?? ?ù¥?ö©?ïú?ã§
    // vec3 texColor = texture2D(material.diffuse, texCoord).xyz;
    // // ?Öç?ä§Ï≤òÎ?? ?ù¥?ö©?ï¥?Ñú ambient Í∞íÏùÑ ?ñª?ñ¥?Ç∏?ã§
    // vec3 ambient = texColor * light.ambient;
 

    // /* 
    //     Í±∞Î¶¨?óê ?î∞Î•? Attenuation ?ùÑ Í≥ÑÏÇ∞?ïú?ã§

    //     Í¥ëÏõê?óê?Ñú ?ñ®?ñ¥Ïß? ÎßåÌÅº Í∞êÏá†
    //  */
    // float dist = length(light.position - position);
    // vec3 distPoly = vec3(1, dist, dist * dist);

    // float attenuation = 1.0f / dot(light.attenuation, distPoly);
    // vec3 lightDir = (light.position - position) / dist; // normalize


    // //vec3 lightDir = normalize(light.position - position);
    // // directional light Î°? Î≥?Í≤?
    // //vec3 lightDir = normalize(-light.direction);


    // // vec3 pixelNorm = normalize(normal);
    // // float diff = max(dot(pixelNorm, lightDir), 0.0);        // ÎπõÏùÑ Î∞õÎäî ?†ï?èÑ
    // // //vec3 diffuse = diff * material.diffuse * light.diffuse;
    // // vec3 diffuse = diff * texColor * light.diffuse;
 
    // // // ?Öç?ä§Ï≤òÏóê?Ñú Î∞òÏÇ¨ Îπ? Í∞íÏùÑ ?ùΩ?ñ¥????Ñú
    // // vec3 specColor = texture2D(material.specular, texCoord).xyz;

    // // vec3 viewDir = normalize(viewPos - position);
    // // vec3 reflectDir = reflect(-lightDir, pixelNorm);
    // // float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // // // Î∞òÏÇ¨ Îπ? Í≥ÑÏÇ∞
    // // vec3 specular = spec * specColor * light.specular;


    // // vec3 result = (ambient + diffuse + specular) * attenuation;


    // // ?îÑ?†àÍ∑∏Î®º?ä∏Í∞? ÎπõÏùÑ Î∞îÎùºÎ≥¥Îäî Î∞©Ìñ•Í≥? ÎπõÏù¥ Î∞îÎùºÎ≥¥Îäî Î∞©Ìñ• ?Ç¨?ù¥?ùò Í∞? -> ÏΩîÏÇ¨?ù∏?ùÑ Íµ¨Ìï¥
    // float theta = dot(lightDir, normalize(-light.direction));
    // vec3 result = ambient;


    // // ÎπõÏùÑ Î∞õÎäî ?ÉÅ????†Å ?úÑÏπòÎ?? Í≥ÑÏÇ∞
    // float intensity = clamp((theta - light.cutoff[1]) / (light.cutoff[0] - light.cutoff[1]), 0.0f, 1.0f);


    // // ÎπõÏùÑ Î∞õÎäî ?úÑÏπòÏóê ?ûà?ùÑ ?ïå
    // if (intensity > 0.0f) {
    //     vec3 pixelNorm = normalize(normal);
    //     float diff = max(dot(pixelNorm, lightDir), 0.0);
    //     vec3 diffuse = diff * texColor * light.diffuse;

    //     vec3 specColor = texture2D(material.specular, texCoord).xyz;
    //     vec3 viewDir = normalize(viewPos - position);
    //     vec3 reflectDir = reflect(-lightDir, pixelNorm);
    //     float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //     vec3 specular = spec * specColor * light.specular;


    //     // ?ùº?ù¥?åÖ?ùÑ Í≥ÑÏÇ∞
    //     // ÎπõÏùÑ Î∞õÎäî ?Ñ∏Í∏∞Î?? Í≥†Î†§
    //     result += (diffuse + specular) * intensity;
    // }


    // // Í∞êÏá†?äî ?òëÍ∞ôÏù¥ Í≥ÑÏÇ∞
    // result *= attenuation;
    // fragColor = vec4(result, 1.0);


    // //fragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}