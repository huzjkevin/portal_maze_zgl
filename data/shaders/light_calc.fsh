uniform sampler2DRect uPosition;
uniform sampler2DRect uNormal;
uniform sampler2DRect uAlbedo;

uniform vec3 uCameraPos;

uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform float uLightIntensity;

uniform float uLightConst;
uniform float uLightLin;
uniform float uLightQuad;

out vec3 fColor;

// Oren Nayar diffuse lighting adapted from https://github.com/glslify/glsl-diffuse-oren-nayar by Mikola Lysenko

#define PI 3.14159265

vec3 orenNayarDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, vec3 albedo) {
    
    float LdotV = dot(lightDirection, viewDirection);   //cos(angle L to V)
    float NdotL = dot(lightDirection, surfaceNormal);   //cos(angle N to L)
    float NdotV = dot(surfaceNormal, viewDirection);    //cos(angle N to V)
    
    float s = LdotV - NdotL * NdotV;
    float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));
    
    float sigma2 = roughness * roughness;               //the roughness is a standard deviation (0 = smooth, 1 = rough)
    vec3 A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
    float B = 0.45 * sigma2 / (sigma2 + 0.09);
    
    return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}

/*vec3 cookTorranceSpecular(vec3 normal, vec3 lightDir, vec3 viewDir, vec3 specularTerm, float roughness) {
    vec3 halfVector = normalize(lightDir + viewDir);
    float sigma = roughness;
    float pi = 3.1415926;

    //Some necessary dot products
    float VN = clamp(dot(viewDir, normal), 0.0001, 1.0);
    float NL = clamp(dot(normal, lightDir), 0.0001, 1.0);
    float NH = clamp(dot(normal, halfVector), 0.0001, 1.0);
    float VH = clamp(dot(viewDir, halfVector), 0.0001, 1.0);
    float HL = clamp(dot(lightDir, halfVector), 0.0001, 1.0); //Will have problem if clamped to 0.0

    //Beckmann distribution
    float alpha = acos(NH);
    float D = (exp(-((tan(alpha) * tan(alpha)) / (sigma * sigma))))/(pi * sigma * sigma * pow(cos(alpha), 4.0));
    //Fresnel term
    float n1 = 1.0;
    float n2 = 0.8;
    float R0 = pow(((n1 - n2)/(n1 + n2)), 2.0);
    float f = R0 + (1 - R0) * pow(1 - VH, 5.0);
    vec3 F = mix(vec3(f), vec3(f), specularTerm); //This step is reffered at the cookTorrance model of glow-samples

    //Geometry term
    float G = min(1.0, min((2.0 * NH * VN) / VH, (2.0 * NH * NL) / VH));

    vec3 Ls = (D * F * G) / (4.0 * VN * NL); //Formula referred at Wikipedia: https://en.wikipedia.org/wiki/Specular_highlight#Cook%E2%80%93Torrance_model

    return Ls;
}*/

void main()
{
    vec3 position = texture(uPosition, gl_FragCoord.xy).rgb;
    vec3 normal = texture(uNormal, gl_FragCoord.xy).rgb;
    vec3 albedo = texture(uAlbedo, gl_FragCoord.xy).rgb;
    
    if(length(normal) < 0.1f) {
        fColor = vec3(0.0, 0.0, 0.0);
    } else {
        normal = normalize(normal);
        
        vec3 lightDir = normalize(uLightPos - position);
        vec3 viewDir = normalize(uCameraPos - position);
        
        float d = distance(uLightPos, position);
        float sigma = 0.7;
        
        //vec3 specular = cookTorranceSpecular(normal, lightDir, viewDir, vec3(0.1, 0.1, 0.1), 0.3);
        
        //fColor = (uLightColor * uLightIntensity / (uLightConst + uLightLin * d + uLightQuad * d * d)) * (orenNayarDiffuse(lightDir, viewDir, normal, sigma, albedo) + specular);
        fColor = (uLightColor * uLightIntensity / (uLightConst + uLightLin * d + uLightQuad * d * d)) * orenNayarDiffuse(lightDir, viewDir, normal, sigma, albedo);
    }
}
