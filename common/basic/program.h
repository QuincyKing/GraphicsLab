#ifndef PROGRAM_H
#define PROGRAM_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
		int width;
		int height;
		int posX;
		int posY;
	}proInfo;

	std::vector<InitEvent> initEvents;
	std::vector<RenderEvent> renderEvents;
	std::vector<DisableEvent> disableEvents;
	std::vector<KeyEvent> keyEvents;
	std::vector<GUIEvent> guiEvents;
	MouseEvent mouseEvent = NULL;
	ScrollEvent scrollEvent = NULL;
	FramebufferSizeEvent framebufferSizeEvent = NULL;
	
public:
	int getScreenWidth() { return proInfo.width; }
	int getScreenHeight() { return  proInfo.height; }

	float deltaTime;
	float lastFrame;

public:
	Program(unsigned int _Width, unsigned int _Height, unsigned int _posX = 0, unsigned int _posY = 0, std::string _title = "")
	{
		proInfo.height = _Height;
		proInfo.width = _Width;
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

		GLFWwindow* window = glfwCreateWindow(proInfo.width, proInfo.height, proInfo.title.c_str(), nullptr, nullptr);
		if (window == NULL)
		{
			std::cerr << "Failed to create GLFW Triangle" << std::endl;
			glfwTerminate();
			return -2;
		}
		glfwSetWindowPos(window, proInfo.posX, proInfo.posY);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync
		//注册事件函数
		glfwSetCursorPosCallback(window, mouseEvent);
		glfwSetScrollCallback(window, scrollEvent);
		glfwSetFramebufferSizeCallback(window, framebufferSizeEvent);

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		//if (!glewLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		//{
		//	std::cout << "Failed to initialize GLAD" << std::endl;
		//	return -3;
		//}

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		//glfwWindowHint(GLFW_SAMPLES, 4);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif
		const char* glsl_version = "#version 130";

		glewInit();
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		// Setup style
		ImGui::StyleColorsDark();

		//初始化函数
		if (!initEvents.empty())
		{
			for(auto fun : initEvents)
				fun();
		}
		
		while (!glfwWindowShouldClose(window))
		{
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			float lastFrame = currentFrame;

			glfwPollEvents();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			//按键事件
			if (!keyEvents.empty())
			{
				for (auto fun : keyEvents)
				{
					fun(window);
				}
			}
			
			//GUI事件
			if (!guiEvents.empty())
			{
				for (auto fun : guiEvents)
				{
					fun();
				}
			}
			glfwMakeContextCurrent(window);
			glfwGetFramebufferSize(window, &proInfo.width, &proInfo.height);
			glViewport(0, 0, proInfo.width, proInfo.height);
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//渲染事件
			if (!renderEvents.empty())
			{
				for (auto fun : renderEvents)
					fun();
			}
			 
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwMakeContextCurrent(window);
			glfwSwapBuffers(window);
		}
		//关闭函数
		if (!disableEvents.empty())
		{
			for (auto fun : disableEvents)
				fun();
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
		glfwDestroyWindow(window);
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

	void RegisterGUI(GUIEvent _ge)
	{
		guiEvents.push_back(_ge);
	}

	void UnRegisterGUI(GUIEvent _ge)
	{
		auto result = find(guiEvents.begin(), guiEvents.end(), _ge);

		if (result != guiEvents.end())
			guiEvents.erase(result);
	}

	void UnRegisterAllGUI()
	{
		guiEvents.clear();
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
