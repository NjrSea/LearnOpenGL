#include <iostream>
//#define GLEW_STATIC // ָ��ʹ�þ�̬�⣬���ǰ�dll�ŵ������ļ�����ȥ����Ҳ����
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Util.h"
#include <math.h>

//#pragma comment(lib, "glew32.lib")  // ָ��ʹ�þ�̬�⣬���ǰ�dll�ŵ������ļ�����ȥ����Ҳ����

using namespace std;

#define numVAOs 1
#define numVBOs 2

float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;

GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mLoc, vLoc, mvLoc, projLoc, tfLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, tMat, rMat;

// 36�����㣬12�������Σ�����˷�֯��ԭ�����2*2*2������
void setupVertices() {
	float vertexPositions[108] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}

string readShaderSource(const char *filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line;
	while (!fileStream.eof()) {
		getline(fileStream, line); // getline�� <string>��ĺ���
		content.append(line + "\n");
	}
	fileStream.close(); // ע������streamҪ�ص�
	return content;
}

GLuint createShaderProgram() {
	// ����д��������ģ�readShaderSource("shader/vShader.glsl")���ص�string�ڵ���.c_str()��ֵ��vshaderSourceָ��󣬾��ͷ���vshaderSourceָ����ڴ��ǷǷ���
	//const char *vshaderSource = readShaderSource("shader/vShader.glsl").c_str(); 

	string vshaderString = readShaderSource("shader/vShader.glsl");
	string fshaderString = readShaderSource("shader/fShader.glsl");
	const char *vshaderSource = vshaderString.c_str();
	const char *fshaderSource = fshaderString.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vShader, 1, &vshaderSource, NULL);
	glShaderSource(fShader, 1, &fshaderSource, NULL);
	glCompileShader(vShader);
	glCompileShader(fShader);

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);

	return vfProgram;
}

void init(GLFWwindow *window) {
	renderingProgram = Utils::createShaderProgram("shader/vShader.glsl", "shader/fShader.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 428.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
	setupVertices();
}

void display(GLFWwindow *window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);

	// ��ȡMV�����ͶӰ�����ͳһ����
	mLoc = glGetUniformLocation(renderingProgram, "m_matrix");
	vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// ����͸�Ӿ���
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degress

	// ������ͼ����ģ�;������ͼ-ģ�;���
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::mat4(1.0f);

	//tMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(0.35f*currentTime)*2.0f, cos(0.52f*currentTime)*2.0f, sin(0.7f*currentTime)*2.0f));
	
	//rMat = glm::rotate(glm::mat4(1.0f), 1.75f*(float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	//rMat = glm::rotate(rMat, 1.75f*(float)currentTime, glm::vec3(1.0f, 0.0f, 0.0f));
	//rMat = glm::rotate(rMat, 1.75f*(float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));

	//mMat = tMat * rMat;

	//mvMat = vMat * mMat;
	
	tfLoc = glGetUniformLocation(renderingProgram, "tf");

	glUniform1f(tfLoc, (float)currentTime);

	// ��͸�Ӿ����mv�����Ƹ���Ӧ��ͳһ����
	glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// ��VBO������������ɫ������Ӧ�Ķ�������
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// https://docs.gl/gl4/glEnableVertexAttribArray
	glEnableVertexAttribArray(0);

	// ����OpenGL���ã�����ģ��
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWritten = 0;
	char *log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	
	if (len > 0) {
		log = (char *)malloc(len);
		glGetShaderInfoLog(shader, len, &chWritten, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWritten = 0;
	char *log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char *)malloc(len);
		glGetProgramInfoLog(prog, len, &chWritten, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) { // ע�������õ�WHILE
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError(); 
	}
	return foundError;
}

int main(void) {
	if (!glfwInit()) { // ����init��ͬ, GLFW init
		exit(EXIT_FAILURE);
	}
	// Ҫ�������OpenGL4.3���� (major4 minor3  )
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow *window = glfwCreateWindow(600, 600, "program1", NULL, NULL); // ��һ��NULL ����ȫ����ʾ �ڶ���NULL������Դ����
	glfwMakeContextCurrent(window); // ����window֮�󲻻�ѵ�ǰ��gl�����Ĺ�����������Ҫ����glfwMakeContextCurrent
	if (glewInit() != GLEW_OK) {  // ����init��ͬ GLEW init
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);
	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime()); // ����ǰʱ�䴫��display��������֤��ͬ�ļ��������ͬ���ٶȲ��ţ�glfwGetTime���س�ʼ���󾭹���ʱ��
		glfwSwapBuffers(window); // ����˫���� �� ��ֱͬ��
		glfwPollEvents();
	}

	glfwDestroyWindow(window); 
	glfwTerminate();
	exit(EXIT_SUCCESS);
}