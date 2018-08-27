#include <basic/camera.h>
#include <basic/shader.h>
#include <basic/program.h>

#include <iostream>
#include <memory>

using namespace std;

void OnFramebufferSize(GLFWwindow* window, int width, int height);
void OnMouse(GLFWwindow* window, double xpos, double ypos);
void OnScroll(GLFWwindow* window, double xoffset, double yoffset);
void OnKey(GLFWwindow *window);
void OnRender();
void OnInit();
void OnDisable();
void RenderSphere();

#define SCR_WIDTH 1000
#define SCR_HEIGHT 800

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
std::shared_ptr<Shader> shader;
std::shared_ptr<Program> pro;

int main()
{
	pro = make_shared<Program>(SCR_WIDTH, SCR_HEIGHT, 100, 100, "pbr");

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
	
	glm::vec3 albedo = glm::vec3(0.5f, 0.0f, 0.0f);
	float ao = 0.5f;

	shader = make_shared<Shader>("./shader/pbr.vs", "./shader/pbr.fs");
	shader->use();
	
	shader->setVec3("albedo", albedo);
	shader->setFloat("ao", ao);
	shader->setFloat("metallic", 0.5f);
	shader->setFloat("roughness", glm::clamp(0.5f, 0.5f, 1.0f));

	glm::mat4 model = glm::mat4();
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader->setMat4("model", model);
	shader->setMat4("projection", projection);
}

int i = 0;
void OnRender()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::vec3 lightPosition = glm::vec3(0.0f, 0.0f, 10.0f);
	glm::vec3 lightColor = glm::vec3(0.0f, 0.0f, 255.0f);
	shader->use();
	glm::mat4 view = camera.GetViewMatrix();
	shader->setMat4("view", view);
	shader->setVec3("camPos", camera.Position);
	shader->setVec3("lightPosition", lightPosition + glm::vec3(sin(0.01 * i * 3.1415926), 0, 0));

	i++;
	RenderSphere();
}

void OnDisable()
{

}

float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

void OnMouse(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
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