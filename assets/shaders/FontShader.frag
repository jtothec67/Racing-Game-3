#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 u_TextColour;

void main()
{    
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    if (sampled.a < 0.9) // Discard fragments with low alpha
        discard;
    color = vec4(u_TextColour, 1.0) * sampled;

    gl_FragDepth = 0.01;
}  