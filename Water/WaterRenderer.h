#pragma  once

#include <vector>
#include <basic/shader.h>
#include <memory>
#include <basic/camera.h>
#include <stb_image.h>
#include <string>

#include "WaterTile.h"
#include "waterframebuffer.h"

class WaterRenderer 
{
public:
	static const float MoveSpeed;

private:
	std::shared_ptr<Shader> s;
	WaterFrameBuffers fbo;
	unsigned int quadVAO;
	unsigned int quadVBO;
	unsigned int dudvMap;
	unsigned int normalMap;
	float moveFactor;

public:
	WaterRenderer() = default;

	WaterRenderer(std::shared_ptr<Shader> &shader, glm::mat4 projectionMatrix, WaterFrameBuffers &fbo) 
	{
		moveFactor = 0.0f;
		quadVAO = 0;
		glm::vec3 lightPosition = glm::vec3(0.0f, 5.0f, -1.0f);
		glm::vec3 lightColor = glm::vec3(0.1f, 0.1f, 0.1f);
		this->fbo = fbo;
		this->s = shader;
		this->s->use();
		this->s->setMat4("projection", projectionMatrix);
		this->s->setInt("reflectionTexture", 0);
		this->s->setInt("refractionTexture", 1);
		this->s->setInt("dudvMap", 2);
		this->s->setInt("normalMap", 3);
		this->s->setInt("depthMap", 4);
		this->s->setVec3("lightPos", lightPosition);
		this->s->setVec3("lightColor", lightColor);

		dudvMap = LoadTexture("../resources/textures/waterDUDV.png");
		normalMap = LoadTexture("../resources/textures/normal.png");
	}

	void render(std::vector<WaterTile> &water, Camera &camera) 
	{
		prepare();
		this->s->use();
		glm::mat4 view = camera.GetViewMatrix();
		this->s->setMat4("view", view);
		this->s->setVec3("cameraPos", camera.Position);

		for (WaterTile tile : water) 
		{
			glm::mat4 model = glm::mat4();
			model = glm::translate(model, glm::vec3(tile.getX(), tile.getHeight(), tile.getZ()));
			this->s->use();
			this->s->setMat4("model", model);
			renderquad();
		}
		unbind();
	}

	void unbind()
	{
		glDisable(GL_BLEND);
		glDisableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	void renderquad()
	{
		if (quadVAO == 0)
		{
			float quadVertices[] = 
			{
				-5.0f, 5.0f, 0.0f, 0.0f, 1.0f,
				-5.0f, -5.0f, 0.0f, 0.0f, 0.0f,
				5.0f, 5.0f, 0.0f, 1.0f, 1.0f,
				5.0f, -5.0f, 0.0f, 1.0f, 0.0f,
			};
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}

private:
	void prepare()
	{
		moveFactor += MoveSpeed * Program::deltaTime;
		if(moveFactor > 1.0f) moveFactor -= 1.0;

		this->s->use();
		this->s->setFloat("moveFactor", moveFactor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fbo.reflectionTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, fbo.refractionTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, dudvMap);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, normalMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, fbo.refractionDepthTexture);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	unsigned int LoadTexture(const std::string _path)
	{
		const char *path = _path.c_str();
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}
};

const float WaterRenderer::MoveSpeed = 0.2f;