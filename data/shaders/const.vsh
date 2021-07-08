uniform mat4 uView;
uniform mat4 uProj;
uniform mat4 uModel;
uniform vec4 uClip;

in vec3 aPosition;
in vec3 aNormal;
in vec3 aTangent;
in vec2 aTexCoord;

out float gl_ClipDistance[1];

void main()
{
    gl_ClipDistance[0] = dot(uClip, uModel * vec4(aPosition, 1));
    gl_Position = uProj * uView * uModel * vec4(aPosition, 1);
}