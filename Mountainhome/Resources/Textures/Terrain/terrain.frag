uniform sampler2D tex0;
uniform sampler2D tex1;

void main(void)
{
    vec4 texval0 = texture2D(tex0, gl_TexCoord[0].st);
    vec4 texval1 = texture2D(tex1, gl_TexCoord[0].st);

    gl_FragColor = 0.5*(texval0 + texval1);
}
