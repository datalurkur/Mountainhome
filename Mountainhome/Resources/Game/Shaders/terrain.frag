#version 120

// A single, gridded texture used with gridLookupIndices for texturing.
uniform sampler2D texture;

// A color value multiplied on top of lighting and texture lookups. Used for tinting.
uniform vec4 colorHint;

// Used to determine if we're texturing the top of a tile.
varying vec3 worldNormal;

// Lighting details. All lighting is handled directionally.
varying vec3 normal;
varying vec4 ambient;
varying vec4 diffuse;
varying vec3 lightDirection;

void main(void)
{
    // Calculate lighting.
    vec3 eyeNormal = normalize(normal);
    float NdotL    = max(dot(eyeNormal, lightDirection), 0.0);
    vec4 lighting  = ambient + diffuse * NdotL;

    // Calculate texturing.
    vec4 texture  = texture2D(texture, gl_TexCoord[0].st);

    // Set the output color.
    gl_FragColor = colorHint * texture * lighting;
}
