uniform sampler2D tex0;
uniform sampler2D tex1;
varying float mix;

void main(void)
{
    vec4 grass   = texture2D(tex0, gl_TexCoord[0].st);
    vec4 dirt    = texture2D(tex1, gl_TexCoord[0].st);
    gl_FragColor = mix(dirt, grass, pow(mix, 5.0)) * gl_Color;
}