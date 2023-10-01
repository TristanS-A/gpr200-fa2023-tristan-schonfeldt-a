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
    unsigned int bgText2 = tsa::loadTexture("assets/duck.png", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    unsigned int characterText = tsa::loadTexture("assets/duck.png", GL_REPEAT, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST);
    unsigned int noiseText = tsa::loadTexture("assets/noiseTexture.png", GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    //Sets initial uniforms
    characterShader.use();
    characterShader.setVec2("_AspectRatio", SCREEN_WIDTH, SCREEN_HEIGHT);

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
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float xPos = 0;
    float yPos = 0;
    float vel = 0.01;
    float rotSpeed = 0.05;
    float rotAngle = 0;
    float charSizeX = 500;
    float charSizeY = 500;


    float currTime = static_cast<float>(glfwGetTime());
    float prevTime = static_cast<float>(glfwGetTime());
    float idleTime = 0;

    float tileSize = 1;
    const char* ButtonNames[] = {"GL_REPEAT", "GL_CLAMP_TO_EDGE", "GL_CLAMP_TO_BORDER"};
    int wrapTypes[3] = {GL_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER};
    int wrapType = 0;
    float CharAlpha = 1;
    float distortionSpeed = 1;
    float distortionPower = 1;
    float distortionColor[4] = {0.0, 0.0, 0.0, 1.0};

    const float FPS = 1 / 60.0;

    while (!glfwWindowShouldClose(window)) {
        currTime = static_cast<float>(glfwGetTime());
        if (currTime > prevTime + FPS) {
            prevTime = currTime;

            glfwPollEvents();
            glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            //Set uniforms
            bgShader.use();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bgText);
            bgShader.setInt("_BGText", 0);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapTypes[wrapType]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTypes[wrapType]);

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, noiseText);
            bgShader.setInt("_NoiseStuff", 1);


            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, bgText2);
            bgShader.setInt("_BG2Text", 2);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapTypes[wrapType]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapTypes[wrapType]);
            
            bgShader.setFloat("_Time", currTime);
            bgShader.setFloat("_TileSize", tileSize);
            bgShader.setFloat("_DistortionSpeed", distortionSpeed);
            bgShader.setFloat("_DistortionPower", distortionPower);
            bgShader.setVec4("_DistortionColor", distortionColor[0], distortionColor[1], distortionColor[2], distortionColor[3]);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

            characterShader.use();
            
            //Alternative way of handling placement of character texture (I don't know if this is better)
            //
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, characterText);

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, characterText);
            characterShader.setInt("_CharacterText", 3);
            characterShader.setFloat("_CharAlpha", CharAlpha);

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                idleTime = currTime - 2/ew::PI;
                xPos += vel * sin(rotAngle);
                yPos += vel * cos(rotAngle);

                if (xPos > 1){
                    xPos = 1;
                }
                else if (xPos < -1){
                    xPos = -1;
                }

                if (yPos > 1){
                    yPos = 1;
                }
                else if (yPos < -1){
                    yPos = -1;
                }

                characterShader.setFloat("_YPos", yPos);
                characterShader.setFloat("_XPos", xPos);
            }
            else {
                xPos = sin((currTime - idleTime) * 2.5) * 0.005 + xPos;
                yPos = cos((currTime - idleTime) * 5) * 0.005 + yPos;
                characterShader.setFloat("_XPos", xPos);
                characterShader.setFloat("_YPos", yPos);
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
                rotAngle -= rotSpeed;
                characterShader.setFloat("_RotAngle", rotAngle);
            }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
                rotAngle += rotSpeed;
                characterShader.setFloat("_RotAngle", rotAngle);
            }

            characterShader.setVec2("_SpriteXY", charSizeX, charSizeY);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);

            //Render UI
            {
                ImGui_ImplGlfw_NewFrame();
                ImGui_ImplOpenGL3_NewFrame();
                ImGui::NewFrame();

                ImGui::Begin("Settings");
                ImGui::DragFloat("Tiles", &tileSize, 0.5, 0, 100);
                ImGui::Combo("Wrap Type", &wrapType, ButtonNames, 3);
                ImGui::DragFloat("Character Alpha", &CharAlpha, 0.01, 0, 1);
                ImGui::ColorEdit4("Distortion Color", distortionColor);
                ImGui::DragFloat("Distortion Speed", &distortionSpeed, 0.1);
                ImGui::DragFloat("Distortion Power", &distortionPower, 0.01, 0, 5);
                ImGui::DragFloat("Character Rotation Speed", &rotSpeed, 0.01, 0, 1);
                ImGui::DragFloat("Character Movement Speed", &vel, 0.01, 0, 1);
                ImGui::DragFloat("Character Size X", &charSizeX, 1, 0, SCREEN_WIDTH * 2);
                ImGui::DragFloat("Character Size Y", &charSizeY, 1, 0, SCREEN_HEIGHT * 2);
                ImGui::End();

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            }

            glfwSwapBuffers(window);
        }
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

