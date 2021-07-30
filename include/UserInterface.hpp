#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class UserInterface
{
private:
	void initializeNewFrame();
	void configure();
	void configureMenuBar();

public:
    UserInterface(){}
    
	void init();
	void update();
	void end();

	unsigned int SCR_WIDTH = 1920;
	unsigned int SCR_HEIGHT = 1080;

	bool* _ssao;
	float* _ssaoRadius;

	int* _currentEffect;
};

#endif
