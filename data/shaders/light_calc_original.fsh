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

uniform vec3 uLightDir;

out vec3 fColor;

float orenNayarDiffuse(float albedo, vec3 normal, vec3 lightDir, vec3 viewDir);
float cookTorranceSpecular(vec3 normal, vec3 lightDir);

void main()
{
    vec3 position = texture(uPosition, gl_FragCoord.xy).rgb;
    vec3 normal = texture(uNormal, gl_FragCoord.xy).rgb;
    vec3 albedo = texture(uAlbedo, gl_FragCoord.xy).rgb;
    
    vec3 lightDir = uLightPos - position;
    vec3 viewDir = uCameraPos - position;
    float d = length(lightDir);
    float diffuse = max(orenNayarDiffuse(dot(normalize(normal), normalize(lightDir)), normalize(normal), normalize(lightDir), viewDir), 0.0);
    float specular = max(cookTorranceSpecular(normalize(normal), normalize(lightDir)), 0.0);
    
    //fColor = min(max(dot(normal, normalize(uLightPos - position)), 0.0) * albedo * uLightColor * uLightIntensity / (uLightConst + uLightLin * d + uLightQuad * d * d), 1.0);
    //fColor = (diffuse * albedo * uLightColor * uLightIntensity + vec3(0.01, 0.01, 0.01) * specular) / (uLightConst + uLightLin * d + uLightQuad * d * d);
    fColor = diffuse * albedo * uLightColor * uLightIntensity / (uLightConst + uLightLin * d + uLightQuad * d * d);
}

float orenNayarDiffuse(float albedo, vec3 normal, vec3 lightDir, vec3 viewDir){
    float sigma = 0.7; //Deviation of the normal distribution
    float pi = 3.1415926;

    //Some necessary dot products
    float VN = clamp(dot(viewDir, normal), 0.0, 1.0);
    float VL = clamp(dot(viewDir, lightDir), 0.0, 1.0);
    float NL = clamp(dot(normal, lightDir), 0.0, 1.0);

    //Parameters of the oren nayar model's formula
    float A = 1.0 - 0.5 * sigma * sigma / (sigma * sigma + 0.33);
    float B = 0.45 * sigma * sigma / (sigma * sigma + 0.09);
    float alpha = max(acos(NL), acos(VN));
    float beta = min(acos(NL), acos(VN));
    float p = 0.66; 

    float Ld = p*(A + (B*max(0.0, VL)*sin(alpha)*tan(beta))) / pi; //formular refferred at wikipedia: https://en.wikipedia.org/wiki/Oren%E2%80%93Nayar_reflectance_model

    return albedo * Ld;
}

float cookTorranceSpecular(vec3 normal, vec3 lightDir) {
    vec3 viewDir = vec3(0.0, 0.0, 1.0);
    vec3 halfVector = normalize(lightDir + viewDir);
    float sigma = 0.3;
    float pi = 3.1415926;

    //Some necessary dot products
    float VN = clamp(dot(viewDir, normal), 0.0, 1.0);
    float NL = clamp(dot(normal, lightDir), 0.0, 1.0);
    float NH = clamp(dot(normal, halfVector), 0.0, 1.0);
    float VH = clamp(dot(viewDir, halfVector), 0.0, 1.0);

    //Beckmann distribution
    float alpha = acos(NH);
    float D = (exp(-((tan(alpha) * tan(alpha)) / (sigma * sigma))))/(pi * sigma * sigma * pow(cos(alpha), 4.0));
    //Fresnel term
    float n1 = 1.0;
    float n2 = 0.8;
    float R0 = pow(((n1 - n2)/(n1 + n2)), 2.0);
    float F = R0 + (1 - R0) * pow(1 - VH, 5.0);

    //Geometry term
    float G = min(1.0, min((2.0 * NH * VN) / VH, (2.0 * NH * NL) / VH));

    float Ls = (D * F * G) / (4.0 * VN * NL); //Formula referred at Wikipedia: https://en.wikipedia.org/wiki/Specular_highlight#Cook%E2%80%93Torrance_model

    return Ls;
}
