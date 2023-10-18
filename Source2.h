#pragma once
#include <glad/glad.h> 
#include <GLFW\glfw3.h>
#include <string>
#include "./stb_image.h"
#include "./Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

std::string readFile(std::string fileName);

int main();
