#pragma once

#include <glm/glm.hpp>
#include <basic/camera.h>
#include <functional>
#include "waterframebuffer.h"
#include "waterrender.h"
#include "water.h"
#include "waterhdrender.h"

#define OPTION true

class WaterMasterRenderer 
{
private:
	static float REFRACTION_BIAS;

	WaterFrameBuffers waterFbos;
	Water water;

	WaterRenderer waterRenderer;
	WaterHdRenderer waterHdRenderer;

	Camera* camera;
	
public:
	WaterMasterRenderer() = default;

	WaterMasterRenderer(Camera *camera, Water &w)
	{
		this->camera = camera;
		this->water = w;
		waterFbos.init();
		glm::mat4 projection = 
			glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		waterHdRenderer = WaterHdRenderer(projection, water, waterFbos, 0.1f, 100.0f);
		waterRenderer = WaterRenderer(projection, water);
	}

	void renderWater(const std::function<void()> &refractionEvent = {}, const std::function<void()> &reflectEvent = {})
	{
		if (OPTION && refractionEvent != nullptr && reflectEvent != nullptr)
		{
			updateWaterTextures(refractionEvent, reflectEvent);
			waterHdRenderer.render(*camera);
		}
		else
			waterRenderer.render(*camera);
	}

	void updateWaterTextures(const std::function<void()> &refractionEvent, const std::function<void()> &reflectEvent)
	{
		if (OPTION)
		{
			glEnable(GL_CLIP_DISTANCE0);
			waterFbos.bindRefractionFrameBuffer();
			glClear(GL_DEPTH_BUFFER_BIT);
			glDisable(GL_BLEND);
			glCullFace(GL_BACK);
			glEnable(GL_DEPTH_TEST);
			refractionEvent();
			waterFbos.unbindCurrentFrameBuffer();
			
			float distance = 2 * (camera->Position.y - water.height);
			camera->Position.y -= distance;
			camera->Pitch = -camera->Pitch;
			waterFbos.bindReflectionFrameBuffer();
			glClearColor(0.0, 0.3, 0.5, 1.0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDisable(GL_BLEND);
			glCullFace(GL_BACK);
			glEnable(GL_DEPTH_TEST);
			reflectEvent();
			waterFbos.unbindCurrentFrameBuffer();
			camera->Position.y += distance;
			camera->Pitch = -camera->Pitch;

			glDisable(GL_CLIP_DISTANCE0);
		}
	}

	void cleanUp() 
	{
		waterFbos.cleanUp();
	}
};

float WaterMasterRenderer::REFRACTION_BIAS = 0.3f;
