#version 120

// The number of rows and columns in the terrainMap.
const float invTextureGridSize = 1.0 / 16.0;

// A single, gridded texture used with gridLookupIndices for texturing.
uniform sampler2D textureGrid;

// The row/column to lookup in the textureGrid when texturing.
uniform vec2 gridOffsets;

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

    // Note, we do a .yx here because offsets are specified as row, column.
    vec2 gridTexCoords = (fract(gl_TexCoord[0].st) + gridOffsets.yx) * invTextureGridSize;
    vec4 texture  = texture2D(textureGrid, gridTexCoords);

    // Set the output color.
    gl_FragColor = colorHint * texture * lighting;
}
