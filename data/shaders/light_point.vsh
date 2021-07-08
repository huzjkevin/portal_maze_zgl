uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTexCoord;

void main()
{
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1);
}