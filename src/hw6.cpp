#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using namespace std;

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;
const char* glsl_version = "#version 130";

const char *objectVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal;  \n"
"    \n"
"    gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"}\n\0";
const char *objectFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"struct Material {\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;    \n"
"    float shininess;\n"
"}; \n"
"\n"
"struct Light {\n"
"    vec3 position;\n"
"\n"
"    vec3 ambient;\n"
"    vec3 diffuse;\n"
"    vec3 specular;\n"
"};\n"
"\n"
"in vec3 FragPos;  \n"
"in vec3 Normal;  \n"
"  \n"
"uniform vec3 viewPos;\n"
"uniform Material material;\n"
"uniform Light light;\n"
"\n"
"void main()\n"
"{\n"
"    // ambient\n"
"    vec3 ambient = light.ambient * material.ambient;\n"
"  	\n"
"    // diffuse \n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(light.position - FragPos);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
"    \n"
"    // specular\n"
"    vec3 viewDir = normalize(viewPos - FragPos);\n"
"    vec3 reflectDir = reflect(-lightDir, norm);  \n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"    vec3 specular = light.specular * (spec * material.specular);  \n"
"        \n"
"    vec3 result = ambient + diffuse + specular;\n"
"    FragColor = vec4(result, 1.0);\n"
"}\n\0 ";

const char *gourObjectVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"\n"
"out vec3 LightingColor; \n"
"\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    \n"
"    vec3 Position = vec3(model * vec4(aPos, 1.0));\n"
"    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"    \n"
"    // ambient\n"
"    float ambientStrength = 0.1;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    \n"
"    // diffuse \n"
"    vec3 norm = normalize(Normal);\n"
"    vec3 lightDir = normalize(lightPos - Position);\n"
"    float diff = max(dot(norm, lightDir), 0.0);\n"
"    vec3 diffuse = diff * lightColor;\n"
"    \n"
"    // specular\n"
"    float specularStrength = 1.0; \n"
"    vec3 viewDir = normalize(viewPos - Position);\n"
"    vec3 reflectDir = reflect(-lightDir, norm);  \n"
"    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"    vec3 specular = specularStrength * spec * lightColor;  \n"
"    LightingColor = ambient + diffuse + specular;\n"
"}\n\0";
const char *gourObjectFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"\n"
"in vec3 LightingColor; \n"
"\n"
"uniform vec3 objectColor;\n"
"\n"
"void main()\n"
"{\n"
"   FragColor = vec4(LightingColor * objectColor, 1.0);\n"
"}\n\0";


