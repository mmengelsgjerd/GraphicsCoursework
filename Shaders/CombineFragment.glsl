#version 150 core
uniform sampler2D diffuseTex;
uniform sampler2D emissiveTex;
uniform sampler2D specularTex;
uniform sampler2D depthTex;
uniform sampler2D normalTex;

in Vertex {
	vec2 texCoord ;
} IN ;

out vec4 fragColour ;

void main ( void ) {
	vec3 diffuse = texture ( diffuseTex , IN . texCoord ). xyz ;
	vec3 light = texture ( emissiveTex , IN . texCoord ). xyz ;
	vec3 specular = texture ( specularTex , IN . texCoord ). xyz ;
	vec3 depth = texture (depthTex, IN.texCoord).xyz;
	vec4 normal = texture (normalTex, IN.texCoord);
	
	//float ambient = 0.2;
	
	if (normal.w == 0)
	{
		fragColour . xyz = diffuse;
		fragColour . a = 1.0;
	}
	else
	{		
		fragColour . xyz = diffuse * 0.2; // ambient
		fragColour . xyz += diffuse * light ; // lambert
		fragColour . xyz += specular ; // Specular
		fragColour . a = 1.0;
	}
}