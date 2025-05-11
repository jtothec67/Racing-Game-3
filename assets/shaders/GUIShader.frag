uniform sampler2D u_Texture;
varying vec2 v_TexCoord;

varying vec3 v_Normal;
varying vec3 v_FragPos;

uniform vec3 u_ViewPos;

void main()
{
	vec4 tex = texture2D(u_Texture, v_TexCoord);
	gl_FragColor = tex;
	gl_FragDepth = 0.02;
}