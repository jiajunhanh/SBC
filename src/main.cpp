#include "sbc.h"
#include <shader_m.h>
#include <camera.h>

#include <glm/glm.hpp>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

int main() {
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "SBC", nullptr, nullptr);
  if (window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  gladLoadGL(glfwGetProcAddress);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile our shader zprogram
  // ------------------------------------
  Shader cube_shader("shaders/cube.vs", "shaders/cube.fs");
  Shader texture_shader("shaders/texture.vs", "shaders/texture.fs");

  // set up vertex data (and buffer(s)) and configure vertex attributes
  // ------------------------------------------------------------------
  float cube_vertices[] = {
      0.5f, 0.5f, 0.5f,
      -0.5f, 0.5f, 0.5f,
      -0.5f, -0.5f, 0.5f,
      0.5f, -0.5f, 0.5f,
      0.5f, 0.5f, -0.5f,
      -0.5f, 0.5f, -0.5f,
      -0.5f, -0.5f, -0.5f,
      0.5f, -0.5f, -0.5f
  };

  unsigned int cube_indices[] = {
      0, 1, 1, 2, 2, 3, 3, 0,
      4, 5, 5, 6, 6, 7, 7, 4,
      0, 4, 1, 5, 2, 6, 3, 7
  };

  unsigned char cube_color[] = {
      255, 165, 0,
      255, 0, 0,
      0, 255, 0,
      255, 255, 0,
      0, 255, 255,
      0, 255, 0,
      0, 165, 255,
      0, 0, 255
  };

  float texture_vertices[] = {
      -0.433f, 0.25f, -0.5f, 1.0f, 1.0f,
      0.433f, -0.25f, -0.5f, 1.0f, 0.0f,
      0.433f, -0.25f, 0.5f, 0.0f, 0.0f,
      -0.433f, 0.25f, 0.5f, 0.0f, 1.0f
  };

  unsigned int texture_indices[] = {
      0, 1, 3,
      1, 2, 3
  };

  unsigned int VBO[2], VAO[2], EBO[2];
  glGenVertexArrays(2, VAO);
  glGenBuffers(2, VBO);
  glGenBuffers(2, EBO);

  // setting for cube
  glBindVertexArray(VAO[0]);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  // setting for texture
  glBindVertexArray(VAO[1]);

  glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices), texture_vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(texture_indices), texture_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  // create a texture
  // -------------------------
  unsigned int texture;
  // texture 1
  // ---------
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // create sbc
  std::vector<glm::vec3> sbc_vertices;
  for (int i = 0; i < 24; i += 3) {
    sbc_vertices.emplace_back(cube_vertices[i], cube_vertices[i + 1], cube_vertices[i + 2]);
  }
  std::vector<std::vector<int>> sbc_faces{
      {0, 1, 2, 3}, {0, 4, 5, 1}, {0, 3, 7, 4},
      {4, 7, 6, 5}, {2, 6, 7, 3}, {1, 5, 6, 2}
  };

  SBC sbc(std::move(sbc_vertices), std::move(sbc_faces));
  // create texture and generate mipmaps
  int width{512}, height{512};
  auto data = std::vector<unsigned char>(width * height * 3, 0);
  for (int j = 0; j < height; ++j) {
    for (int i = 0; i < width; ++i) {
      int idx = (j * width + i) * 3;
      float u = ((float) i + 0.5f) / (float) width;
      float v = ((float) j + 0.5f) / (float) height;
      float x = (1 - v) * texture_vertices[2 * 5] + v * texture_vertices[3 * 5];
      float y = (1 - v) * texture_vertices[2 * 5 + 1] + v * texture_vertices[3 * 5 + 1];
      float z = (1 - u) * texture_vertices[2 * 5 + 2] + u * texture_vertices[5 + 2];
      auto w = sbc.get_spherical_barycentric_coordinate(glm::vec3(x, y, z));

      float r = 0.0f;
      float g = 0.0f;
      float b = 0.0f;
      for (int k = 0; k < 8; ++k) {
        r += w[k] * (float) cube_color[k * 3];
        g += w[k] * (float) cube_color[k * 3 + 1];
        b += w[k] * (float) cube_color[k * 3 + 2];
      }

      data[idx] = (unsigned char) r;
      data[idx + 1] = (unsigned char) g;
      data[idx + 2] = (unsigned char) b;
    }
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
  glGenerateMipmap(GL_TEXTURE_2D);

  // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
  // -------------------------------------------------------------------------------------------
  texture_shader.use();
  texture_shader.setInt("Texture", 0);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // per-frame time logic
    // --------------------
    auto currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind textures on corresponding texture units
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4
        projection = glm::perspective(glm::radians(camera.Zoom), (float) SCR_WIDTH / (float) SCR_HEIGHT, 0.1f, 100.0f);

    // camera/view transformation
    glm::mat4 view = camera.GetViewMatrix();

    // render cube
    cube_shader.use();
    cube_shader.setMat4("projection", projection);
    cube_shader.setMat4("view", view);
    cube_shader.setMat4("model", glm::mat4(1.0f));

    glBindVertexArray(VAO[0]);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr);

    // render texture
    texture_shader.use();
    texture_shader.setMat4("projection", projection);
    texture_shader.setMat4("view", view);
    texture_shader.setMat4("model", glm::mat4(1.0f));

    glBindVertexArray(VAO[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------
  glDeleteVertexArrays(2, VAO);
  glDeleteBuffers(2, VBO);
  glDeleteBuffers(2, EBO);

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  (void) window;
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
  (void) window;
  auto xpos = static_cast<float>(xposIn);
  auto ypos = static_cast<float>(yposIn);

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  (void) window;
  (void) xoffset;
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}