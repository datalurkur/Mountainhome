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
    vec4 lighting  = ambient + ((diffuse * NdotL) * (vec4(vec3((worldNormal.z * 0.25) + 0.75), 1)));

    // Calculate texturing.
    vec4 texture  = texture2D(texture, gl_TexCoord[0].st);

    // Set the output color.
    gl_FragColor = texture * lighting;

    // If we're doing a color hint, convert to greyscale so we get a clearly visible color.
    // XXXBMW: FIXME - This is a BIG perf hit. Do this correctly!!
    if (colorHint != vec4(1, 1, 1, 1)) {
        gl_FragColor = colorHint * mix(gl_FragColor, vec4(1.0), 0.5);
    }
}
