#pragma once

#include "waterframebuffer.h"
#include "water.h"
#include <memory>
#include <basic/shader.h>
#include <basic/camera.h>
#include <basic/program.h>

class WaterHdRenderer 
{
private:
	static int REFLECTION_TEX_UNIT;
	static int DEPTH_TEX_UNIT;

	static float WAVE_PERIOD;

	WaterFrameBuffers fbos;
	std::shared_ptr<Shader> shader;
	float waveTime = 0;
	Water water;

public:
	WaterHdRenderer() = default;

	WaterHdRenderer(glm::mat4 projectionMatrix, Water &w, WaterFrameBuffers &fbos, float N, float F)
	{
		this->fbos = fbos;
		water = w;
		shader = std::make_shared<Shader>("./shader/waterHdVertex.glsl", "./shader/waterHdFragment.glsl");
		glm::vec3 lightDirection = glm::vec3(0.4f, -1.0f, 0.2f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, glm::vec3(water.x, water.height, water.z));

		this->shader->use();
		this->shader->setMat4("projectionMatrix", projectionMatrix);
		this->shader->setMat4("model", model);
		this->shader->setFloat("amplitude", Water::AMPLITUDE);
		this->shader->setFloat("worldRadius", 10.0);
		this->shader->setFloat("fadeOutPeriod", 10.0);
		this->shader->setVec2("worldCenter", glm::vec2(0.0f, 0.0f));
		this->shader->setVec2("frustumNearFar", glm::vec2(N, F));
		this->shader->setVec2("mistValues", glm::vec2(20, 120));
		this->shader->setVec3("mistColour", glm::vec3(1.0, 222.0 / 255.0, 210.0 / 255.0));
		this->shader->setVec3("skyColour", glm::vec3(1, 0.87f, 0.6f));
		this->shader->setFloat("waterHeight", water.height);
		this->shader->setVec3("lightDirection", lightDirection);
		this->shader->setVec3("lightColour", lightColor);
		this->shader->setInt("reflectionTexture", 0);
		this->shader->setInt("depthTexture", 1);
	}

	void render(Camera &camera) 
	{
		prepare(camera);
		draw();
		stopRendering();
	}

private:
	void prepare(Camera &camera)
	{
		glCullFace(GL_BACK);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(false);
		
		this->shader->use();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbos.reflectionTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fbos.refractionDepthTexture);

		this->shader->setMat4("viewMatrix", camera.GetViewMatrix());
		this->shader->setVec3("cameraPosition", camera.Position);

		updateWaveTime();
	}

	void stopRendering() 
	{
		glDisable(GL_BLEND);
		glBindVertexArray(0);
		glDepthMask(true);
	}

	void updateWaveTime()
	{
		waveTime += Program::deltaTime;
		if (waveTime > WaterHdRenderer::WAVE_PERIOD) waveTime -= WaterHdRenderer::WAVE_PERIOD;
		shader->use();
		shader->setFloat("waveTime", waveTime / WaterHdRenderer::WAVE_PERIOD);
	}

	void draw()
	{
		if (water.vao == 0)
		{
			std::vector<float> vertexData = WaterGenerator::generate(water.pointCount - 1, 0);
			water.vertexCount = vertexData.size() / WaterGenerator::VERTEX_SIZE_BYTES;

			glGenVertexArrays(1, &water.vao);
			glBindVertexArray(water.vao);

			unsigned int vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);
			float stride = 6 * sizeof(float);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));

		}
		glBindVertexArray(water.vao);
		glDrawArrays(GL_TRIANGLES, 0, water.vertexCount);
	}
};

float WaterHdRenderer::WAVE_PERIOD = 3.9f;