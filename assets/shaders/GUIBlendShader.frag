#version 330 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform float u_BlendFactor;

out vec4 FragColor;

void main()
{
    if (v_TexCoord.x < u_BlendFactor)
    {
        FragColor = texture(u_Texture2, v_TexCoord);
    }
    else
    {
        FragColor = texture(u_Texture1, v_TexCoord);
    }
}
