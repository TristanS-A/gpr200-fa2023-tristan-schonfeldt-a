#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <ew/shader.h>
#include <ew/texture.h>
#include <ew/procGen.h>
#include <ew/transform.h>
#include <ew/camera.h>
#include <ew/cameraController.h>

#define MAX_LIGHTS 2

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;
ew::Vec3 bgColor = ew::Vec3(0.1f);

ew::Camera camera;
ew::CameraController cameraController;

struct Light {
    ew::Vec3 position;
    ew::Vec3 color;
};

struct Material {
    float ambientK;
    float diffuseK;
    float shininess;
    float specular;
};

int main() {
	printf("Initializing...");
	if (!glfwInit()) {
		printf("GLFW failed to init!");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Camera", NULL, NULL);
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

	//Global settings
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	ew::Shader shader("assets/defaultLit.vert", "assets/defaultLit.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

    ew::Shader lightShader("assets/unlit.vert", "assets/unlit.frag");

	//Create cube
	ew::Mesh cubeMesh(ew::createCube(1.0f));
	ew::Mesh planeMesh(ew::createPlane(5.0f, 5.0f, 10));
	ew::Mesh sphereMesh(ew::createSphere(0.5f, 64));
	ew::Mesh cylinderMesh(ew::createCylinder(0.5f, 1.0f, 32));

	//Initialize transforms
	ew::Transform cubeTransform;
	ew::Transform planeTransform;
	ew::Transform sphereTransform;
	ew::Transform cylinderTransform;
	planeTransform.position = ew::Vec3(0, -1.0, 0);
	sphereTransform.position = ew::Vec3(-1.5f, 0.0f, 0.0f);
	cylinderTransform.position = ew::Vec3(1.5f, 0.0f, 0.0f);

	resetCamera(camera,cameraController);

    //Create materials for meshes
    Material cubeMat = {0, 1, 9, 1};
    Material planeMat = {0, 1, 9, 1};
    Material sphereMat = {0, 1, 9, 1};
    Material cylinderMat = {0, 1, 9, 1};

    //Create lights
    Light lights[MAX_LIGHTS] = {
            Light {ew::Vec3(0.0, 0.0, 0.0), ew::Vec3(1.0, 0.0, 0.0)},
            Light {ew::Vec3(0.0, 0.0, 0.0), ew::Vec3(0.0, 0.0, 1.0)}
    };
    ew::Mesh sphereLightMesh1(ew::createSphere(0.2f, 64));
    ew::Transform sphereLightTransform1;
    sphereLightTransform1.position = ew::Vec3(1.0f, 1.0f, 1.0f);

    ew::Mesh sphereLightMesh2(ew::createSphere(0.2f, 64));
    ew::Transform sphereLightTransform2;
    sphereLightTransform2.position = ew::Vec3(-1.0f, 1.0f, 1.0f);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		//Update camera
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
		cameraController.Move(window, &camera, deltaTime);

		//RENDER
		glClearColor(bgColor.x, bgColor.y,bgColor.z,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());
        shader.setVec3("_CamPos", camera.position);

		//Draw shapes
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		//shader.setMat4("_NewModelMatrix", ew::macubeTransform.getModelMatrix());
        shader.setFloat("_Mat.ambientK", cubeMat.ambientK);
        shader.setFloat("_Mat.diffuseK", cubeMat.diffuseK);
        shader.setFloat("_Mat.shininess", cubeMat.shininess);
        shader.setFloat("_Mat.specular", cubeMat.specular);
		cubeMesh.draw();

		shader.setMat4("_Model", planeTransform.getModelMatrix());
        shader.setFloat("_Mat.ambientK", planeMat.ambientK);
        shader.setFloat("_Mat.diffuseK", planeMat.diffuseK);
        shader.setFloat("_Mat.shininess", planeMat.shininess);
        shader.setFloat("_Mat.specular", planeMat.specular);
		planeMesh.draw();

		shader.setMat4("_Model", sphereTransform.getModelMatrix());
        shader.setFloat("_Mat.ambientK", sphereMat.ambientK);
        shader.setFloat("_Mat.diffuseK", sphereMat.diffuseK);
        shader.setFloat("_Mat.shininess", sphereMat.shininess);
        shader.setFloat("_Mat.specular", sphereMat.specular);
		sphereMesh.draw();

		shader.setMat4("_Model", cylinderTransform.getModelMatrix());
        shader.setFloat("_Mat.ambientK", cylinderMat.ambientK);
        shader.setFloat("_Mat.diffuseK", cylinderMat.diffuseK);
        shader.setFloat("_Mat.shininess", cylinderMat.shininess);
        shader.setFloat("_Mat.specular", cylinderMat.specular);
		cylinderMesh.draw();

        //Drawing Lights
        //ew::Vec4 lightWorldPos = sphereLightTransform.getModelMatrix() * ew::Vec4(lights[0].position, 1.0);
        //lights[0].position = ew::Vec3(lightWorldPos.x, lightWorldPos.y, lightWorldPos.z);
        lights[0].position = sphereLightTransform1.position;
        lights[1].position = sphereLightTransform2.position;
        for (int i = 0; i < MAX_LIGHTS; i++) {
            shader.setVec3("_Lights[" + std::to_string(i) + "].position", lights[i].position);
            shader.setVec3("_Lights[" + std::to_string(i) + "].color", lights[i].color);
        }

        lightShader.use();
        lightShader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

        lightShader.setMat4("_Model", sphereLightTransform1.getModelMatrix());
        lightShader.setVec3("_Color", lights[0].color);
        sphereLightMesh1.draw();

        lightShader.setMat4("_Model", sphereLightTransform2.getModelMatrix());
        lightShader.setVec3("_Color", lights[1].color);
        sphereLightMesh2.draw();

		//TODO: Render point lights

		//Render UI
		{
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Settings");
			if (ImGui::CollapsingHeader("Camera")) {
				ImGui::DragFloat3("Position", &camera.position.x, 0.1f);
				ImGui::DragFloat3("Target", &camera.target.x, 0.1f);
				ImGui::Checkbox("Orthographic", &camera.orthographic);
				if (camera.orthographic) {
					ImGui::DragFloat("Ortho Height", &camera.orthoHeight, 0.1f);
				}
				else {
					ImGui::SliderFloat("FOV", &camera.fov, 0.0f, 180.0f);
				}
				ImGui::DragFloat("Near Plane", &camera.nearPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Far Plane", &camera.farPlane, 0.1f, 0.0f);
				ImGui::DragFloat("Move Speed", &cameraController.moveSpeed, 0.1f);
				ImGui::DragFloat("Sprint Speed", &cameraController.sprintMoveSpeed, 0.1f);
				if (ImGui::Button("Reset")) {
					resetCamera(camera, cameraController);
				}
			}

			ImGui::ColorEdit3("BG color", &bgColor.x);
            ImGui::DragFloat3("Light Position", &sphereLightTransform1.position.x, 0.1f);
            ImGui::DragFloat3("Sphere Position", &sphereTransform.position.x, 0.1f);
            ImGui::DragFloat3("Sphere Rotation", &sphereTransform.rotation.x, 0.1f);
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
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 5);
	camera.target = ew::Vec3(0);
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


