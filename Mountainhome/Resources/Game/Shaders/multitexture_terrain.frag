#version 120

// A single, gridded texture used with gridLookupIndices for texturing.
uniform sampler2D bottomTexture;
uniform sampler2D sideTexture;
uniform sampler2D topTexture;

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
#if 0
    vec4 texture;
    if      (worldNormal.z >  0.5) { texture  = texture2D(topTexture,    gl_TexCoord[0].st); }
    else if (worldNormal.z < -0.5) { texture  = texture2D(bottomTexture, gl_TexCoord[0].st); }
    else                           { texture  = texture2D(sideTexture,   gl_TexCoord[0].st); }
#else
    // Note, we do a .yx here because offsets are specified as row, column.
    vec4 texBottom = texture2D(bottomTexture, gl_TexCoord[0].st);
    vec4 texSide   = texture2D(sideTexture,   gl_TexCoord[0].st);
    vec4 texTop    = texture2D(topTexture,    gl_TexCoord[0].st);
    vec4 texture   = mix(texBottom, mix(texSide, texTop, step(0.5, worldNormal.z)), step(-0.5, worldNormal.z));
#endif

    // Calculate fog.
    float fogFactor = (gl_Fog.end - gl_FogFragCoord) * gl_Fog.scale * gl_Fog.density;
    fogFactor = exp(-(fogFactor * fogFactor));
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    // Set the output color.
    gl_FragColor = mix(texture * lighting, gl_Fog.color, fogFactor);

    // XXXBMW: FIXME - This is a BIG perf hit. Do this correctly!!
    if (colorHint != vec4(1, 1, 1, 1)) {
        gl_FragColor = colorHint * mix(gl_FragColor, vec4(1.0), 0.5);
    }
}
