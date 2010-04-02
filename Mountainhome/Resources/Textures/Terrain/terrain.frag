uniform sampler2D tex0;
uniform sampler2D tex1;
varying float diffuse;
varying float mix;

void main(void)
{
    vec4 grass   = texture2D(tex0, gl_TexCoord[0].st);
    vec4 dirt    = texture2D(tex1, gl_TexCoord[0].st);
    gl_FragColor = mix(dirt, grass, mix) * diffuse;
}
