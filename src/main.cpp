#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "GLFW/glfw3.h"
#include <iostream>
#include <vector>
#include <fstream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "imfilebrowser.h"
#include "improgress.h"

#include "ThreadedExtractor.h"

constexpr int FOLDER_PICKER_FLAGS = ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir |
        ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_HideRegularFiles |
        ImGuiFileBrowserFlags_ConfirmOnEnter;

void sshsteps(int step, string keyfile);

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
    const ImU32 bg = ImGui::GetColorU32(ImVec4(0.1, 0.1, 0.5, 1));

    bakermaker::ThreadedExtractor* extr = nullptr;

    std::vector<string> keys;
    for(const auto &entry: std::filesystem::directory_iterator("keys/")) {
        string filepath = entry.path().string();
        keys.push_back(filepath.substr(filepath.find_last_of('/') + 1));
    }

    int selectedkey = 0;
    int programstate = 0;
    int sshdone = 0;

    while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);

        if(ImGui::Begin("Bakermaker", &open, flags)) {
            switch(programstate) {
                case 0:

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

                    ImGui::TextUnformatted("Choose key");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(200);
                    if(ImGui::BeginCombo("##keypicker", keys[selectedkey].c_str())) {
                        for(size_t i = 0; i < keys.size(); i++) {
                            if(ImGui::Selectable(keys[i].c_str(), i == selectedkey)) selectedkey = i;
                        }
                        ImGui::EndCombo();

                    }

                    if(ImGui::Button("Start")) {
                        extr = new bakermaker::ThreadedExtractor("PortableGit.zip", folderOutput, true);
                        extr->start();
                        programstate++;
                    }

                    break;

                case 1:
                    ImGui::Text("Extracting Git to: %s", folderOutput.c_str());
                    ImGui::Text("Setting key as: %s", keys[selectedkey].c_str());
                    ImGui::NewLine();

                    if(!extr->isFinished()) {
                        ImGui::Text("Extracting: %i / %i", extr->getFinishedFiles(), extr->getTotalFiles());
                        ImGui::BufferingBar("##extraction_progress",
                                            (((float) extr->getFinishedFiles()) / extr->getTotalFiles()),
                                            ImVec2(100, 5), bg, fg);
                    } else {
                        ImGui::Text("Extracting: Done");
                        ImGui::BufferingBar("##extraction_progress", 1,
                                            ImVec2(100, 5), bg, fg);
                    }

                    ImGui::NewLine();

                    if(sshdone != 2) {
                        ImGui::Text("SSH Steps Complete: %i / 5", sshdone);
                        ImGui::BufferingBar("#sshsteps", (sshdone / 5.0),
                                            ImVec2(100, 5), bg, fg);
                        sshsteps(sshdone++, keys[selectedkey]);
                    }

                    else {
                        ImGui::Text("SSH Steps Complete");
                        ImGui::BufferingBar("#sshsteps", 1,
                                            ImVec2(100, 5), bg, fg);
                    }

                    if(extr->isFinished() && sshdone == 2) {
                        extr->join();
                        delete extr;
                        extr = nullptr;
                        programstate++;
                    }
                    break;

                case 2:
                    ImGui::Text("Extracting Git to: %s", folderOutput.c_str());
                    ImGui::Text("Setting key as: %s", keys[selectedkey].c_str());

                    ImGui::NewLine();

                    ImGui::Text("Extracting: Done");
                    ImGui::BufferingBar("##extraction_progress", 1,
                                        ImVec2(100, 5), bg, fg);

                    ImGui::NewLine();

                    ImGui::Text("SSH Steps Complete");
                    ImGui::BufferingBar("#sshsteps", 1,
                                        ImVec2(100, 5), bg, fg);
                    break;
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

void sshsteps(int step, string keyfile) {
    string path(getenv("HOMEDRIVE"));
    path.append(getenv("HOMEPATH"));

    switch(step) {
        case 0: {
            if(!std::filesystem::directory_entry(path + "\\.ssh").exists())
                std::filesystem::create_directories(path + "\\.ssh");
            return;
        }

        case 1: {
            std::stringstream sshconfig;
            sshconfig << "Host git\n\tHostName ";
            std::ifstream ip("ip");
            {
                string temp;
                ip >> temp;
                sshconfig << temp;
            }

            sshconfig << "\n\tUser git\n\tIdentityFile ~/.ssh/" << keyfile;
            std::ofstream config(path + "\\.sssh\\config");
            config << sshconfig.str();
        }

        case 2: {
            std::filesystem::copy("keys\\" + keyfile, path + "\\.ssh\\" + keyfile);
        }

        case 3: {
            std::stringstream gitconfig;
            gitconfig << "[user]\n\tname = " << keyfile << "\n";
            gitconfig << "\temail = " << keyfile;
            std::ofstream gitconfigfile(path + "\\.gitconfig");
            gitconfigfile << gitconfig.str();
        }

        default:
            return;
    }
}