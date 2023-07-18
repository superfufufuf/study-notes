precision lowp float;
uniform sampler2D s_texture;
varying vec2 vTextureOut;
varying vec4 vColorOut;

void main()
{
	vec4 vColorOutTemp;
	vColorOutTemp = texture2D(s_texture,vTextureOut);
	vColorOutTemp.x += vColorOut.x;
	vColorOutTemp.y += vColorOut.y;
	vColorOutTemp.z += vColorOut.z;
	vColorOutTemp.w *= vColorOut.w;
	gl_FragColor = vColorOutTemp;
}
