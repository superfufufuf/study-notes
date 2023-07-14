precision lowp float;
uniform sampler2D s_texture;
varying vec2 vTextureOut;
varying vec4 vColorOut;

float random (in vec2 st)
{
	return fract(sin(dot(st.xy, vec2(12.9898,78.233))) * 43758.5453123);
}

float noise (in vec2 st)
{
    vec2 i = floor(st);
	vec2 f = fract(st);
	float a = random(i);
	float b = random(i + vec2(1.0, 0.0));
	float c = random(i + vec2(0.0, 1.0));
	float d = random(i + vec2(1.0, 1.0));
	vec2 u = f * f * ( 3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

void main()
{
	vec2 noiseSt = v_texCoord * 100.0;	// 将UV扩大100倍
	vec3 noiseColor = noise(noiseSt);	// 生成2维噪声
	gl_FragColor = vec4(noiseColor, 1.0);
}
