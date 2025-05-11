#version 330 core
out vec3 dir;
uniform mat4 u_InvPV;
void main()
{
    vec2 pos  = vec2( (gl_VertexID & 2)>>1, 1 - (gl_VertexID & 1)) * 2.0 - 1.0;
    vec4 front= u_InvPV * vec4(pos, -1.0, 1.0);
    vec4 back = u_InvPV * vec4(pos,  1.0, 1.0);

    dir=back.xyz / back.w - front.xyz / front.w;
    gl_Position = vec4(pos,1.0,1.0);
}