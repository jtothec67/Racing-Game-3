#version 330 core

in vec2 v_TexCoord;

uniform sampler2D u_Texture1;
uniform sampler2D u_Texture2;
uniform float u_BlendFactor;

out vec4 FragColor;

void main()
{
    vec4 color1 = texture(u_Texture1, v_TexCoord);
    vec4 color2 = texture(u_Texture2, v_TexCoord);
    FragColor = mix(color1, color2, u_BlendFactor);
}
