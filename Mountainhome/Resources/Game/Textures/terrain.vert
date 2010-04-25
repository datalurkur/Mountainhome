varying float mix;

void main() {
    vec3 normal   = normalize( gl_NormalMatrix * gl_Normal);
    vec3 lightDir = normalize(-gl_LightSource[0].position.xyz);
    vec4 diffuse  = gl_LightSource[0].diffuse * max(0.0, dot(normal, lightDir));

    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor  = min(vec4(1), diffuse + gl_LightSource[0].ambient);
    mix            = gl_Normal.z;
}
