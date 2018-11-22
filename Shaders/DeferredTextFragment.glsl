#version 150
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 gl_FragColor[2];

void main(void){
	gl_FragColor[0] = texture(diffuseTex, IN.texCoord);
	gl_FragColor[1] = vec4(0,0,0,0);
}