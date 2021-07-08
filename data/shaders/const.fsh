uniform vec3 uColor;

out vec3 fPosition;
out vec3 fNormal;
out vec3 fAlbedo;

void main()
{
    fPosition = vec3(0.0f, 0.0f, 0.0f);
    fNormal = vec3(0.0f, 0.0f, 0.0f);
    fAlbedo = uColor;
}
