#version 330 core

in vec3 FragPos;
in vec3 Normal;

uniform mat4 view;
uniform vec3 outlineColor;
uniform float outlineWidth;

out vec4 FragColor;

void main()
{
    // Extract the camera position from the view matrix
    vec3 viewPos = vec3(inverse(view)[3]);

    // Calculate the view direction
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Calculate the dot product of the view direction and the normal
    float intensity = dot(viewDir, normalize(Normal));
    
    // If the intensity is less than a threshold, draw the outline
    if (intensity < outlineWidth)
    {
        FragColor = vec4(outlineColor, 1.0);
    }
    else
    {
        discard;
    }
}