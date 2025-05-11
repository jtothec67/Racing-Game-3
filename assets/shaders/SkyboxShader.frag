#version 330 core
in vec3 dir;
out vec4 color;
uniform samplerCube uTexEnv;
void main()
{
    color=texture(uTexEnv, dir);
}