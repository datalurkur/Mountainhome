varying float diffuse;
varying float mix;

void main() {
    vec3 normal   = normalize( gl_NormalMatrix * gl_Normal);
    vec3 lightDir = normalize(-gl_LightSource[0].position.xyz);

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    diffuse = max(0.0, dot(normal, lightDir));
    mix = gl_Normal.z;
}
