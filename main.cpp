#include <iostream>
//#define GLEW_STATIC // ָ��ʹ�þ�̬�⣬���ǰ�dll�ŵ������ļ�����ȥ����Ҳ����
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

//#pragma comment(lib, "glew32.lib")  // ָ��ʹ�þ�̬�⣬���ǰ�dll�ŵ������ļ�����ȥ����Ҳ����

using namespace std;

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

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
	renderingProgram = createShaderProgram();
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);
}

void display(GLFWwindow *window, double currenTime) {
	glUseProgram(renderingProgram);
	glPointSize(30.0f);
	glDrawArrays(GL_POINTS, 0, 1);
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