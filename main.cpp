#include <iostream>
//#define GLEW_STATIC // 指定使用静态库，但是把dll放到产物文件夹里去掉了也能用
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>

//#pragma comment(lib, "glew32.lib")  // 指定使用静态库，但是把dll放到产物文件夹里去掉了也能用

using namespace std;

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

string readShaderSource(const char *filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line;
	while (!fileStream.eof()) {
		getline(fileStream, line); // getline是 <string>里的函数
		content.append(line + "\n");
	}
	fileStream.close(); // 注意用完stream要关掉
	return content;
}

GLuint createShaderProgram() {
	// 这样写是有问题的，readShaderSource("shader/vShader.glsl")返回的string在调用.c_str()赋值给vshaderSource指针后，就释放了vshaderSource指向的内存是非法的
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
	while (glErr != GL_NO_ERROR) { // 注意这里用的WHILE
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError(); 
	}
	return foundError;
}

int main(void) {
	if (!glfwInit()) { // 两次init不同, GLFW init
		exit(EXIT_FAILURE);
	}
	// 要求机器与OpenGL4.3兼容 (major4 minor3  )
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow *window = glfwCreateWindow(600, 600, "program1", NULL, NULL); // 第一个NULL 用于全屏显示 第二个NULL用于资源共享
	glfwMakeContextCurrent(window); // 创建window之后不会把当前的gl上下文关联起来，需要调用glfwMakeContextCurrent
	if (glewInit() != GLEW_OK) {  // 两次init不同 GLEW init
		exit(EXIT_FAILURE);
	}
	glfwSwapInterval(1);
	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, glfwGetTime()); // 将当前时间传给display函数，保证不同的计算机以相同的速度播放，glfwGetTime返回初始化后经过的时间
		glfwSwapBuffers(window); // 开启双缓冲 和 垂直同步
		glfwPollEvents();
	}

	glfwDestroyWindow(window); 
	glfwTerminate();
	exit(EXIT_SUCCESS);
}