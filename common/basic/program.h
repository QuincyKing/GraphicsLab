#ifndef PROGRAM_H
#define PROGRAM_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <windows.h>
#include <basic/delegate.h>
#include <vector>
#include <iostream>

class Program
{
private:
	struct ProInfo
	{
		std::string title;
		unsigned int Width;
		unsigned int Height;
		unsigned int posX;
		unsigned int posY;
	}proInfo;

public:
	std::vector<InitEvent> initEvents;
	std::vector<RenderEvent> renderEvents;
	std::vector<DisableEvent> disableEvents;
	std::vector<KeyEvent> keyEvents;
	MouseEvent mouseEvent = NULL;
	ScrollEvent scrollEvent = NULL;
	FramebufferSizeEvent framebufferSizeEvent = NULL;
	
	float deltaTime;
	float lastFrame;

public:
	Program(unsigned int _Width, unsigned int _Height, unsigned int _posX = 0, unsigned int _posY = 0, std::string _title = "")
	{
		proInfo.Height = _Height;
		proInfo.Width = _Width;
		proInfo.title = _title;
		proInfo.posX = _posX;
		proInfo.posY = _posY;

		keyEvents.push_back(keyEvent);
	}

	int Run()
	{
		if (!glfwInit())
		{
			std::cerr << "Failed to initialize GLFW" << std::endl;
			return -1;
		}

		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

		GLFWwindow* window = glfwCreateWindow(proInfo.Width, proInfo.Height, proInfo.title.c_str(), nullptr, nullptr);
		if (window == NULL)
		{
			std::cerr << "Failed to create GLFW Triangle" << std::endl;
			glfwTerminate();
			return -2;
		}
		glfwSetWindowPos(window, proInfo.posX, proInfo.posY);
		glfwMakeContextCurrent(window);

		//注册事件函数
		glfwSetCursorPosCallback(window, mouseEvent);
		glfwSetScrollCallback(window, scrollEvent);
		glfwSetFramebufferSizeCallback(window, framebufferSizeEvent);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -3;
		}

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
		
		//初始化函数
		if (!initEvents.empty())
		{
			for(auto fun : initEvents)
				fun();
		}

		glViewport(0, 0, proInfo.Width, proInfo.Height);

		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			float lastFrame = currentFrame;

			glfwPollEvents();

			if (!keyEvents.empty())
			{
				for (auto fun : keyEvents)
				{
					fun(window);
				}
			}

			//渲染函数
			if (!renderEvents.empty())
			{
				for (auto fun : renderEvents)
					fun();
			}

			glfwSwapBuffers(window);
		}
		//关闭函数
		if (!disableEvents.empty())
		{
			for (auto fun : disableEvents)
				fun();
		}

		glfwTerminate();

		return 0;
	}

	static void keyEvent(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

	~Program() {}
};

#endif
