varying vec4 vTexCoord;

#ifdef VERTEX
	attribute vec3 aCoord;
	attribute vec4 aTexCoord;

	uniform mat4 uViewProj;

	void main() {
		vTexCoord   = aTexCoord;
		gl_Position = uViewProj * vec4(aCoord, 1.0);
	}
#else
	uniform sampler2D sDiffuse;

	void main() {
		gl_FragColor = texture2D(sDiffuse, vTexCoord.xy);
	}
#endif