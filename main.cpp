#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// SHADERS DEFINED HERE
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 transform;\n"
"void main() {\n"
"\tgl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"\tFragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

void framebufferResize(GLFWwindow* win, int width, int height);
void framebufferResize(GLFWwindow* win, int width, int height) {
  glViewport(0, 0, width, height);
}

glm::vec2 processInput(GLFWwindow *window) {
  glm::vec2 dir(0.0f);
  if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
    glfwSetWindowShouldClose(window, true);
  }
  if (glfwGetKey(window, GLFW_KEY_W)) {
    dir.y += 0.01f;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    dir.y -= 0.01f;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    dir.x -= 0.01f;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    dir.x += 0.01f;
  }
  return dir;
}

int main() {
  // We're initializing GLFW here to make sure that the correct version is installed
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // Now we can make a window to actually render stuff
  GLFWwindow* window = glfwCreateWindow(800, 600, "Zandris", NULL, NULL); // Init glfw
  if (window == NULL) {
    std::cerr << "Failed to make GLFW window, check your display manager?\n"; // Making sure the window was actually made
    glfwTerminate();
    return 1; // Error out
  }
  glfwMakeContextCurrent(window); // Init glad
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to init GLAD\n";
    return 1;
  }
  glfwSetFramebufferSizeCallback(window, framebufferResize); // Making window resizable
  float verts[] = {
    -0.05f, -0.1f, 0.0f,
    0.05f, -0.1f, 0.0f,
    -0.05f, 0.1f, 0.0f,

    0.05f, -0.1f, 0.0f,
    0.05f, 0.1f, 0.0f,
    -0.05f, 0.1f, 0.0f
  }; // Making verticies with (x, y, z) coords
     // The domain is [-1, 1], and the range is also [-1, 1]
  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
  unsigned int vertexShader;
  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  unsigned int fragmentShader;
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  unsigned int shaderProgram;
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // glUseProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glUseProgram(shaderProgram);
  glm::mat4 transform = glm::mat4(1.0f);
  int transLoc = glGetUniformLocation(shaderProgram, "transform");
  float speed = 0.01f;
  glm::vec2 pos(0.0f);
  while (!glfwWindowShouldClose(window)) { // Loop :D
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::vec2 input = processInput(window);
    if (glm::length(input) > 0.0f) input = glm::normalize(input);
    pos += input * speed;
    float limit = 1.0f, halfSize = 0.5f;
    pos.x = glm::clamp(pos.x, -limit, limit - halfSize);
    pos.y = glm::clamp(pos.y, -limit + halfSize, limit - halfSize);
    // transform = glm::translate(transform, glm::vec3(input.x * speed, input.y * speed, 0.0f));
    transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate(); // Kill glfw
  return 0;
}
