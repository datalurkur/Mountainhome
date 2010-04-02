uniform sampler2D tex0;
uniform sampler2D tex1;
varying vec3 normal;

void main(void)
{
    vec4 texval0 = texture2D(tex0, gl_TexCoord[0].st);
    vec4 texval1 = texture2D(tex1, gl_TexCoord[0].st);

    gl_FragColor = mix(texval1, texval0, normal[2]);
}
