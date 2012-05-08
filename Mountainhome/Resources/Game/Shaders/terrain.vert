#version 120

uniform float diffuseFactor;
uniform float ambientFactor;

varying vec3 worldNormal;

varying vec3 normal;
varying vec3 lightDirection;
varying vec4 ambient;
varying vec4 diffuse;
varying vec4 position;

void main() {
    // Calculate the typical tex coord and position.
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;

    // Calculate lighting. For directional lights, the position is actually the direction.
    lightDirection = normalize(gl_LightSource[0].position.xyz);

    diffuse = gl_LightSource[0].diffuse;
    ambient = gl_LightSource[0].ambient + gl_LightModel.ambient;

    diffuse.rgb *= diffuseFactor;
    ambient.rgb *= ambientFactor;

    // And do the normal, finally.
    normal = normalize(gl_NormalMatrix * gl_Normal);
    worldNormal = gl_Normal;

    // Fog and Frustum start / end values are defined as positive values, despite OGL's
    // left handed coordinate system. Swap our fog coord to match this.
    gl_FogFragCoord = -(gl_ModelViewMatrix * gl_Vertex).z;
}
