#version 120

varying vec3 worldPositionIn;
varying vec3 lightDirectionIn;
varying vec4 ambientIn;
varying vec4 diffuseIn;

void main() {
    gl_TexCoord[0]   = gl_MultiTexCoord0;
    gl_Position      = gl_ModelViewProjectionMatrix * gl_Vertex;
    worldPositionIn  = gl_Vertex.xyz;

    // Calculate lighting.
    // For directional lights, the position is actually the direction.
    lightDirectionIn = normalize(gl_LightSource[0].position.xyz * -1);
    // lightDirectionIn = normalize((gl_ModelViewProjectionMatrix * vec4(1, 0, 0, 1)).xyz);
    // lightDirectionIn = normalize(vec3(1, 1, 1));

    diffuseIn =
        gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambientIn =
        gl_FrontMaterial.ambient * gl_LightSource[0].ambient +
        gl_FrontMaterial.ambient * gl_LightModel.ambient;

}
