#include "block.hpp"

#include <cppitertools/itertools.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Blocks::initializeGL(GLuint program) {
  terminateGL();

  m_program = program;
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_rotationLoc = glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_translationLoc = glGetUniformLocation(m_program, "translation");

  m_blocks.clear();

  // Create regular polygon
  auto sides{60};

  std::vector<glm::vec2> positions(0);
  positions.emplace_back(0, 0);
  auto step{M_PI * 2 / sides};
  for (auto angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
  }
  positions.push_back(positions.at(1));

  // Generate VBO of positions
  glGenBuffers(1, &m_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

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

  // End of binding to current VAO
  glBindVertexArray(0);
}

void Blocks::paintGL() {
  glUseProgram(m_program);

  glBindVertexArray(m_vao);
  glUniform4f(m_colorLoc, 1, 1, 1, 1);
  glUniform1f(m_rotationLoc, 0);
  glUniform1f(m_scaleLoc, m_scale);

  for (auto &block : m_blocks) {
    glUniform2f(m_translationLoc, block.m_translation.x,
                block.m_translation.y);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 62);
  }

  glBindVertexArray(0);

  glUseProgram(0);

  // Start pseudo-random number generator
  auto seed{std::chrono::steady_clock::now().time_since_epoch().count()};
  m_randomEngine.seed(seed);

}

void Blocks::terminateGL() {
  glDeleteBuffers(1, &m_vbo);
  glDeleteVertexArrays(1, &m_vao);
}

void Blocks::update(Jetpack &jetpack, const GameData &gameData, float deltaTime) {
  // Create a pair of bullets
  if (gameData.m_state == State::Playing) {
    // At least 250 ms must have passed since the last bullets
    if (jetpack.m_blockCoolDownTimer.elapsed() > m_delay / 1000.0) {
      jetpack.m_blockCoolDownTimer.restart();

      // Bullets are shot in the direction of the jetpack's forward vector
      glm::vec2 forward{glm::rotate(glm::vec2{0.0f, 1.0f}, 4.712f)};
      glm::vec2 right{glm::rotate(glm::vec2{1.0f, 0.0f}, jetpack.m_rotation)};
      //auto cannonOffset{(11.0f / 15.5f) * jetpack.m_scale};
      auto blockSpeed{2.0f};

      std::uniform_real_distribution<float> rd1(-1.0f, 0.5f);

      Block block{.m_dead = false,
                    .m_translation = glm::vec2(1.0f, rd1(m_randomEngine)),
                    .m_velocity = jetpack.m_velocity + forward * blockSpeed};

      m_blocks.push_back(block);

    }
  }

  for (auto &block : m_blocks) {
    block.m_translation.x -= 2.0f * deltaTime;
    //block.m_translation -= block.m_velocity * deltaTime;

    // Kill bullet if it goes off screen
    if (block.m_translation.x < -1.1f) block.m_dead = true;
    if (block.m_translation.x > +1.1f) block.m_dead = true;
    if (block.m_translation.y < -1.1f) block.m_dead = true;
    if (block.m_translation.y > +1.1f) block.m_dead = true;
  }

  // Remove dead bullets
  m_blocks.remove_if([&](const Block &p) { return p.m_dead; });

}
