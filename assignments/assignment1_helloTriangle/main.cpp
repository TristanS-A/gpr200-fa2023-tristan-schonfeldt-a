#include <stdio.h>
#include <math.h>

#include <ew/external/glad.h>
#include <ew/ewMath/ewMath.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 900;

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

unsigned int createVAO(float* vertexData, int numVertices, int totatlLength){

    //Creates vertex buffer object
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //Assigns vertices to vertex buffer
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(float)* totatlLength), vertexData, GL_STATIC_DRAW);

    //Creates vertex array object
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    //Assigns vertex buffer object tp vao
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    //Creates attribute pointer to position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, static_cast<int>(sizeof(float)) * (totatlLength / numVertices), (const void*)0);
    //Enables attribute at location 0;
    glEnableVertexAttribArray(0);

    //Creates attribute pointer to color
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, static_cast<int>(sizeof(float)) * (totatlLength / numVertices), (const void*)(sizeof(float)*3));
    //Enables attribute at location 1
    glEnableVertexAttribArray(1);

    return vao;
}

unsigned int createShader(GLenum shaderType, const char* sourceCode){

    //Creates shader
    unsigned int shader = glCreateShader(shaderType);
    //Adds shader source code
    glShaderSource(shader, 1, &sourceCode, NULL);
    //Compiles shader
    glCompileShader(shader);

    //Test if compiled correctly
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

    //Creates vertex and fragment shaders
    unsigned int vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSource);
    unsigned int fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    //Creates shader program from vertex and fragment shader
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Test if program linked properly
    int success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success){
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Failed to link shader program: %s", infoLog);
        return 0;
    }

    //Deletes old shaders
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

    //Creates shader program
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    //Creates vertex array buffer
    unsigned int vao = createVAO(verticies, 3, 21);

    //Render loop
	while (!glfwWindowShouldClose(window)) {

        //Gets time
        float time = static_cast<float>(glfwGetTime());

        //Finds uniform time in shader source code
        int timeLocation = glGetUniformLocation(shaderProgram, "_Time");

        //Sets uniform to time
        glUniform1f(timeLocation, time);

        //Defines the rotation matrix to rotate vertices
        float rotMatrix[9] = {
                cos(time), -sin(time), 0.0,
                sin(time), cos(time), 0.0,
                0.0, 0.0, 1.0,
                };

        //Gets matrix uniform location in source code
        int matrixLocation = glGetUniformLocation(shaderProgram, "_RotMatrix");
        //Sets matrix to rotMatrix
        glUniformMatrix3fv(matrixLocation, 1, GL_FALSE, rotMatrix);

		glfwPollEvents();

        //Clear screen
		glClearColor(abs(sin(time)), abs(sin(time / 3)), abs(sin(time / 2)), 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        //Display vertices with shader program, vao, and a draw call
        glUseProgram(shaderProgram);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,3);

		glfwSwapBuffers(window);
	}
	printf("Shutting down...");
}
