#version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 norm;
layout (location = 3) in vec4 boidPos;
layout (location = 4) in vec4 boidVel;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

flat out vec3 sendColor;
flat out vec3 sendNorm;

vec3 up = vec3(0.0f,1.0f,0.0f);

// based off of  https://stackoverflow.com/questions/12486484/calcuating-lookat-matrix-using-in-vertex-shader-opengl-3-x
mat4 lookAt(vec3 eye, vec3 center){
	
	
	mat4 Matrix;

    vec3 X,Y, Z;
	
	Z = eye - center;
    Z = normalize(Z);
    X = cross( up,Z );
	Y = normalize(cross(Z, X ));
	X = normalize(X);
    Matrix[0][0] = X.x;
    Matrix[1][0] = X.y;
    Matrix[2][0] = X.z;
    Matrix[3][0] = dot(-X,eye );
    Matrix[0][1] = Y.x;
    Matrix[1][1] = Y.y;
    Matrix[2][1] = Y.z;
    Matrix[3][1] =  dot(-X,eye );
    Matrix[0][2] = Z.x;
    Matrix[1][2] = Z.y;
    Matrix[2][2] = Z.z;
    Matrix[3][2] = dot(-Z, eye );
    Matrix[0][3] = 0;
    Matrix[1][3] = 0;
    Matrix[2][3] = 0;
    Matrix[3][3] = 1.0f;

    return Matrix;
}
void main()
{
	mat4 translation;
	translation[0][0]		= 1;
	translation[1][1]		= 1;
	translation[2][2]		= 1;
	translation[3][3]		= 1;
	
	translation[3][0]		= boidPos.x;
	translation[3][1]		= boidPos.y;
	translation[3][2]		= boidPos.z;


	mat4 rotation = lookAt(vec3(0,0,0),-boidVel.xyz);
	
	mat4 transform = translation*inverse(rotation);
	gl_Position = projection*view * transform* vec4(vertex, 1.0f);

	sendColor = color;
	sendNorm= (rotation*vec4(norm,0)).xyz;

}