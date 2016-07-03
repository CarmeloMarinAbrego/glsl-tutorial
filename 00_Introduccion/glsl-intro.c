#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// verificar errores en el shader object, si los hay,
// muestra la informacion del error y elimina el shader
GLboolean check_shader(GLuint shader, const char* title) {
    GLint status = -1;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        GLchar* log_text = malloc(log_length);
        if (!log_text) return status;

        glGetShaderInfoLog(shader, log_length, NULL, log_text);
        glDeleteShader(shader);

        fprintf(stderr, "%s: %s", title, log_text);

        free(log_text);
        log_text = NULL;
    }

    return status;
}

// verificar errores al enlazar el programa, si los hay,
// muestra la informacion del error y elimina el shader program
GLboolean check_link(GLuint program) {
    GLint link_status = -1;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    
    if (link_status != GL_TRUE)
    {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

        GLchar* log_text = malloc(log_length);
        if (!log_text) return link_status;

        glGetProgramInfoLog(program, log_length, NULL, log_text);
        glDeleteProgram(program);

        fprintf(stderr, "%s: %s", "Link Program Error", log_text);

        free(log_text);
        log_text = NULL;
    }

    return link_status;
}

// compilar el shader object 
GLuint create_shader_object(GLenum type, const GLchar* shader_source) {
    GLuint shader = glCreateShader(type);
    
    glShaderSource(shader, 1, (const GLchar**)&shader_source, NULL);
    glCompileShader(shader);

    return shader;
}

GLuint compile_shaders(void)
{
    GLuint program = 0;

    // Source code for vertex shader
    const GLchar* vertex_shader_source =
    {
        "#version 330                             \n"
        "                                         \n"
        "layout (location = 0) in vec4 vPosition; \n"
        "layout (location = 1) in vec4 vColor;    \n"
        "                                         \n"
        "out vec4 color;                          \n"
        "                                         \n"
        "void main(void)                          \n"
        "{                                        \n"
        "   gl_Position = vPosition;              \n"
        "   color = vColor;                       \n"
        "}                                        \n"
    };

    // Source code for fragment shader
    const GLchar* fragment_shader_source =
    {
        "#version 330                             \n"
        "                                         \n"
        "in vec4 color;                           \n"
        "out vec4 frag_color;                     \n"
        "                                         \n"
        "void main(void)                          \n"
        "{                                        \n"
        "   frag_color = color;                   \n"
        "}                                        \n"
    };

    GLuint vertex_shader = create_shader_object(GL_VERTEX_SHADER, vertex_shader_source);
    GLuint fragment_shader = create_shader_object(GL_FRAGMENT_SHADER, fragment_shader_source);

    GLboolean shader_status = GL_TRUE;
    shader_status &= check_shader(vertex_shader, "Vertex Shader");
    shader_status &= check_shader(fragment_shader, "Fragment Shader");

    if (shader_status != GL_TRUE) return 0;

    // Create program, attach shaders to it, and link it
    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    if (check_link(program) != GL_TRUE) return 0;

    // Delete the shaders as the program has them now
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

int main(void)
{
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "TUTORIAL GLSL", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    GLuint program = compile_shaders();
    if (program == 0) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

	GLuint vao_triangle = 0;
	glGenVertexArrays(1, &vao_triangle);
	glBindVertexArray(vao_triangle);

	// vertices para generar un trinagulo 2D
	static const float vertex[] =
	{
		1.0f, -1.0f, 0.0f, 1.0f,
	   -1.0f, -1.0f, 0.0f, 1.0f,
		1.0f,  1.0f, 0.0f, 1.0f
	};

	// color RGBA para cada vertice
	static const float color[] =
	{
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};

	GLuint buffer[2];

	glGenBuffers(2, buffer);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

    glBindVertexArray(0);
	
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao_triangle);
		glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}