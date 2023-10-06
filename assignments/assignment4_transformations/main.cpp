#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <tsa/shader.h>
#include <tsa/transfromations.h>
#include <ew/ewMath/vec3.h>
#include <ew/procGen.h>

void framebufferSizeCallback(GLFWwindow* window, int width, int height);

//Square aspect ratio for now. We will account for this with projection later.
const int SCREEN_WIDTH = 720;
const int SCREEN_HEIGHT = 720;

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);

	tsa::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	
	//Cube mesh
	ew::Mesh cubeMesh(ew::createCube(0.5f));

    //Makes transforms for cubes
	tsa::Transform transform1; 
	transform1.position = { -0.5, 0.5, 0.0 };
	tsa::Transform transform2;
	transform2.position = { 0.5, 0.5, 0.0 };
	tsa::Transform transform3;
	transform3.position = { 0.5, -0.5, 0.0 };
	tsa::Transform transform4;
	transform4.position = { -0.5, -0.5, 0.0 };
	const int NUM_TRANSFORMS = 4;

    //Makes array of transforms
	tsa::Transform transforms[4] = {
		transform1, transform2, transform3, transform4
	};
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.3f, 0.4f, 0.9f, 1.0f);
		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Set uniforms
		shader.use();

        //Sets model transform uniforms of all transforms in transforms array
		for (int i = 0; i < NUM_TRANSFORMS; i++)
		{
			shader.setMat4("_ModelMat", transforms[i].getModelMatrix());
			cubeMesh.draw();
		}

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

            //ImGui controls for all cube transforms
			ImGui::Begin("Transform");
			for (size_t i = 0; i < NUM_TRANSFORMS; i++) {
				ImGui::PushID(i);
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::DragFloat3("Position", &transforms[i].position.x, 0.05f);
					ImGui::DragFloat3("Rotation", &transforms[i].rotation.x, 1.0f);
					ImGui::DragFloat3("Scale", &transforms[i].scale.x, 0.05f);
				}
				ImGui::PopID();
			}
			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

