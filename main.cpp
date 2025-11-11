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
"uniform vec4 color;\n"
"void main() {\n"
"\tFragColor = color;\n"
"}\0";

void framebufferResize(GLFWwindow* win, int width, int height);
void framebufferResize(GLFWwindow* win, int width, int height) {
  glViewport(0, 0, width, height);
}

glm::vec2 processMovement(GLFWwindow *window) {
  glm::vec2 dir(0.0f);
  if (glfwGetKey(window, GLFW_KEY_W) || glfwGetKey(window, GLFW_KEY_UP) || glfwGetKey(window, GLFW_KEY_SPACE)) dir.y += 0.1f;
  // if (glfwGetKey(window, GLFW_KEY_S) || glfwGetKey(window, GLFW_KEY_DOWN)) dir.y -= 0.01f;
  if (glfwGetKey(window, GLFW_KEY_A) || glfwGetKey(window, GLFW_KEY_LEFT)) dir.x -= 1.0f;
  if (glfwGetKey(window, GLFW_KEY_D) || glfwGetKey(window, GLFW_KEY_RIGHT)) dir.x += 1.0f;
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) dir.x *= 1.7;
  return dir;
}
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);
}

void draw(GLFWwindow *window, unsigned int VAO, unsigned int VBO, unsigned int shaderProgram, const float *verts, glm::vec2 pos, glm::vec4 color, int transLoc, int colorLoc, int low, int high) {
  glBindVertexArray(VAO);
  glUseProgram(shaderProgram);
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
  glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));
  glUniform4fv(colorLoc, 1, glm::value_ptr(color));
  glDrawArrays(GL_TRIANGLES, low, high);
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
  unsigned int VAO; // The V(ertex)A(rray)O(bject) stores vertex info for late use
  glGenVertexArrays(1, &VAO); // Making array (don't I send verts anyways? Idk)
  glBindVertexArray(VAO); // Binding to the variable called VAO
  unsigned int VBO; // The V(ertex)B(uffer)O(bject) tells GPU how to process verts I think
  glGenBuffers(1, &VBO); // Generating buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO); // Binding to VBO variable, I believe that the GL_ARRAY_BUFFER is global
  glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW); // Telling GPU what to draw (verts) and how to draw it (Dynamic draw)
  unsigned int vertexShader; // Vertex shader info at the top of the file, vertexShaderSource
  vertexShader = glCreateShader(GL_VERTEX_SHADER); // Making shader
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); // Binding shader infor
  glCompileShader(vertexShader); // Compiling GLSL shader
  unsigned int fragmentShader; // Same as vertex but for fragment
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  unsigned int shaderProgram; // Combines vertex and fragment shaders
  shaderProgram = glCreateProgram(); // Init shader program
  glAttachShader(shaderProgram, vertexShader); // Attaching vert shader
  glAttachShader(shaderProgram, fragmentShader); // Attaching frag shader
  glLinkProgram(shaderProgram); // Linking program
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glDeleteShader(vertexShader); // Don't need shaders once program's made 
  glDeleteShader(fragmentShader);
  glm::mat4 transform = glm::mat4(1.0f); // Instead of reassignment, just tell the GPU how to move something
  int transLoc = glGetUniformLocation(shaderProgram, "transform"); // Sends it to the uniform "transform" in the shader that updates vert info
  float speed = 0.0f, yVel = 0.0f, grav = -0.01981f, timeValue = glfwGetTime(), greenValue = ((sin(timeValue) / 2.0f) + 0.5f);
  glm::vec2 pos(0.0f, -0.95f);
  glm::vec4 color(0.0f, 0.0f, 0.0f, 1.0f);
  int colorLoc = glGetUniformLocation(shaderProgram, "color"), vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
  glUniform4fv(colorLoc, 1, glm::value_ptr(color));
  glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
  while (!glfwWindowShouldClose(window)) { // Loop :D
    timeValue = glfwGetTime();
    greenValue = (sin(timeValue) / 2.0f) + 0.5f;
    vertexColorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glm::vec2 input = processMovement(window);
    processInput(window);
    float dTime = 1.0f / 60.0f;
    pos.x += input.x * 0.01 + speed;
    yVel += grav * dTime;
    float limit = 1.0f, halfSize = 0.05f;
    if (pos.y >= -0.95 + halfSize + 0.0001) pos.y += 0.01 + yVel;
    else {
      yVel += input.y * 0.03;
      pos.y += yVel;
      yVel = 0.0f;
    }
    if (pos.y < -0.95 + halfSize) {
      pos.y = -0.95 + halfSize;
      yVel = 0.0f;
    }
    pos.x = glm::clamp(pos.x, -limit + halfSize, limit - halfSize);
    transform = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(transLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glBindVertexArray(VAO);
    color.x = 0.5f;
    color.y = greenValue;
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
    draw(window, VAO, VBO, shaderProgram, verts, pos, color, transLoc, colorLoc, 0, 3);
    color.y = 0.0f;
    color.x = greenValue;
    glUniform4fv(colorLoc, 1, glm::value_ptr(color));
    draw(window, VAO, VBO, shaderProgram, verts, pos, color, transLoc, colorLoc, 3, 6);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwTerminate(); // Kill glfw
  return 0;
}
