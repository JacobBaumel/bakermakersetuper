#include "GLFW/glfw3.h"
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imfilebrowser.h"
#include "improgress.h"

#include "ThreadedExtractor.h"

constexpr int FOLDER_PICKER_FLAGS = ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir |
        ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_HideRegularFiles |
        ImGuiFileBrowserFlags_ConfirmOnEnter;

int main() {
    if(!glfwInit()) {
        std::cout << "Error initing glfw" << std::endl;
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(750, 500, "Window", nullptr, nullptr);
    if(!window) {
        std::cout << "Error creating window!" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration;
    bool open = true;

    ImGui::FileBrowser folderPicker(FOLDER_PICKER_FLAGS);
    folderPicker.SetTitle("Pick Folder");

    std::string folderOutput;

    const ImU32 fg = ImGui::GetColorU32(ImVec4(0.9, 0.9, 0.9, 1));
    const ImU32 bg = ImGui::GetColorU32(ImVec4(0.1, 0.1, 0.1, 1));

    bakermaker::ThreadedExtractor* extr = nullptr;
    bool hasExtractionStarted = false;

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

        if(ImGui::Begin("Bakermaker", &open, flags)) {
            if(ImGui::Button("Open Folder Picker Dialogue")) {
                folderPicker.Open();
            }

            folderPicker.Display();

            if(folderPicker.HasSelected()) {
                folderOutput = folderPicker.GetSelected().string();
                folderOutput.append("\\");
                folderPicker.ClearSelected();
                folderPicker.Close();
            }

            if(!hasExtractionStarted) {
                if(ImGui::Button("Extract")) {
                    extr = new bakermaker::ThreadedExtractor("PortableGit.zip", folderOutput, true);
                    hasExtractionStarted = true;
                    extr->start();
                }
            }

            else if(extr != nullptr){
                if(!extr->isFinished()) {
                    ImGui::Text("Extracting: %i / %i", extr->getFinishedFiles(), extr->getTotalFiles());
                    ImGui::BufferingBar("##extraction_progress",
                                        (((float) extr->getFinishedFiles()) / extr->getTotalFiles()),
                                        ImVec2(100, 5), bg, fg);
                }

                else {
                    extr->join();
                    delete extr;
                    extr = nullptr;
                }
            }
        }
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }


    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}