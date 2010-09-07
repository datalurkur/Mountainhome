#version 120

varying vec3 lightDirection;
varying vec4 ambient;
varying vec4 diffuse;

void main() {
    // Calculate the typical tex coord and position.
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // Calculate lighting. For directional lights, the position is actually the direction.
    lightDirection = normalize(gl_LightSource[0].position.xyz);

    diffuse =
        gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
    ambient =
        gl_FrontMaterial.ambient * gl_LightSource[0].ambient +
        gl_FrontMaterial.ambient * gl_LightModel.ambient;

}
