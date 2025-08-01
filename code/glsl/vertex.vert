#version 450

layout(location=0) out vec3 v_col;

vec2 a_pos[3] = vec2[](
  vec2(0.0, -0.5), 
  vec2(0.5, 0.5), 
  vec2(-0.5, 0.5)
);

vec3 a_col[3] = vec3[](
  vec3(1.0, 0.0, 0.0), 
  vec3(0.0, 1.0, 0.0), 
  vec3(0.0, 0.0, 1.0)
);

void main()
{
  gl_Position = vec4(a_pos[gl_VertexIndex], 0.0, 1.0);
  v_col = a_col[gl_VertexIndex];
}
