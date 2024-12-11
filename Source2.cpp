#include "Source2.h"

#include <windows.h>
#include <filesystem>

#include "Camera.h"
#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;
double cursorPosX = 0.0f;
double cursorPosY = 0.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
glm::vec3 direction;


std::string readFile(std::string filePath) {
    std::string text{ "" };

    // Attempt to open file
    std::ifstream file{ filePath };

    // If we couldn't open the output file stream for reading
    if (!file)
    {
        // Print an error and exit
        std::fprintf(stderr, "Uh oh, %s could not be opened for reading!\n", filePath.c_str());
        abort();
    }
    while (file)
    {
        std::string line{};
        std::getline(file, line);
        text.append(line + "\n");
    }
    file.close();
    return text;
}

int main() {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Window
    GLFWwindow* window = glfwCreateWindow(1280, 800, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 1200, 800);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Use our shader program
    Shader litShader("./vertex.glsl","./LitFragment.glsl");

    Shader lightingCubeShader("./LightVertex.glsl", "./LightFragment.glsl");

    Shader defaultShader("./DefaultVertex.glsl", "DefaultFragment.glsl");

    // rotate image upside down
    stbi_set_flip_vertically_on_load(true);

    // Load guitar backpack using Model
    std::filesystem::path p = "models/backpack.obj";
    auto pStr = absolute(p).string();
    auto pCstr = pStr.c_str();
    Model* backpack = new Model(pCstr);

    // Generate Texture buffer
    unsigned int texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("./res/container2.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    unsigned int texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load texture
    // rotate image upside down
    // stbi_set_flip_vertically_on_load(true);
    data = stbi_load("./res/container2_specular.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // Create VAO, VBO, EBO
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    glm::vec3 cubePositions[] = {
             glm::vec3(0.0f,  0.0f,  0.0f),
             glm::vec3(2.0f,  5.0f, -15.0f),
             glm::vec3(-1.5f, -2.2f, -2.5f),
             glm::vec3(-3.8f, -2.0f, -12.3f),
             glm::vec3(2.4f, -0.4f, -3.5f),
             glm::vec3(-1.7f,  3.0f, -7.5f),
             glm::vec3(1.3f, -2.0f, -2.5f),
             glm::vec3(1.5f,  2.0f, -2.5f),
             glm::vec3(1.5f,  0.2f, -1.5f),
             glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    unsigned int cubeVAO;
    glGenVertexArrays(1, &cubeVAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Bind textures to available slots
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    // set texture index
    litShader.use();
    litShader.setInt("material.diffuse", 0);
    litShader.setInt("material.specular", 1);

    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // set the vertex attribute 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    float mixValue = 0.0f;
    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // program start time
    double startTime = glfwGetTime();

    glEnable(GL_DEPTH_TEST);
    // Main Loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 lightColor{1.0f, 1.0f, 1.0f};
        //lightColor.x = sin(glfwGetTime() * 2.0f);
        //lightColor.y = sin(glfwGetTime() * 0.7f);
        //lightColor.z = sin(glfwGetTime() * 1.3f);

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f);
        glm::vec3 ambientColor = lightColor * glm::vec3(0.2f);

        litShader.use();
        litShader.setVec3("viewPos", camera.Pos);

        // directional light
        litShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        litShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
        litShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        litShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // point light 1
        litShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        litShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
        litShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
        litShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
        litShader.setFloat("pointLights[0].constant", 1.0f);
        litShader.setFloat("pointLights[0].linear", 0.09f);
        litShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        litShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        litShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
        litShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
        litShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
        litShader.setFloat("pointLights[1].constant", 1.0f);
        litShader.setFloat("pointLights[1].linear", 0.09f);
        litShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        litShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        litShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
        litShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
        litShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
        litShader.setFloat("pointLights[2].constant", 1.0f);
        litShader.setFloat("pointLights[2].linear", 0.09f);
        litShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        litShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        litShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
        litShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
        litShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
        litShader.setFloat("pointLights[3].constant", 1.0f);
        litShader.setFloat("pointLights[3].linear", 0.09f);
        litShader.setFloat("pointLights[3].quadratic", 0.032f);

        litShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        litShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
        litShader.setFloat("material.shininess", 32.0f);

        // setup coordinate systems
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(55.0f), 1280.0f / 800.0f, 0.1f, 100.0f);
        litShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        litShader.setMat4("view", view);

        glBindVertexArray(cubeVAO);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            if ((i-1) % 3 == 0)
            {
                angle += (glfwGetTime() - startTime) * 20.0f;
            }
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            litShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightingCubeShader.use();
        lightingCubeShader.setMat4("projection", projection);
        lightingCubeShader.setMat4("view", view);
        lightingCubeShader.setMat4("model", model);
        lightingCubeShader.setVec3("lightColor", lightColor);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        defaultShader.use();
        defaultShader.setMat4("projection", projection);
        defaultShader.setMat4("view", view);
        defaultShader.setMat4("model", glm::mat4(1.0f));
        backpack->Draw(defaultShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(litShader.ID);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
