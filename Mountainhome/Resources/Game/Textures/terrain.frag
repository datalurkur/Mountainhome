#version 120

uniform sampler2D color;
uniform sampler2D normal;

varying vec4 ambient;
varying vec4 diffuse;
varying vec3 lightDirection;

void main(void)
{
    // Get the world space normal from the normal sampler.
    //vec3 normal = texture2D(normal, gl_TexCoord[0].st);
    vec3 normal   = vec3(0, 1, 0);

    // Calculate lighting.
    vec3 eyeNormal = normalize(gl_NormalMatrix * normal);
    float NdotL    = max(dot(eyeNormal, lightDirection), 0.0);
    vec4 lighting  = ambient + diffuse * NdotL;

    // Calculate texturing.
    vec4 texture  = texture2D(color,  gl_TexCoord[0].st);

    // Set the output color.
    gl_FragColor = texture * lighting;
}
