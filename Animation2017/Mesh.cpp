#include "Mesh.h"
#include "RenderEngine.h"
#include "TimeSystem.h"
/* The mesh class will hold data about rendering an object. Ideally physical properties would be stored in a 
	rigid body class or something (think unity if you've used it). 
*/
Mesh::Mesh() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &posBuffer);
	glGenBuffers(1, &velBuffer);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	vertices = new std::vector<float>(0);
	indices = new std::vector<GLuint>(0);
	enable();


}
Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &posBuffer);
	glDeleteBuffers(1, &velBuffer);

	delete(vertices);
	delete(indices);
}



#pragma region Setters


void Mesh::setVerticies(std::vector<GLfloat>& p, std::vector<GLfloat>& c, std::vector<GLfloat>& n) {
	assert(p.size() == c.size());
	assert(p.size() == n.size());
	assert(p.size() % 3 == 0);
	std::vector<GLfloat>* v = new std::vector<GLfloat>(0);
	for (int i = 0; i < p.size();i+=3) {
		int index0 = i;
		int index1 = i + 1;
		int index2 = i + 2;

		v->push_back(p[index0]);
		v->push_back(p[index1]);
		v->push_back(p[index2]);

		v->push_back(c[index0]);
		v->push_back(c[index1]);
		v->push_back(c[index2]);

		v->push_back(n[index0]);
		v->push_back(n[index1]);
		v->push_back(n[index2]);
	}
	setVerticies(v);
}
void Mesh::setVerticies(std::vector<GLfloat>* v) {

	glBindVertexArray(VAO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	delete vertices;
	vertices = v;
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(GLfloat), &vertices->front(), GL_STATIC_DRAW);


}
void Mesh::setIndices(std::vector<GLuint>* i) {
	glBindVertexArray(VAO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(), NULL, GL_STATIC_DRAW);
	delete indices;
	indices = i;
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*indices->size(), &indices->front(), GL_STATIC_DRAW);

}
void Mesh::setShader(Shader* s) {
	shader = s;
}

#pragma endregion
void Mesh::setBufferData(vector<glm::vec4> & data, GLuint buffer, int n) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	if (numPrims < data.size()) {
		
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec4), &data[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(n);
		glVertexAttribPointer(n, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
		glVertexAttribDivisor(n, 1);

	}

	else if (data.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(glm::vec4), &data[0]);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	}

}

void Mesh::setBufferData(vector<glm::mat4> & data) {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	if (numPrims < data.size()) {
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::mat4), &data[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));


		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		numPrims = data.size();
	}
	else if (data.size() > 0) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(glm::mat4), &data[0]);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	}

}

void Mesh::printMesh() {
	std::cout << std::endl;

	std::cout << "VERTICIES: ";
	for (int i = 0; i < vertices->size();i++) {
		float x = vertices->at(i);
		std::cout << x<<" ";
	}
	std::cout << std::endl;;
	std::cout << "INDICES: ";
	for (int i = 0; i < indices->size();i++) {
		int x = indices->at(i);
		std::cout << x << " ";
	}
	std::cout << std::endl;


}
