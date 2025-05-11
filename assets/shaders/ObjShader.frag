#version 460

uniform sampler2D u_Texture;
in vec2 v_TexCoord;

in vec3 v_Normal;
in vec3 v_FragPos;

uniform vec3 u_ViewPos;
uniform vec3 u_Ambient;
uniform float u_SpecStrength;

#define MAX_LIGHTS 10
uniform vec3 u_LightPositions[MAX_LIGHTS];
uniform vec3 u_LightColors[MAX_LIGHTS];
uniform float u_LightStrengths[MAX_LIGHTS];

out vec4 FragColor;

void main()
{
    vec4 tex = texture(u_Texture, v_TexCoord);

    vec3 N = normalize(v_Normal);
    vec3 viewDir = normalize(u_ViewPos - v_FragPos);
    vec3 lighting = u_Ambient;

    for (int i = 0; i < u_LightPositions.length(); ++i)
    {
        vec3 lightDir = normalize(u_LightPositions[i] - v_FragPos);
        float diff = max(dot(N, lightDir), 0.0);
        vec3 diffuse = u_LightColors[i] * diff * u_LightStrengths[i];

        vec3 reflectDir = reflect(-lightDir, N);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
        vec3 specular = spec * u_LightColors[i] * u_LightStrengths[i] * u_SpecStrength;

        lighting += diffuse + specular;
    }

    FragColor = vec4(lighting, 1) * tex;
}