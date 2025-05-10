#include <iostream>
#include <glad/glad.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <Rendering/Shader.h>
#include <Rendering/VAO.h>
#include <Rendering/VBO.h>
#include <Rendering/EBO.h>
#include <Camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height);
void MouseCallback(GLFWwindow *window, double xpos, double ypos);
void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
void ProcessInput(GLFWwindow *window);

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(int, char **)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Feature Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSwapInterval(1); // Enable vsync

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader simpleShader = Shader("resources/shaders/simple.vert", "resources/shaders/simple.frag");

    float vertices[] = {
        0.5f, 0.5f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f   // top left
    };
    unsigned int indices[] = {
        // note that we start from 0!
        0, 1, 3, // first Triangle
        1, 2, 3  // second Triangle
    };

    VAO vao;
    vao.Bind();
    VBO vbo(vertices, 12 * sizeof(float));
    EBO ebo(indices, 6 * sizeof(unsigned int));
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void *)0);
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Use appropriate GLSL version

    float lastCheck = 0;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (lastCheck > 1 / 2)
        {
            std::string fps = std::to_string(1 / deltaTime);
            glfwSetWindowTitle(window, fps.c_str());
            ProcessInput(window);
            lastCheck = 0;
        }
        lastCheck += deltaTime;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        simpleShader.use();

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

        simpleShader.setVec3("viewPos", camera.Position);
        // retrieve the matrix uniform locations

        unsigned int viewLoc = glGetUniformLocation(simpleShader.ID, "view");
        // pass them to the shaders (3 different ways)
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        glUniformMatrix4fv(glGetUniformLocation(simpleShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        unsigned int modelLoc = glGetUniformLocation(simpleShader.ID, "model");
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        vao.Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Create a basic ImGui window
        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("This is a basic ImGui window.");
        ImGui::End();

        // Render ImGui
        ImGui::Render(); /*
         glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT); */
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // error catching
        GLenum err = glGetError();
        if (err != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    vao.Delete();
    vbo.Delete();
    ebo.Delete();
    return 0;
}

void FrameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow *window, double xposIn, double yposIn)
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

void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void ProcessInput(GLFWwindow *window)
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
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
        camera.SetSpeed(camera.MovementSpeed - 0.1f);
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
        camera.SetSpeed(camera.MovementSpeed + 0.1f);
    if (glfwGetKey(window, GLFW_KEY_SLASH) == GLFW_PRESS)
        camera.SetSpeed();
}