#include "UserInterface.hpp"

// PRIVATE
void UserInterface::initializeNewFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}


void UserInterface::configure()
{
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_MenuBar);
    {
		ImGui::Text("Application average %.1f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Once);
        ImGui::SetWindowSize(ImVec2((float)SCR_WIDTH*0.2f, (float)SCR_HEIGHT));
        ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Checkbox("SSAO", _ssao);
		if (*_ssao) {
			ImGui::SliderFloat("radius", _ssaoRadius, 0.1f, 5.0f);
		}

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SameLine();

		if (ImGui::RadioButton("Bloom", *_currentEffect == 1)) {
			if (*_currentEffect == 1) *_currentEffect = 0;
			else *_currentEffect = 1;
		}
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); 
		ImGui::SameLine();

		if (ImGui::RadioButton("Sobel", *_currentEffect == 2)) {
			if (*_currentEffect == 2) *_currentEffect = 0;
			else *_currentEffect = 2;
		}
		ImGui::SameLine();
		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::SameLine();

		if (ImGui::RadioButton("Pencil", *_currentEffect == 3)) {
			if (*_currentEffect == 3) *_currentEffect = 0;
			else *_currentEffect = 3;
		}

		if (ImGui::BeginMenuBar())
		{
			configureMenuBar();
			ImGui::EndMenuBar();
		}

    }
	ImGui::End();
}


void UserInterface::configureMenuBar()
{
    if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Open...", "Ctrl+O")) {}
		if (ImGui::MenuItem("Save", "Ctrl+S")) {}
		if (ImGui::MenuItem("Close", "Ctrl+W")) { glfwSetWindowShouldClose(glfwGetCurrentContext(), true); }
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Scale"))
	{
		if (ImGui::MenuItem("less")) {
			ImGuiStyle *style = &ImGui::GetStyle(); style->ScaleAllSizes(0.5f);
			ImGui::GetIO().FontGlobalScale = 1.0;
		}
		if (ImGui::MenuItem("more")) {
			ImGuiStyle *style = &ImGui::GetStyle(); style->ScaleAllSizes(2.0f);
			ImGui::GetIO().FontGlobalScale = 2.0;
		}
		ImGui::EndMenu();
	}
}

// PUBLIC
void UserInterface::init()
{
    // Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 460");
}


void UserInterface::update()
{
    initializeNewFrame();
	configure();
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void UserInterface::end()
{
    ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
