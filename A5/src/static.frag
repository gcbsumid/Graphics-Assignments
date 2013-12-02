#version 330 core

in vec3 fragvert;

float rand(vec2 n)
{
  return 0.5 + 0.5 * 
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main(void)
{
  float x = rand(fragvert.xy);
  gl_FragColor = vec4(x, x, x, 1.0);
  // gl_FragColor = vec4(1.0, 0, 0, 1.0);
}