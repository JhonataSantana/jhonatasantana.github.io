#include "jetpack.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Jetpack::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_init.x = -0.75;
  m_init.y = -0.85;
  m_translation.x = m_init.x;
  m_translation.y = m_init.y;
  m_velocity = glm::vec2(0);

  m_speedY = 0.0f;    
  m_gravity = 0.05;
  m_gravitySpeed = 0.0f;

  std::array<glm::vec2, 24> positions{
      // Ship body
      glm::vec2{-02.5f, +12.5f}, glm::vec2{-15.5f, +02.5f},
      glm::vec2{-15.5f, -12.5f}, glm::vec2{-09.5f, -07.5f},
      glm::vec2{-03.5f, -12.5f}, glm::vec2{+03.5f, -12.5f},
      glm::vec2{+09.5f, -07.5f}, glm::vec2{+15.5f, -12.5f},
      glm::vec2{+15.5f, +02.5f}, glm::vec2{+02.5f, +12.5f},

      // Cannon left
      glm::vec2{-12.5f, +10.5f}, glm::vec2{-12.5f, +04.0f},
      glm::vec2{-09.5f, +04.0f}, glm::vec2{-09.5f, +10.5f},

      // Cannon right
      glm::vec2{+09.5f, +10.5f}, glm::vec2{+09.5f, +04.0f},
      glm::vec2{+12.5f, +04.0f}, glm::vec2{+12.5f, +10.5f},
      
      // Thruster trail (left)
      glm::vec2{-12.0f, -07.5f}, 
      glm::vec2{-09.5f, -18.0f}, 
      glm::vec2{-07.0f, -07.5f},

      // Thruster trail (right)
      glm::vec2{+07.0f, -07.5f}, 
      glm::vec2{+09.5f, -18.0f}, 
      glm::vec2{+12.0f, -07.5f},
      };

  // Normalize
  for (auto &position :positions) {
    position /= glm::vec2{15.5f, 15.5f};
  }

  std::array indices{0, 1, 3,
                     1, 2, 3,
                     0, 3, 4,
                     0, 4, 5,
                     9, 0, 5,
                     9, 5, 6,
                     9, 6, 8,
                     8, 6, 7,
                     // Cannons
                     10, 11, 12,
                     10, 12, 13,
                     14, 15, 16,
                     14, 16, 17,
                     // Thruster trails
                     18, 19, 20,
                     21, 22, 23};

  // Generate VBO
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  glGenBuffers(1, &m_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  glGenVertexArrays(1, &m_vao);

  // Bind vertex attributes to current VAO
  glBindVertexArray(m_vao);

  glEnableVertexAttribArray(positionAttribute);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Jetpack::paintGL(const GameData &gameData) {
  if (gameData.m_state != State::Playing) return;

  glUseProgram(m_program);

  glBindVertexArray(m_vao);

  glUniform1f(m_scaleLoc, m_scale);
  glUniform1f(m_rotationLoc, m_rotation);
  glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0) m_trailBlinkTimer.restart();

  if (gameData.m_input[static_cast<size_t>(Input::Up)] || gameData.m_input[static_cast<size_t>(Input::Fire)]) {
    // Show thruster trail during 50 ms
    if (m_trailBlinkTimer.elapsed() < 50.0 / 1000.0) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // 50% transparent
      glUniform4f(m_colorLoc, 1, 1, 1, 0.5f);

      glDrawElements(GL_TRIANGLES, 14 * 3, GL_UNSIGNED_INT, nullptr);

      glDisable(GL_BLEND);
    }
  }

  glUniform4fv(m_colorLoc, 1, &m_color.r);
  glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);

  glUseProgram(0);
}

void Jetpack::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteBuffers(1, &m_ebo);
  glDeleteVertexArrays(1, &m_vao);
}

//void Jetpack::update(const GameData &gameData, float deltaTime) {

void Jetpack::update(const GameData &gameData) {
  if (gameData.m_input[static_cast<size_t>(Input::Up)] || gameData.m_input[static_cast<size_t>(Input::Fire)]){

    m_gravity = 0.0007;

    if(m_translation.y < 0.5){
      m_gravitySpeed += m_gravity;
      m_translation.y += m_speedY + m_gravitySpeed;
    }

    if(m_translation.y >= 0.5){
      m_translation.y = 0.5;
      m_speedY = 0.0f;
      m_gravitySpeed = 0.0f;
    }

    if(m_translation.y <= m_init.y){
      m_translation.y = m_init.y;
      m_gravitySpeed = 0.0f;
    }


  }else{
    if(m_translation.y >= 0.5){
      m_translation.y = 0.5;
      m_speedY = 0.0f;
      m_gravitySpeed = 0.0f;
    }
    m_gravity = -0.0007;
    if(m_translation.y > m_init.y){
      m_gravitySpeed += m_gravity;
      m_translation.y += m_speedY + m_gravitySpeed;
    }
    if(m_translation.y <= m_init.y){
      m_translation.y = m_init.y;
      m_gravitySpeed = 0.0f;
    }
  }

  // Apply thrust
  /*if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Thrust in the forward vector
    glm::vec2 forward = glm::rotate(glm::vec2{0.0f, 1.0f}, m_rotation);
    m_velocity += forward * deltaTime;
  }*/
}
