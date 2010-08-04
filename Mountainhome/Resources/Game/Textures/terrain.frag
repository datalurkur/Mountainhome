#version 120

// 53/255.0 127/255.0 164/255.0
// 

uniform sampler2D tex0;
uniform sampler2D tex1;

varying float mix;
varying vec4 ambient;
varying vec4 diffuse;
varying vec3 lightDirection;
varying vec3 normal;

void main(void)
{
    // Calculate lighting.
    float NdotL = max(dot(normalize(normal), lightDirection), 0.0);
    vec4 lighting = ambient + diffuse * NdotL;

    // Calculate texturing.
    vec4 grass    = texture2D(tex0, gl_TexCoord[0].st);
    vec4 dirt     = texture2D(tex1, gl_TexCoord[0].st);
    vec4 texture  = mix(dirt, grass, mix);

    // Set the output color.
    gl_FragColor = texture * lighting;
}
