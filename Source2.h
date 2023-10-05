#pragma once
#include <glad/glad.h> 
#include <GLFW\glfw3.h>
#include <string>
#include "./stb_image.h"
#include "./Shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

std::string readFile(std::string fileName);

int main();
