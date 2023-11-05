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
#include "tsa/procGen.h"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void resetCamera(ew::Camera& camera, ew::CameraController& cameraController);

int SCREEN_WIDTH = 1080;
int SCREEN_HEIGHT = 720;

float prevTime;

struct AppSettings {
	const char* shadingModeNames[6] = { "Solid Color","Normals","UVs","Texture","Lit","Texture Lit"};
	int shadingModeIndex;

	ew::Vec3 bgColor = ew::Vec3(0.1f);
	ew::Vec3 shapeColor = ew::Vec3(1.0f);
	bool wireframe = true;
	bool drawAsPoints = false;
	bool backFaceCulling = true;

	//Euler angles (degrees)
	ew::Vec3 lightRotation = ew::Vec3(0, 0, 0);
}appSettings;

ew::Camera camera;
ew::CameraController cameraController;

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

	//Enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//Depth testing - required for depth sorting!
	glEnable(GL_DEPTH_TEST);
	glPointSize(3.0f);
	glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);

	ew::Shader shader("assets/vertexShader.vert", "assets/fragmentShader.frag");
	unsigned int brickTexture = ew::loadTexture("assets/brick_color.jpg",GL_REPEAT,GL_LINEAR);

	//Create cube
	ew::MeshData cubeMeshData = ew::createCube(0.5);
	ew::Mesh cubeMesh(cubeMeshData);

	//Initialize transforms
	ew::Transform cubeTransform;

    //New meshes
	int planeSubDiv = 5;
    ew::MeshData planeMeshData = tsa::createPlane(0.5, planeSubDiv);
    ew::Mesh planeMesh(planeMeshData);

    ew::Transform planeTransform;
	planeTransform.position = ew::Vec3(1, 0, 0);

	int cylinderSubDiv = 10;
    ew::MeshData cylinderMeshData = tsa::createCylinder(0.5,0.2,cylinderSubDiv);
    ew::Mesh cylinderMesh(cylinderMeshData);

    ew::Transform cylinderTransform;
	cylinderTransform.position = ew::Vec3(2, 0, 0);

	int sphereSubDiv = 20;
    ew::MeshData sphereMeshData = tsa::createSphere(0.5, sphereSubDiv);
    ew::Mesh sphereMesh(sphereMeshData);

    ew::Transform sphereTransform;
	sphereTransform.position = ew::Vec3(3, 0, 0);

    int stackSubDiv = 10;
    int sliceSubDiv = 10;
    float innerRadius = 0.1;
    float outerRadius = 0.3;
    ew::MeshData torusMeshData = tsa::createTorus(innerRadius, outerRadius, stackSubDiv, sliceSubDiv);
    ew::Mesh torusMesh(torusMeshData);

    ew::Transform torusTransform;
    torusTransform.position = ew::Vec3(4, 0, 0);

    int coneSubDiv = 10;
    ew::MeshData coneMeshData = tsa::createCone(0.5, 0.3, coneSubDiv);
    ew::Mesh coneMesh(coneMeshData);

    ew::Transform coneTransform;
    coneTransform.position = ew::Vec3(5, 0, 0);

    int springStackSubDiv = 10;
    int springSliceSubDiv = 10;
    float springInnerRadius = 0.1;
    float springOuterRadius = 0.2;
    float springHeight = 1;
    int springCoils = 3;
    ew::MeshData springMeshData = tsa::createSpring(springHeight, springCoils, outerRadius, innerRadius, springStackSubDiv, springSliceSubDiv);
    ew::Mesh springMesh(springMeshData);

    ew::Transform springTransform;
    springTransform.position = ew::Vec3(6, 0, 0);

	resetCamera(camera,cameraController);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;

		float time = (float)glfwGetTime();
		float deltaTime = time - prevTime;
		prevTime = time;

		cameraController.Move(window, &camera, deltaTime);

		//Render
		glClearColor(appSettings.bgColor.x, appSettings.bgColor.y, appSettings.bgColor.z,1.0f);

		//Clear both color buffer AND depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindTexture(GL_TEXTURE_2D, brickTexture);
		shader.setInt("_Texture", 0);
		shader.setInt("_Mode", appSettings.shadingModeIndex);
		shader.setVec3("_Color", appSettings.shapeColor);
		shader.setMat4("_ViewProjection", camera.ProjectionMatrix() * camera.ViewMatrix());

		//Euler angels to forward vector
		ew::Vec3 lightRot = appSettings.lightRotation * ew::DEG2RAD;
		ew::Vec3 lightF = ew::Vec3(sinf(lightRot.y) * cosf(lightRot.x), sinf(lightRot.x), -cosf(lightRot.y) * cosf(lightRot.x));
		shader.setVec3("_LightDir", lightF);

		//Draw cube
		shader.setMat4("_Model", cubeTransform.getModelMatrix());
		cubeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

        //Draw Plane
        shader.setMat4("_Model", planeTransform.getModelMatrix());
        planeMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

        //Draw Cylinder
        shader.setMat4("_Model", cylinderTransform.getModelMatrix());
        cylinderMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

        //Draw sphere
        shader.setMat4("_Model", sphereTransform.getModelMatrix());
        sphereMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

        //Draw torus
        shader.setMat4("_Model", torusTransform.getModelMatrix());
        torusMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

        //Draw torus
        shader.setMat4("_Model", coneTransform.getModelMatrix());
        coneMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

        //Draw spring
        shader.setMat4("_Model", springTransform.getModelMatrix());
        springMesh.draw((ew::DrawMode)appSettings.drawAsPoints);

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

			ImGui::ColorEdit3("BG color", &appSettings.bgColor.x);
			ImGui::ColorEdit3("Shape color", &appSettings.shapeColor.x);
			ImGui::Combo("Shading mode", &appSettings.shadingModeIndex, appSettings.shadingModeNames, IM_ARRAYSIZE(appSettings.shadingModeNames));
			if (appSettings.shadingModeIndex > 3) {
				ImGui::DragFloat3("Light Rotation", &appSettings.lightRotation.x, 1.0f);
			}
			ImGui::Checkbox("Draw as points", &appSettings.drawAsPoints);
			if (ImGui::Checkbox("Wireframe", &appSettings.wireframe)) {
				glPolygonMode(GL_FRONT_AND_BACK, appSettings.wireframe ? GL_LINE : GL_FILL);
			}
			if (ImGui::Checkbox("Back-face culling", &appSettings.backFaceCulling)) {
				if (appSettings.backFaceCulling)
					glEnable(GL_CULL_FACE);
				else
					glDisable(GL_CULL_FACE);
			}
			if (ImGui::CollapsingHeader("Mesh Data")) {
                if (ImGui::CollapsingHeader("Plane Data")) {
                    if (ImGui::DragInt("Plane Subdivisions", &planeSubDiv, 1, 1, 300)) {
                        planeMeshData = tsa::createPlane(0.5, planeSubDiv);
                        planeMesh.load(planeMeshData);
                    }
                    ImGui::DragFloat3("Plane Position", &planeTransform.position.x, 0.1f);
                    ImGui::DragFloat3("Plane Scale", &planeTransform.scale.x, 0.1f);
                }
                if (ImGui::CollapsingHeader("Cylinder Data")) {
                    if (ImGui::DragInt("Cylinder Subdivisions", &cylinderSubDiv, 1, 3, 300)) {
                        cylinderMeshData = tsa::createCylinder(0.5, 0.2, cylinderSubDiv);
                        cylinderMesh.load(cylinderMeshData);
                    }
                    ImGui::DragFloat3("Cylinder Position", &cylinderTransform.position.x, 0.1f);
                    ImGui::DragFloat3("Cylinder Scale", &cylinderTransform.scale.x, 0.1f);
                }
                if (ImGui::CollapsingHeader("Sphere Data")) {
                    if (ImGui::DragInt("Sphere Subdivisions", &sphereSubDiv, 1, 3, 300)) {
                        sphereMeshData = tsa::createSphere(0.5, sphereSubDiv);
                        sphereMesh.load(sphereMeshData);
                    }
                    ImGui::DragFloat3("Sphere Position", &sphereTransform.position.x, 0.1f);
                    ImGui::DragFloat3("Sphere Scale", &sphereTransform.scale.x, 0.1f);
                }
                if (ImGui::CollapsingHeader("Torus Data")) {
                    if (ImGui::DragInt("Torus Stack Subdivisions", &stackSubDiv, 1, 3, 300)) {
                        torusMeshData = tsa::createTorus(innerRadius, outerRadius, stackSubDiv, sliceSubDiv);
                        torusMesh.load(torusMeshData);
                    }
                    if (ImGui::DragInt("Torus Slice Subdivisions", &sliceSubDiv, 1, 3, 300)) {
                        torusMeshData = tsa::createTorus(innerRadius, outerRadius, stackSubDiv, sliceSubDiv);
                        torusMesh.load(torusMeshData);
                    }
                    if (ImGui::DragFloat("Torus Inner Radius", &innerRadius, 0.01, 0.001, 10)) {
                        torusMeshData = tsa::createTorus(innerRadius, outerRadius, stackSubDiv, sliceSubDiv);
                        torusMesh.load(torusMeshData);
                    }
                    if (ImGui::DragFloat("Torus Outer Radius", &outerRadius, 0.01, 0.001, 10)) {
                        torusMeshData = tsa::createTorus(innerRadius, outerRadius, stackSubDiv, sliceSubDiv);
                        torusMesh.load(torusMeshData);
                    }
                    ImGui::DragFloat3("Torus Position", &torusTransform.position.x, 0.1f);
                    ImGui::DragFloat3("Torus Scale", &torusTransform.scale.x, 0.1f);
                }
                if (ImGui::CollapsingHeader("Cone Data")) {
                    if (ImGui::DragInt("Cone Subdivisions", &coneSubDiv, 1, 3, 300)) {
                        coneMeshData = tsa::createCone(0.5, 0.3, coneSubDiv);
                        coneMesh.load(coneMeshData);
                    }
                    ImGui::DragFloat3("Cone Position", &coneTransform.position.x, 0.1f);
                    ImGui::DragFloat3("Cone Scale", &coneTransform.scale.x, 0.1f);
                }
                if (ImGui::CollapsingHeader("Spring Data")) {
                    if (ImGui::DragInt("Spring Stack Subdivisions", &springStackSubDiv, 1, 3, 300)) {
                        springMeshData = tsa::createSpring(springHeight, springCoils, springOuterRadius,
                                                           springInnerRadius, springStackSubDiv, springSliceSubDiv);
                        springMesh.load(springMeshData);
                    }
                    if (ImGui::DragInt("Spring Slice Subdivisions", &springSliceSubDiv, 1, 3, 300)) {
                        springMeshData = tsa::createSpring(springHeight, springCoils, springOuterRadius,
                                                           springInnerRadius, springStackSubDiv, springSliceSubDiv);
                        springMesh.load(springMeshData);
                    }
                    if (ImGui::DragInt("Spring Coils", &springCoils, 1, 1, 50)) {
                        springMeshData = tsa::createSpring(springHeight, springCoils, springOuterRadius,
                                                           springInnerRadius, springStackSubDiv, springSliceSubDiv);
                        springMesh.load(springMeshData);
                    }
                    if (ImGui::DragFloat("Spring Height", &springHeight, 0.1, 0.01, 50)) {
                        springMeshData = tsa::createSpring(springHeight, springCoils, springOuterRadius,
                                                           springInnerRadius, springStackSubDiv, springSliceSubDiv);
                        springMesh.load(springMeshData);
                    }
                    if (ImGui::DragFloat("Spring Inner Radius", &springInnerRadius, 0.1, 0.1, 50)) {
                        springMeshData = tsa::createSpring(springHeight, springCoils, springOuterRadius,
                                                           springInnerRadius, springStackSubDiv, springSliceSubDiv);
                        springMesh.load(springMeshData);
                    }
                    if (ImGui::DragFloat("Spring Outer Radius", &springOuterRadius, 0.1, 0.1, 50)) {
                        springMeshData = tsa::createSpring(springHeight, springCoils, springOuterRadius,
                                                           springInnerRadius, springStackSubDiv, springSliceSubDiv);
                        springMesh.load(springMeshData);
                    }
                    ImGui::DragFloat3("Spring Position", &springTransform.position.x, 0.1f);
                    ImGui::DragFloat3("Spring Scale", &springTransform.scale.x, 0.1f);
                }
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
	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
}

void resetCamera(ew::Camera& camera, ew::CameraController& cameraController) {
	camera.position = ew::Vec3(0, 0, 3);
	camera.target = ew::Vec3(0);
	camera.aspectRatio = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
	camera.fov = 60.0f;
	camera.orthoHeight = 6.0f;
	camera.nearPlane = 0.1f;
	camera.farPlane = 100.0f;
	camera.orthographic = false;

	cameraController.yaw = 0.0f;
	cameraController.pitch = 0.0f;
}


