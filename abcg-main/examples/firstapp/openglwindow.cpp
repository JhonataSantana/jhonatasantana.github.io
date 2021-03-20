#include <fmt/core.h>

#include "openglwindow.hpp"

#include <imgui.h>

void OpenGLWindow::initializeGL() {
  auto windowSettings{getWindowSettings()};
  fmt::print("Initial window size: {}x{}\n", windowSettings.width,
             windowSettings.height);
}

void OpenGLWindow::paintGL() {
  // Set the clear color
  glClearColor(m_clearColor[0], m_clearColor[1], m_clearColor[2],
               m_clearColor[3]);
  // Clear the color buffer
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLWindow::paintUI() {
  // Parent class will show fullscreen button and FPS meter
  abcg::OpenGLWindow::paintUI();

  // Our own ImGui widgets go below
  /*{
    // Window begin
    ImGui::Begin("Hello, First App!");

    // Static text
    auto windowSettings{getWindowSettings()};
    ImGui::Text("Current window size: %dx%d (in windowed mode)",
                windowSettings.width, windowSettings.height);

    // Slider from 0.0f to 1.0f
    static float f{};
    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

    // ColorEdit to change the clear color
    ImGui::ColorEdit3("clear color", m_clearColor.data());

    // More static text
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0 / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

    // Window end
    ImGui::End();
  }*/

  //Testes de componentes
  {
    // Window begin
    ImGui::Begin("Jogo da Velha");

    static int jogadas = 0;

    // Static text
    auto windowSettings{getWindowSettings()};
    ImGui::Text("Jogadas Restantes: %d", jogadas);

    // 100x50 button
    /*if (ImGui::Button("Press me!", ImVec2(100, 50))) {
      fmt::print("Button pressed.\n");
    }*/

    ImGui::Columns(3, "mycolumns3");  // 3-ways, no border


    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Separator();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();
    ImGui::Button("O", ImVec2(-1, 50));
    ImGui::NextColumn();

    ImGui::Columns(1);

    static int topleft = 0;

    if(topleft == 0){
      if (ImGui::Button("Press me!", ImVec2(100, 50))) {
        topleft++;
        jogadas++;
      }
    }else{
      ImGui::Button("Press me! ok", ImVec2(100, 50));
    }
    

    // Nx50 button, where N is the remaining width available
    /*ImGui::Button("Press me!", ImVec2(-1, 50));*/
    // See also IsItemHovered, IsItemActive, etc
    /*if (ImGui::IsItemClicked()) {
      fmt::print("Button pressed.\n");
    }*/

    // Window end
    ImGui::End();
  }

  ImGui::ShowDemoWindow();

}