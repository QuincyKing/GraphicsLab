
#include <basic/camera.h>

#include <basic/shader.h>

#include <basic/program.h>

#include <stb_image.h>

#include <iostream>
#include <memory>

#include "WaterRenderer.h"
#include "waterframebuffer.h"

using namespace std;

void OnFramebufferSize(GLFWwindow* window, int width, int height);
void OnMouse(GLFWwindow* window, double xpos, double ypos);
void OnScroll(GLFWwindow* window, double xoffset, double yoffset);
void OnKey(GLFWwindow *window);
void OnCursorPos(GLFWwindow*, double, double);
void OnRender();
void OnInit();
void OnDisable();
unsigned int LoadTexture(const string path);
void RenderSphere();
void RenderCube();

#define SCR_WIDTH 1000
#define SCR_HEIGHT 800

Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
std::shared_ptr<Shader> shader;
std::shared_ptr<Shader> waterShader;
std::shared_ptr<Program> pro;
unsigned int albedo, normal, metallic, roughness, ao;
WaterRenderer wr;
WaterTile tile(0.0f, 0.0f, 0.0f);
std::vector<WaterTile> tiles;
glm::vec2 curPos(0, 0);
WaterFrameBuffers wf;

int main()
{
	pro = make_shared<Program>(SCR_WIDTH, SCR_HEIGHT, 100, 100, "water");

	pro->RegisterFramebuffer(OnFramebufferSize);
	pro->RegisterMouse(OnMouse);
	pro->RegisterScroll(OnScroll);
	pro->RegisterKey(OnKey);
	pro->RegisterInit(OnInit);
	pro->RegisterRender(OnRender);
	pro->RegisterDisable(OnDisable);
	
	pro->Run();
	return 0;
}

void OnInit()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	shader = make_shared<Shader>("./shader/pbr.vs", "./shader/pbr.fs");
	waterShader = make_shared<Shader>("./shader/water.vs", "./shader/water.fs");
	shader->use();

	shader->setInt("albedoMap", 0);
	shader->setInt("normalMap", 1);
	shader->setInt("metallicMap", 2);
	shader->setInt("roughnessMap", 3);
	shader->setInt("aoMap", 4);

	albedo = LoadTexture("../resources/textures/pbr/plastic/albedo.png");
	normal = LoadTexture("../resources/textures/pbr/plastic/normal.png");
	metallic = LoadTexture("../resources/textures/pbr/plastic/metallic.png");
	roughness = LoadTexture("../resources/textures/pbr/plastic/roughness.png");
	ao = LoadTexture("../resources/textures/pbr/plastic/ao.png");

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader->use();
	shader->setMat4("model", model);
	shader->setMat4("projection", projection);

	wf.init();
	wr = WaterRenderer(waterShader, projection, wf);
	tiles.push_back(tile);
}

void OnRender()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 lightColor = glm::vec3(255.0f, 255.0f, 255.0f);
	shader->use();
	shader->setVec3("camPos", camera.Position);
	shader->setVec3("lightPos", lightPosition + glm::vec3(curPos, 0.0));
	shader->setVec3("lightColor", lightColor);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, albedo);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, metallic);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, roughness);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, ao);

	glEnable(GL_CLIP_DISTANCE0);

	wf.bindReflectionFrameBuffer();
	float distance = 2 * (camera.Position.y - tile.getHeight());
	camera.Position.y -= distance;
	camera.Pitch = -camera.Pitch;
	glm::mat4 view = camera.GetViewMatrix();
	shader->use();
	shader->setVec4("plane", glm::vec4(0.0f, 1.0f, 0.0f, -tile.getHeight()));
	shader->setMat4("view", view);
	RenderSphere();

	camera.Position.y += distance;
	camera.Pitch = -camera.Pitch;
	wf.bindRefractionFrameBuffer();
	shader->use();
	shader->setVec4("plane", glm::vec4(0.0f, -1.0f, 0.0f, tile.getHeight()));
	view = camera.GetViewMatrix();
	shader->setMat4("view", view);
	RenderSphere();

	wf.unbindCurrentFrameBuffer();

	glDisable(GL_CLIP_DISTANCE0);

	shader->use();
	RenderSphere();
	wr.render(tiles, camera);
}

void OnDisable()
{
	wf.cleanUp();
}

float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

void OnMouse(GLFWwindow* window, double xpos, double ypos)
{
	curPos.x = 10 * float((xpos - SCR_WIDTH / 2) / SCR_WIDTH);
	curPos.y = 10 * float(0 - (ypos - SCR_HEIGHT / 2) / SCR_HEIGHT);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

void OnKey(GLFWwindow *window)
{
	float cameraSpeed = 2.5 * 0.01f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, 0.01f);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, 0.01f);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, 0.01f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, 0.01f);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessMouseMovement(0.0, 5.0);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessMouseMovement(0.0, -5.0);
}

void OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

unsigned int sphereVAO = 0;
unsigned int indexCount;
void RenderSphere()
{
	if (sphereVAO == 0)
	{
		glGenVertexArrays(1, &sphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y       * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

unsigned int cubeVAO = 0, cubeVBO = 0;
void RenderCube()
{
	if (cubeVAO == 0 || cubeVBO == 0)
	{
		float cubeVertices[] = {
			// positions       
			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,

			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,

			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,

			-0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, -0.5f,
			0.5f, -0.5f, 0.5f,
			0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, 0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, -0.5f,
			0.5f, 0.5f, 0.5f,
			0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, 0.5f,
			-0.5f, 0.5f, -0.5f
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

unsigned int LoadTexture(const string _path)
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