#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <tsa/texture.h>

struct Vertex {
    float x, y, z;
    float u, v;
};

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

Vertex vertices[4] = {
        {-1.0, -1.0, 0.0, 0.0, 0.0},
        {1.0, -1.0, 0.0, 1.0, 0.0},
        {1.0, 1.0, 0.0, 1.0, 1.0},
        {-1.0, 1.0, 0.0, 0.0, 1.0}
};
unsigned short indices[6] = {
        0, 1, 2,
        2, 3, 0
};

int main() {
    printf("Initializing...");
    if (!glfwInit()) {
        printf("GLFW failed to init!");
        return 1;
    }

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Textures", NULL, NULL);
    if (window == NULL) {
        printf("GLFW failed to create window");
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGL(glfwGetProcAddress)) {
        printf("GLAD Failed to load GL headers");
        return 1;
    }

    //Initialize ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ew::Shader bgShader("assets/bgVertShader.vert", "assets/bgFragShader.frag");
    ew::Shader characterShader("assets/characterVertShader.vert", "assets/characterFragShader.frag");

    unsigned int quadVAO = createVAO(vertices, 4, indices, 6);

    glBindVertexArray(quadVAO);

    unsigned int bgText = tsa::loadTexture("assets/spook.jpg", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int characterText = tsa::loadTexture("assets/duck.png", GL_REPEAT, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST);
    unsigned int noiseText = tsa::loadTexture("assets/noiseTexture.png", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    ////////////Another way to display multiple textures
    /*
     ///////////Outside render loop:

     glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgText);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, characterText);

     ///////////Inside render loop:

    //Set uniforms
    bgShader.use();

    bgShader.setInt("_BGText", 0);
    characterShader.setInt("_CharacterText", 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

    characterShader.use();
    characterShader.setInt("_CharacterText", 0);
    bgShader.setInt("_BGText", 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);*/

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Set uniforms
        bgShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgText);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, noiseText);
        bgShader.setInt("_NoiseStuff", 1);
        bgShader.setFloat("_Time", static_cast<float>(glfwGetTime()));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

        characterShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, characterText);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

        //Render UI
        {
            ImGui_ImplGlfw_NewFrame();
            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Settings");
            ImGui::End();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        glfwSwapBuffers(window);
    }
    printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned short* indicesData, int numIndices) {
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //Vertex Buffer Object
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertexData, GL_STATIC_DRAW);

    //Element Buffer Object
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * numIndices, indicesData, GL_STATIC_DRAW);

    //Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, x));
    glEnableVertexAttribArray(0);

    //UV attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(offsetof(Vertex, u)));
    glEnableVertexAttribArray(1);

    return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

