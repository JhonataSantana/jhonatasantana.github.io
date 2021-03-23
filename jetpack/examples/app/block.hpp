#ifndef BLOCKS_HPP_
#define BLOCKS_HPP_

#include <list>
#include <random>

#include "abcg.hpp"
#include "gamedata.hpp"
#include "jetpack.hpp"

class OpenGLWindow;

class Blocks {
 public:
  void initializeGL(GLuint program);
  void paintGL();
  void terminateGL();

  void update(Jetpack &jetpack, const GameData &gameData, float deltaTime);

 private:
  friend OpenGLWindow;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  GLuint m_vao{};
  GLuint m_vbo{};

  struct Block {
    bool m_dead{false};
    glm::vec2 m_translation{glm::vec2(0)};
    glm::vec2 m_velocity{glm::vec2(0)};
    float m_scale{0.18f};
  };

  float m_scale{0.18f};
  int m_delay{1500};

  std::list<Block> m_blocks;
  std::default_random_engine m_randomEngine;

};

#endif