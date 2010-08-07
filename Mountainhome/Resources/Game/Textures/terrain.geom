#version 120
#extension GL_EXT_geometry_shader4 : require

varying in vec3 worldPositionIn[];
varying in vec3 lightDirectionIn[];
varying in vec4 diffuseIn[];
varying in vec4 ambientIn[];

varying out vec4 ambient;
varying out vec4 diffuse;
varying out vec3 lightDirection;
varying out vec3 normal;
varying out float mix;

void main(void) {
    // Setup the per-face values.
    vec3 worldNormal = normalize(cross(
        worldPositionIn[1] - worldPositionIn[0],
        worldPositionIn[2] - worldPositionIn[1]));

    mix = worldNormal.z;

    normal = gl_NormalMatrix * worldNormal;

    // Setup the per-vertex values and emit vertices.
    for (int i = 0; i < gl_VerticesIn; i++) {
        if (abs(worldNormal.x) > 0) {
            gl_TexCoord[0].st = worldPositionIn[i].yz;
        } else if (abs(worldNormal.y) > 0) {
            gl_TexCoord[0].st = worldPositionIn[i].xz;
        } else if (abs(worldNormal.z) > 0) {
            gl_TexCoord[0].st = worldPositionIn[i].xy;
        }

        gl_Position = gl_PositionIn[i];
        lightDirection = lightDirectionIn[i];
        ambient = ambientIn[i];
        diffuse = diffuseIn[i];

        EmitVertex();
    }

    EndPrimitive();
}
