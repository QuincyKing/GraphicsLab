#pragma  once

#include <vector>
#include <basic/shader.h>
#include <memory>
#include <basic/camera.h>
#include <stb_image.h>
#include <string>
#include <basic/program.h>

#include "water.h"

class WaterRenderer 
{
public:
	static float WAVE_PERIOD;

private:
	std::shared_ptr<Shader> s;
	float waveTime;
	Water water;

public:
	WaterRenderer() = default;

	WaterRenderer(glm::mat4 projectionMatrix, Water &w) 
	{
		waveTime = 0;
		this->water = w;
		s = std::make_shared<Shader>("./shader/waterVertex.glsl", "./shader/waterFragment.glsl");
		glm::vec3 lightDirection = glm::vec3(0.4f, -1.0f, 0.2f);
		glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, glm::vec3(water.x, water.height, water.z));
		
		this->s->use();
		this->s->setMat4("projectionMatrix", projectionMatrix);
		this->s->setMat4("model", model);
		this->s->setVec3("lightDirection", lightDirection);
		this->s->setVec3("lightColour", lightColor);
		this->s->setFloat("worldRadius", 10.0);
		this->s->setFloat("fadeOutPeriod", 10.0);
		this->s->setFloat("squareSize", 1.0f);
		this->s->setFloat("amplitude", Water::AMPLITUDE);
		this->s->setVec2("worldCenter", glm::vec2(0.0f, 0.0f));
		this->s->setVec2("mistValues", glm::vec2(20, 120));
		this->s->setVec3("mistColour", glm::vec3(1.0, 222.0 / 255.0, 210.0 / 255.0));
		this->s->setFloat("waterHeight", water.height);
	} 

	void render(Camera &camera) 
	{
		prepare(camera);
		draw();
		unbind();
	}

	void unbind()
	{
		glBindVertexArray(0);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_MULTISAMPLE);
		glDepthMask(true);
	}

private:
	void prepare(Camera &camera)
	{
		glCullFace(GL_BACK);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(false);

		this->s->use();
		this->s->setMat4("viewMatrix", camera.GetViewMatrix());
		this->s->setVec3("cameraPosition", camera.Position);
		updateWaveTime();
	}

	void updateWaveTime() 
	{
		waveTime += Program::deltaTime;
		if(waveTime > WaterRenderer::WAVE_PERIOD ) waveTime -= WaterRenderer::WAVE_PERIOD;
		this->s->use();
		this->s->setFloat("waveTime", waveTime / WaterRenderer::WAVE_PERIOD);
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

float WaterRenderer::WAVE_PERIOD = 3.5f;