float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
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
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int objectVertexShader;
	objectVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(objectVertexShader, 1, &objectVertexShaderSource, NULL);
	glCompileShader(objectVertexShader);
	int  success;
	char infoLog[512];
	glGetShaderiv(objectVertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(objectVertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int objectFragmentShader;
	objectFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(objectFragmentShader, 1, &objectFragmentShaderSource, NULL);
	glCompileShader(objectFragmentShader);
	glGetShaderiv(objectFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(objectFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int objectShaderProgram;
	objectShaderProgram = glCreateProgram();
	glAttachShader(objectShaderProgram, objectVertexShader);
	glAttachShader(objectShaderProgram, objectFragmentShader);
	glLinkProgram(objectShaderProgram);
	glGetProgramiv(objectShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(objectShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(objectVertexShader);
	glDeleteShader(objectFragmentShader);

	unsigned int gourObjectVertexShader;
	gourObjectVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(gourObjectVertexShader, 1, &gourObjectVertexShaderSource, NULL);
	glCompileShader(gourObjectVertexShader);
	glGetShaderiv(gourObjectVertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(gourObjectVertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int gourObjectFragmentShader;
	gourObjectFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gourObjectFragmentShader, 1, &gourObjectFragmentShaderSource, NULL);
	glCompileShader(gourObjectFragmentShader);
	glGetShaderiv(gourObjectFragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(gourObjectFragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	unsigned int gourObjectShaderProgram;
	gourObjectShaderProgram = glCreateProgram();
	glAttachShader(gourObjectShaderProgram, gourObjectVertexShader);
	glAttachShader(gourObjectShaderProgram, gourObjectFragmentShader);
	glLinkProgram(gourObjectShaderProgram);
	glGetProgramiv(gourObjectShaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(gourObjectShaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(gourObjectVertexShader);
	glDeleteShader(gourObjectFragmentShader);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	float ambient = 1.0f;
	float diffuse = 1.0f;
	float specular = 1.0f;
	float shininess = 32.0f;

	int phong = 0;
	bool Rotate = false;

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		processInput(window);

		unsigned int VBO;
		unsigned int VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glEnable(GL_DEPTH_TEST);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(2.0f, 0.5f, 0.5f),
			glm::vec3(0.0f, 1.0f, 0.0f));

		x = -3.2f;
		y = 2.2f;
		z = 1.2f;

		if (Rotate) {
			float raduis = 2.0f;
			x = sin((float)glfwGetTime()) * raduis;
			y = 0;
			z = cos((float)glfwGetTime()) * raduis;
		}

		glm::vec3 lightambient = glm::vec3(0.5f);
		glm::vec3 lightdiffuse = glm::vec3(0.5f);
		glm::vec3 lightspecular = glm::vec3(0.5f);
		glm::vec3 lightPos = glm::vec3(x, y, z);
		glm::vec3 viewPos(2.0f, 0.5f, 0.5f);
		glm::vec3 materialambient(1.0f, 0.5f, 0.3f);
		glm::vec3 materialdiffuse(1.0f, 0.5f, 0.31f);
		glm::vec3 materialspecular(0.5f, 0.5f, 0.5f);

		glm::vec3 lightColor(0.5f, 0.5f, 0.5f);
		glm::vec3 objectColor(0.0f, 0.0f, 1.0f);

		if (!phong) {
			glUseProgram(objectShaderProgram);
			unsigned int modelLoc = glGetUniformLocation(objectShaderProgram, "model");
			unsigned int viewLoc = glGetUniformLocation(objectShaderProgram, "view");
			unsigned int projectionLoc = glGetUniformLocation(objectShaderProgram, "projection");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

			materialambient = materialambient * ambient;
			materialdiffuse = materialdiffuse * diffuse;
			materialspecular = materialspecular * specular;

			glUniform3fv(glGetUniformLocation(objectShaderProgram, "light.ambient"), 1, glm::value_ptr(lightambient));
			glUniform3fv(glGetUniformLocation(objectShaderProgram, "light.diffuse"), 1, &lightdiffuse[0]);
			glUniform3fv(glGetUniformLocation(objectShaderProgram, "light.specular"), 1, &lightspecular[0]);
			glUniform3fv(glGetUniformLocation(objectShaderProgram, "light.position"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(objectShaderProgram, "ViewPos"), 1, &viewPos[0]);

			glUniform3fv(glGetUniformLocation(objectShaderProgram, "material.ambient"), 1, &materialambient[0]);
			glUniform3fv(glGetUniformLocation(objectShaderProgram, "material.diffuse"), 1, &materialdiffuse[0]);
			glUniform3fv(glGetUniformLocation(objectShaderProgram, "material.specular"), 1, &materialspecular[0]);
			glUniform1f(glGetUniformLocation(objectShaderProgram, "material.shininess"), shininess);
		}
		else
		{
			glUseProgram(gourObjectShaderProgram);
			unsigned int modelLoc = glGetUniformLocation(gourObjectShaderProgram, "model");
			unsigned int viewLoc = glGetUniformLocation(gourObjectShaderProgram, "view");
			unsigned int projectionLoc = glGetUniformLocation(gourObjectShaderProgram, "projection");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

			glUniform3fv(glGetUniformLocation(gourObjectShaderProgram, "lightPos"), 1, &lightPos[0]);
			glUniform3fv(glGetUniformLocation(gourObjectShaderProgram, "viewPos"), 1, &viewPos[0]);
			glUniform3fv(glGetUniformLocation(gourObjectShaderProgram, "lightColor"), 1, &lightColor[0]);
			glUniform3fv(glGetUniformLocation(gourObjectShaderProgram, "objectColor"), 1, &objectColor[0]);
		}
		

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("HOMEWORK");
			
			ImGui::RadioButton("phong", &phong, 0);
			ImGui::RadioButton("gouraud", &phong, 1);
			ImGui::Checkbox("Rotate", &Rotate);

			if (!phong) {
				ImGui::SliderFloat("ambient", &ambient, 0, 1);
				ImGui::SliderFloat("diffuse", &diffuse, 0, 1);
				ImGui::SliderFloat("specular", &specular, 0, 1);
				ImGui::SliderFloat("shininess", &shininess, 20, 40);
			}

			ImGui::End();
		}

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);

		glfwSwapBuffers(window);


		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;
}