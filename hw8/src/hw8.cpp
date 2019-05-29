#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <cmath>
#include <math.h>
#include <algorithm>
#include <iostream>

using namespace std;

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
int xpos;
int ypos;
float tn = 2;

struct Point
{
	int x;
	int y;
	Point() {

	}
	Point(int a, int b) {
		x = a;
		y = b;
	}
};

struct resPoint
{
	float x;
	float y;
	resPoint() {

	}
	resPoint(float a, float b) {
		x = a;
		y = b;
	}
};

vector<Point> points;
vector<resPoint> line;
vector<Point> temppoints;

void setPoint(vector<Point> tempuse)
{
	for (int i = 0; i < tempuse.size(); i++) {
		for (int j = tempuse[i].x - 3; j < tempuse[i].x + 4; j++) {
			for (int k = tempuse[i].y - 3; k < tempuse[i].y + 4; k++) {
				if (j >= 0 && j <= 600 && k <= 600 && k >= 0) {
					line.push_back(resPoint((float)j, (float)k));
				}
			}
		}
	}
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow* window, double x, double y)
{
	//将位置储存在全局变量中
	xpos = (int)x;
	ypos = SCR_HEIGHT - (int)y;
	return;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS) {
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
			//将tn设为大于1的值来停止动态生成
			tn = 2;
			//添加点
			points.push_back(Point(xpos, ypos));
			break;
		case GLFW_MOUSE_BUTTON_RIGHT:
			//将tn设为大于1的值来停止动态生成
			tn = 2;
			//删除点
			if (points.size() > 0) {
				points.pop_back();
			}
			break;
		default:
			return;
		}
	}
}

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

void swap(int& num1, int& num2)
{
	int temp = num1;
	num1 = num2;
	num2 = temp;
}

void getLinePoint(int x1, int y1, int x2, int y2)
{
	int temp_x = abs(x1 - x2);
	int temp_y = abs(y1 - y2);

	//斜率大于1时进行坐标变换
	bool change = (temp_x < temp_y) ? true : false;
	if (change) {
		swap(x1, y1);
		swap(x2, y2);
		swap(temp_x, temp_y);
	}

	//判断斜率是否为正
	bool pos = ((x1 - x2) * (y1 - y2) >= 0) ? true : false;

	int x = (x1 < x2) ? x1 : x2;
	int y = (x1 < x2) ? y1 : y2;
	int p = 2 * temp_y - temp_x;
	for (int i = 0; i < temp_x + 1; i++) {

		if (change) {
			line.push_back(resPoint((float)y, (float)(x + i)));
		}
		else {
			line.push_back(resPoint((float)(x + i), (float)y));
		}

		if (p > 0) {
			//斜率为正递增，为负递减
			y = pos ? y + 1 : y - 1;
			p = p + 2 * temp_y - 2 * temp_x;
		}
		else {
			p = p + 2 * temp_y;
		}
	}
}

//求阶乘
long factorial(int num) {
	if (num == 0)
		return 1;
	long temp = num;
	for (int i = 1; i < num; i++) {
		temp = temp * i;
	}
	return temp;
}

void setCurve()
{
	//points中储存的是标记的点
	float tempX;
	float tempY;
	int n = points.size() - 1;
	for (float t = 0; t <= 1; t += 0.001) {
		tempX = 0.0f;
		tempY = 0.0f;
		for (int i = 0; i <= n; i++) {
			tempX += (factorial(n) / (factorial(i) * factorial(n - i)) * pow(1 - t, n - i) * pow(t, i) * points[i].x);
			tempY += (factorial(n) / (factorial(i) * factorial(n - i)) * pow(1 - t, n - i) * pow(t, i) * points[i].y);
		}
		line.push_back(resPoint(tempX, tempY));
	}
}

void createCurve(vector<Point> temp)
{
	int n = temp.size();
	if (n <= 2)
		return;

	vector<Point> tempuse;
	for (int i = 0; i < n - 2; i++) {
		int tempX1 = (int)((1 - tn) * temp[i].x + tn * temp[i + 1].x);
		int tempY1 = (int)((1 - tn) * temp[i].y + tn * temp[i + 1].y);
		int tempX2 = (int)((1 - tn) * temp[i + 1].x + tn * temp[i + 2].x);
		int tempY2 = (int)((1 - tn) * temp[i + 1].y + tn * temp[i + 2].y);
		getLinePoint(tempX1, tempY1, tempX2, tempY2);
		tempuse.push_back(Point(tempX1, tempY1));
		if (i == n - 3) {
			tempuse.push_back(Point(tempX2, tempY2));
			int x1 = (int)((1 - tn) * tempX1 + tn * tempX2);
			int y1 = (int)((1 - tn) * tempY1 + tn * tempY2);
			vector<Point> sem;
			sem.push_back(Point(x1, y1));
			setPoint(sem);
		}
	}
	//setPoint的作用是进行将坐标点附近的点进行渲染使肉眼可以直观的看见点
	setPoint(tempuse);
	createCurve(tempuse);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float beftime = 0.0f;
	float nowtime = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		vector<resPoint> s;
		line.swap(s);

		setPoint(points);

		if (points.size() > 1) {
			for (int i = 0; i < points.size() - 1; i++) {
				getLinePoint(points[i].x, points[i].y, points[i + 1].x, points[i + 1].y);
			}
			if (points.size() > 2) {
				setCurve();
				if (tn == 2) {
					temppoints = points;
					tn = 0;
				}
			}
		}

		nowtime = glfwGetTime();
		if (nowtime - beftime > 0.001) {
			beftime = nowtime;
			tn += 0.001;
		}

		if (tn < 1) {
			createCurve(temppoints);
		}

		float *vertices = new float[line.size() * 3];
		for (int i = 0; i < line.size(); i++) {
			vertices[i * 3] = (float)(line[i].x - 300) / 300;
			vertices[i * 3 + 1] = (float)(line[i].y - 300) / 300;
			vertices[i * 3 + 2] = 0.0f;
		}

		unsigned int VBO;
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, line.size() * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, line.size());

		glfwMakeContextCurrent(window);

		glfwSwapBuffers(window);
		glfwPollEvents();

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);

		delete[]vertices;
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}