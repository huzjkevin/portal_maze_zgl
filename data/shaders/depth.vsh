uniform float uDepth;

in vec2 aPosition;

void main()
{
    gl_Position = vec4(aPosition, uDepth, 1.0f);
}