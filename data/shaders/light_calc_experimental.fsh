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

// function from https://github.com/glslify

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

void main()
{
    vec3 position = texture(uPosition, gl_FragCoord.xy).rgb;
    vec3 normal = normalize(texture(uNormal, gl_FragCoord.xy).rgb);
    vec3 albedo = texture(uAlbedo, gl_FragCoord.xy).rgb;
    
    vec3 lightDir = normalize(uLightPos - position);
    vec3 viewDir = normalize(uCameraPos - position);
    
    float d = distance(uLightPos, position);
    float sigma = 0.7;
    
    fColor = (uLightColor * uLightIntensity / (uLightConst + uLightLin * d + uLightQuad * d * d)) * orenNayarDiffuse(lightDir, viewDir, normal, sigma, albedo);
}
