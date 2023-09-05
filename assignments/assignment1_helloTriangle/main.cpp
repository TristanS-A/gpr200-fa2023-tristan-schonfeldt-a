#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float verticies[9] = {
		0.0, 0.5, 0.0, //Top
		0.5, -0.5, 0.0,  //Right
		-0.5, -0.5, 0.0  //Left
};

const char* vertexShaderSource = R"(
    #version 450
    layout(location = 0) in vec3 vPos;
    void main(){
    gl_Position = vec4(vPos,1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 450
    out vec4 FragColor;
    void main(){
    FragColor = vec4(1.0);
    }
)";

unsigned int createVAO(float* vertexData, int numVertices){
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, numVertices, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (const void*)0);
    glEnableVertexAttribArray(0);
    return vao;
}

unsigned int createShader(GLenum shaderType, const char* sourceCode){
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &sourceCode, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success){
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Failed to compile shader: %s", infoLog);
        return 0;
    }

    return shader;
}

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource){
    unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success){
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
        printf("Failed to link shader program: %s", infoLog);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

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

	if (!gladLoadGL(glfwGetProcAddress)) {
		printf("GLAD Failed to load GL headers");
		return 1;
	}

    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    unsigned int vao = createVAO(verticies, 3);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClearColor(0.5f, 0.0f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
