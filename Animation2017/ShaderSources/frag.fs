#version 330 core


flat in vec3 sendNorm;
flat in vec3 sendColor;

//Light
uniform vec3 lightDirection;
uniform vec3 lightDirection2; 
 
uniform vec3 lightColor;

uniform vec3 ambientLight;

out vec4 FragColor;


void main()
{
	

	vec3 _lightDirection = normalize(-lightDirection);
	float prod = max(abs( dot(normalize(sendNorm) ,_lightDirection)),0.5);

	
	vec3 result = vec3(prod,prod,prod);


	
	
	
	FragColor = vec4(sendColor* result,1);
}   


