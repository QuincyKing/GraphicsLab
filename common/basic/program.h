#ifndef PROGRAM_H
#define PROGRAM_H

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>
#include <windows.h>
#include <basic/delegate.h>
#include <vector>
#include <iostream>
#include <algorithm>

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

	std::vector<InitEvent> initEvents;
	std::vector<RenderEvent> renderEvents;
	std::vector<DisableEvent> disableEvents;
	std::vector<KeyEvent> keyEvents;
	MouseEvent mouseEvent = NULL;
	ScrollEvent scrollEvent = NULL;
	FramebufferSizeEvent framebufferSizeEvent = NULL;
	
public:
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

		keyEvents.push_back(_keyEvent);
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
		//glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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

	static void _keyEvent(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
	}

#pragma region 事件的注册与注销
	void RegisterInit(InitEvent _ie)
	{
		initEvents.push_back(_ie);
	}

	void UnRegisterInit(InitEvent _ie)
	{
		auto result = find(initEvents.begin(), initEvents.end(), _ie);

		if(result != initEvents.end())
			initEvents.erase(result);
	}
	
	void UnRegisterAllInit()
	{
		initEvents.clear();
	}

	void RegisterRender(RenderEvent _re)
	{
		renderEvents.push_back(_re);
	}

	void UnRegisterRender(RenderEvent _re)
	{
		auto result = find(renderEvents.begin(), renderEvents.end(), _re);

		if(result != renderEvents.end())
			renderEvents.erase(result);
	}

	void UnRegisterAllRender()
	{
		renderEvents.clear();
	}

	void RegisterDisable(DisableEvent _de)
	{
		disableEvents.push_back(_de);
	}

	void UnRegisterDisable(DisableEvent _de)
	{
		auto result = find(disableEvents.begin(), disableEvents.end(), _de);

		if (result != disableEvents.end())
			disableEvents.erase(result);
	}

	void UnRegisterAllDisable()
	{
		disableEvents.clear();
	}

	void RegisterKey(KeyEvent _ke)
	{
		keyEvents.push_back(_ke);
	}

	void UnRegisterKey(KeyEvent _ke)
	{
		auto result = find(keyEvents.begin(), keyEvents.end(), _ke);

		if (result != keyEvents.end())
			keyEvents.erase(result);
	}

	void UnRegisterAllKey()
	{
		keyEvents.clear();
	}

	void RegisterMouse(MouseEvent _me)
	{
		mouseEvent = _me;
	}

	void UnRegisterMouse()
	{
		mouseEvent = NULL;
	}

	void RegisterScroll(ScrollEvent _se)
	{
		scrollEvent = _se;
	}

	void UnRegisterScroll()
	{
		scrollEvent = NULL;
	}

	void RegisterFramebuffer(FramebufferSizeEvent _fe)
	{
		framebufferSizeEvent = _fe;
	}

	void UnRegisterFramebuffer()
	{
		framebufferSizeEvent = NULL;
	}

#pragma endregion

	~Program() 
	{
		UnRegisterAllInit();
		UnRegisterAllRender();
		UnRegisterAllDisable();
		UnRegisterAllKey();
		UnRegisterFramebuffer();
		UnRegisterMouse();
		UnRegisterScroll();
	}
};

#endif
