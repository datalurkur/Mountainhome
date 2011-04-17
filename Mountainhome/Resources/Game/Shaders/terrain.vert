#version 120

uniform float diffuseFactor;
uniform float ambientFactor;

varying vec3 normal;
varying vec3 lightDirection;
varying vec4 ambient;
varying vec4 diffuse;

void main() {
    // Calculate the typical tex coord and position.
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;

    // Calculate lighting. For directional lights, the position is actually the direction.
    lightDirection = normalize(gl_LightSource[0].position.xyz);

    diffuse = gl_LightSource[0].diffuse;
    ambient = gl_LightSource[0].ambient + gl_LightModel.ambient;

    diffuse.rgb *= diffuseFactor;
    ambient.rgb *= ambientFactor;

    // And do the normal, finally.
    normal = normalize(gl_NormalMatrix * gl_Normal);
}
