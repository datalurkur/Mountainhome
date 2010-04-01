uniform sampler2D tex1;
uniform sampler2D tex2;

void main(void)
{
    vec4 texval1 = texture2D(tex1, vec2(gl_TexCoord[0]));
    vec4 texval2 = texture2D(tex2, vec2(gl_TexCoord[1]));

    gl_FragColor = 0.5*(texval1 + texval2);
}
