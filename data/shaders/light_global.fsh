uniform sampler2DRect uPosition;
uniform sampler2DRect uNormal;
uniform sampler2DRect uAlbedo;

uniform vec3 uLightDir;
uniform vec3 uLightColor;

uniform float uAmbient;
uniform float uIntensity;

out vec3 fColor;

//float orenNayarDiffuse(float albedo, vec3 normal, vec3 lightDir);

void main()
{
    vec3 position = texture(uPosition, gl_FragCoord.xy).rgb;
    vec3 normal = texture(uNormal, gl_FragCoord.xy).rgb;
    vec3 albedo = texture(uAlbedo, gl_FragCoord.xy).rgb;
    
    if(length(normal) < 0.1f) {
        fColor = albedo;
    } else {
        fColor = uIntensity * albedo * uLightColor * max(uAmbient, dot(normalize(uLightDir), normal));
    }
}