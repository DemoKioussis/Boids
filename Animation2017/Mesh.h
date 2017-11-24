#pragma once
#include "Shader.h"
#include <vector>
#include <glm\glm.hpp>

#include "Vertex.h"
#include "Renderable.h"
#include "Component.h"
using namespace std;


class Mesh : public Component{
private:



	
	

public:

	GLuint VBO, VAO, EBO, posBuffer,velBuffer;
	int numPrims;
	Shader* shader;

	vector<GLfloat>* vertices;
	vector<GLuint>* indices;

	Mesh();
	~Mesh();

	void setVerticies(vector<GLfloat>& p, vector<GLfloat>& c, vector<GLfloat>& n);
	void setVerticies(vector<GLfloat>* v);
	void setIndices(vector<GLuint>* i);



	void setShader(Shader* s);

	void setBufferData(vector<glm::vec4> & posData,GLuint buffer, int n);
	void setBufferData(vector<glm::mat4> & data);

	//public for now
	glm::mat4 *transform;
	void printMesh();


};