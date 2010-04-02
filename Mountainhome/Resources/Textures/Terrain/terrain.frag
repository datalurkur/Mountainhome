uniform sampler2D tex1;
uniform sampler2D tex2;

void main(void)
{
    vec4 texval1 = texture2D(tex1, gl_TexCoord[0].st);
    vec4 texval2 = texture2D(tex2, gl_TexCoord[0].st);

    gl_FragColor = texval1;
}
