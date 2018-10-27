varying vec3 vNormal;
varying vec4 vTexCoord;

#ifdef VERTEX
	attribute vec3 aCoord;
	attribute vec3 aNormal;
	attribute vec4 aTexCoord;

	uniform mat4 uViewProj;
	uniform mat4 uModel;

	void main() {
		vTexCoord   = aTexCoord;
		vNormal     = aNormal;
		gl_Position = uViewProj * (uModel * vec4(aCoord, 1.0));
	}
#else
	uniform sampler2D sDiffuse;
	uniform sampler2D sLightmap;

	void main() {
		gl_FragColor = texture2D(sDiffuse, vTexCoord.xy) * texture2D(sLightmap, vTexCoord.zw) * 1.5;
	}
#endif