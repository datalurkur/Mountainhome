uniform sampler2D glyph;

void main()
{
	gl_FragColor = gl_Color;
	gl_FragColor.a = texture2D(glyph, gl_TexCoord[0].st).a;
}

