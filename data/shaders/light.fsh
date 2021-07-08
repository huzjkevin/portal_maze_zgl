uniform sampler2DRect uPosition;
uniform sampler2DRect uNormal;
uniform sampler2DRect uAlbedo;

uniform vec3 uLightDir;

out vec3 fColor;

void main()
{
    vec3 position = texture(uPosition, gl_FragCoord.xy).rgb;
    vec3 normal = texture(uNormal, gl_FragCoord.xy).rgb;
    vec3 albedo = texture(uAlbedo, gl_FragCoord.xy).rgb;
    
    if(length(normal) < 0.5f) {
        fColor = albedo;
    } else {
        fColor = 0.1* albedo * max(0.5, dot(normalize(uLightDir), normal));
    }
}
