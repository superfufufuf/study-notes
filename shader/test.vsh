attribute vec3 vPosition;
attribute vec2 vTexture;
attribute vec4 vColor;
uniform mat4 mvpMatrix;
varying   vec2 vTextureOut;
varying   vec4 vColorOut;

void  main()
{
    gl_Position = mvpMatrix * vec4(vPosition,1.0);
    vTextureOut = vTexture;
    float colorW = step(vColor.w,-0.01);
    colorW = max(colorW,vColor.w);
    vColorOut = vec4(vColor.xyz,colorW);
}