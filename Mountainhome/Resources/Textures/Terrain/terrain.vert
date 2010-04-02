varying float diffuse;
varying float mix;

void main() {
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 lightPos = normalize(gl_LightSource[0].position.xyz);

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    diffuse = max(dot(normal, lightPos), 0.0);
    mix = normalize(gl_Normal).z;
} 
