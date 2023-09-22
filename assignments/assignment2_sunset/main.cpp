#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <tsa/shader.h>

struct Vertex {
    float x, y, z;
    float u, v;
};

//unsigned int createShader(GLenum shaderType, const char* sourceCode);
//unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource);
unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

Vertex vertices[4] = {
    //x    y    z    u    v
    { -1, -1, 0.0, 0.0, 0.0 }, //Bottom left
    { 1, -1, 0.0, 1.0, 0.0 }, //Bottom right
    { 1,  1, 0.0, 1.0, 1.0 },  //Top right
    { -1, 1, 0.0, 0.0, 1.0 }  //Top left
};

unsigned int indices[6] = {
        0, 1, 2,
        2, 3, 0
};

bool showImGUIDemoWindow = true;

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello Triangle", NULL, NULL);
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

    tsa::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
    shader.use();

	unsigned int vao = createVAO(vertices, 12, indices, 6);
    glBindVertexArray(vao);

    //Set initial colors
    float leftWaveBaseColor[3] = {0.0, 0.0, 0.8};
    float rightWaveBaseColor[3] = {0.0, 0.0, 0.9};
    float sunColorDay[3] = {1.0, 1.0, 0.4};
    float sunColorNight[3] = {1.0,0.6,0.1};
    float sunsetColorTop[3] = {1.0,0.7, 1.0};
    float sunsetColorBottom[3] = {0.9,0.5,0.1};
    float regularSkyColor[3] = {0.0,0.7,1.0};
    float nightSkyColorTop[3] = {0.0,0.0,0.0};
    float nightSkyColorBottom[3] = {0.1,0.0,0.1};

    //Set initial uniforms
    shader.setVec2("_AspectRatio", SCREEN_WIDTH, SCREEN_HEIGHT);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Set time uniform
        shader.setFloat("_Time", static_cast<float>(glfwGetTime()));
        shader.setVec3("_LeftWaveBaseColor", leftWaveBaseColor[0], leftWaveBaseColor[1], leftWaveBaseColor[2]);
        shader.setVec3("_RightWaveBaseColor", rightWaveBaseColor[0], rightWaveBaseColor[1], rightWaveBaseColor[2]);
        shader.setVec3("_SunColorDay", sunColorDay[0], sunColorDay[1], sunColorDay[2]);
        shader.setVec3("_SunColorNight", sunColorNight[0], sunColorNight[1], sunColorNight[2]);
        shader.setVec3("_SunsetColorTop", sunsetColorTop[0], sunsetColorTop[1], sunsetColorTop[2]);
        shader.setVec3("_SunsetColorBottom", sunsetColorBottom[0], sunsetColorBottom[1], sunsetColorBottom[2]);
        shader.setVec3("_RegularSkyColor", regularSkyColor[0], regularSkyColor[1], regularSkyColor[2]);
        shader.setVec3("_NightSkyTop", nightSkyColorTop[0], nightSkyColorTop[1], nightSkyColorTop[2]);
        shader.setVec3("_NightSkyBottom", nightSkyColorBottom[0], nightSkyColorBottom[1], nightSkyColorBottom[2]);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();
			ImGui::Begin("Settings");
			ImGui::Checkbox("Show Demo Window", &showImGUIDemoWindow);
            ImGui::ColorEdit3("Left Wave Color", leftWaveBaseColor);
            ImGui::ColorEdit3("Right Wave Color", rightWaveBaseColor);
            ImGui::ColorEdit3("Sun Day Color", sunColorDay);
            ImGui::ColorEdit3("Sun Night Color", sunColorNight);
            ImGui::ColorEdit3("Sunset Color Top", sunsetColorTop);
            ImGui::ColorEdit3("Sunset Color Bottom", sunsetColorBottom);
            ImGui::ColorEdit3("Day Sky Color", regularSkyColor);
            ImGui::ColorEdit3("Night Sky Color Top", nightSkyColorTop);
            ImGui::ColorEdit3("Night Sky Color Bottom", nightSkyColorBottom);


			ImGui::End();
			if (showImGUIDemoWindow) {
				ImGui::ShowDemoWindow(&showImGUIDemoWindow);
			}

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

unsigned int createVAO(Vertex* vertexData, int numVertices, unsigned int* indicesData, int numIndices) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //Allocate space for + send vertex data to GPU.
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertexData, GL_STATIC_DRAW);

	//Define a new buffer id
    unsigned int ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndices, indicesData, GL_STATIC_DRAW);

	//Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,x));
	glEnableVertexAttribArray(0);

    //UV attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex,u));
    glEnableVertexAttribArray(1);

	return vao;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

