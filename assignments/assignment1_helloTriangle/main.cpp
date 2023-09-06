#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 1080;
const int SCREEN_HEIGHT = 720;

float verticies[21] = {
		0.0, 0.5, 0.0, 1.0, 0.0, 0.0, 1.0, /*Top*/
		0.5, -0.5, 0.0, 0.0, 1.0, 0.0, 1.0, /*Right*/
		-0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0 /*Left*/
};

const char* vertexShaderSource = R"(
    #version 450
    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec4 vColor;
    out vec4 Color;
    uniform mat3 _RotMatrix;
    void main(){
        gl_Position = vec4(_RotMatrix * vPos, 1.0);
        Color = vColor;
    }
)";

const char* fragmentShaderSource = R"(
    #version 450
    in vec4 Color;
    out vec4 FragColor;
    uniform float _Time;
    void main(){
        FragColor = Color * abs(sin(_Time));
    }
)";

unsigned int createVAO(float* vertexData, int numVertices){
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), vertexData, GL_STATIC_DRAW);

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    const int NUM_COLORS = 4;

    glVertexAttribPointer(0, numVertices, GL_FLOAT, GL_FALSE, static_cast<int>(sizeof(float)) * (numVertices + NUM_COLORS), (const void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, static_cast<int>(sizeof(float)) * (numVertices + NUM_COLORS), (const void*)(sizeof(float)*3));
    glEnableVertexAttribArray(1);

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
        float time = (float)glfwGetTime();
        int timeLocation = glGetUniformLocation(shaderProgram, "_Time");
        glUniform1f(timeLocation, time);

        float rotMatrix[9] = {
                cos(time), 0.0, -sin(time),
                0.0, 1, 0.0,
                sin(time), 0.0, cos(time),
                };

        int matrixLocation = glGetUniformLocation(shaderProgram, "_RotMatrix");
        glUniformMatrix3fv(matrixLocation, 1, GL_FALSE, rotMatrix);

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
