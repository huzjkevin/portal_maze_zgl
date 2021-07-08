uniform sampler2DRect uTexColor;

out vec3 fColor;

void main()
{
    vec3 color = texture(uTexColor, gl_FragCoord.xy).rgb;

    fColor = pow(color, vec3(1 / 2.224));
}
