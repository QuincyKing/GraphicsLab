#pragma once

#include "water.h"

#include <vector>
#include <glm/glm.hpp>

class WaterMeshGenerator 
{
private:
	static int waterPoint;

	/**
	 * Generates the {@code float[]} of vertex data which will be loaded to the
	 * water's VAO. The array contains the vertex x and z positions as well as
	 * an encoded indication of which corner of its triangle each vertex lies
	 * in.
	 *
	 * @return The {@code float[]} of vertex data to be loaded to a VAO.
	 */
public:
	static std::vector<float> generateVertices(HeightFinder heightFinder,int waterpoint, float height) 
	{
		WaterMeshGenerator::waterPoint = waterpoint;
		std::vector<float> vertices;
		for (int col = 0; col < waterPoint - 1; col++) 
			for (int row = 0; row < waterPoint - 1; row++)
				generateWaterQuad(col, row, vertices);

		std::vector<float> overgroundVertices = removeUndergroundVertices(vertices, heightFinder, height);
		return overgroundVertices;
	}

private:
	static void generateWaterQuad(int col, int row, std::vector<float> &vertices) 
	{
		int topLeft = (row * waterPoint) + col;
		int topRight = topLeft + 1;
		int bottomLeft = ((row + 1) * waterPoint) + col;
		int bottomRight = bottomLeft + 1;
		if (row % 2 == 0)
			storeQuad1(vertices, topLeft, topRight, bottomLeft, bottomRight, col % 2 == 0);
		else
			storeQuad2(vertices, topLeft, topRight, bottomLeft, bottomRight, col % 2 == 0);
	}

	static void storeQuad1(std::vector<float> &vertices, int topLeft, int topRight, int bottomLeft, int bottomRight, bool mixed)
	{
		storeVertex(vertices, topLeft, glm::vec2(0, 1), mixed ? glm::vec2(1, 0) : glm::vec2(1, 1));
		storeVertex(vertices, bottomLeft, mixed ? glm::vec2(1, -1) : glm::vec2(1, 0), glm::vec2(0, -1));
		if (mixed) 
			storeVertex(vertices, topRight, glm::vec2(-1, 0), glm::vec2(-1, 1));
		else
			storeVertex(vertices, bottomRight, glm::vec2(-1, -1), glm::vec2(-1, 0));
		 
		storeVertex(vertices, bottomRight, glm::vec2(0, -1), mixed ? glm::vec2(-1, 0) : glm::vec2(-1, -1));
		storeVertex(vertices, topRight, mixed ? glm::vec2(-1, 1) : glm::vec2(-1, 0), glm::vec2(0, 1));
		if (mixed)
			storeVertex(vertices, bottomLeft, glm::vec2(1, 0), glm::vec2(1, -1));
		else
			storeVertex(vertices, topLeft, glm::vec2(1, 1), glm::vec2(1, 0));
	}

	static void storeQuad2(std::vector<float> &vertices, int topLeft, int topRight, int bottomLeft, int bottomRight, bool mixed)
	{
		storeVertex(vertices, topRight, glm::vec2(-1, 0), mixed ? glm::vec2(0, 1) : glm::vec2(-1, 1));
		storeVertex(vertices, topLeft, mixed ? glm::vec2(1, 1) : glm::vec2(0, 1), glm::vec2(1, 0));
		if (mixed)
			storeVertex(vertices, bottomRight, glm::vec2(0, -1), glm::vec2(-1, -1));
		else
			storeVertex(vertices, bottomLeft, glm::vec2(1, -1), glm::vec2(0, -1));
		
		storeVertex(vertices, bottomLeft, glm::vec2(1, 0), mixed ? glm::vec2(0, -1) : glm::vec2(1, -1));
		storeVertex(vertices, bottomRight, mixed ? glm::vec2(-1, -1) : glm::vec2(0, -1), glm::vec2(-1, 0));
		if (mixed)
			storeVertex(vertices, topLeft, glm::vec2(0, 1), glm::vec2(1, 1));
		else
			storeVertex(vertices, topRight, glm::vec2(-1, 1), glm::vec2(0, 1));
	}

	static void storeVertex(std::vector<float> &vertices, int index, glm::vec2 otherPoint1, glm::vec2 otherPoint2)
	{
		int gridX = index % waterPoint;
		int gridZ = index / waterPoint;
		float x = gridX * Water::SQUARE_SIZE;
		float z = gridZ * Water::SQUARE_SIZE;
		vertices.push_back(x);
		vertices.push_back(z);
		vertices.push_back(encode(otherPoint1.x, otherPoint1.y, otherPoint2.x, otherPoint2.y));
	}

	static float encode(float x, float z, float x2, float z2) 
	{
		float p3 = (x + 1) * 27;
		float p2 = (z + 1) * 9;
		float p1 = (x2 + 1) * 3;
		float p0 = (z2 + 1) * 1;
		return p0 + p1 + p2 + p3;
	}

	static std::vector<float> removeUndergroundVertices(std::vector<float> &vertices, HeightFinder heightFinder, float waterHeight)
	{
		std::vector<float> overgroundVertices;
		for (int i = 0; i < vertices.size(); i += 9) 
		{
			glm::vec3 point1 = extractVertex(vertices, i);
			glm::vec3 point2 = extractVertex(vertices, i + 3);
			glm::vec3 point3 = extractVertex(vertices, i + 6);
			bool triangleUnder = isUnderGround(point1, heightFinder, waterHeight)
				&& isUnderGround(point2, heightFinder, waterHeight)
				&& isUnderGround(point3, heightFinder, waterHeight);
			if (!triangleUnder) 
			{
				addVertexToList(overgroundVertices, point1);
				addVertexToList(overgroundVertices, point2);
				addVertexToList(overgroundVertices, point3);
			}
		}
		return overgroundVertices;
	}

	static void addVertexToList(std::vector<float> &vertices, glm::vec3 point) 
	{
		vertices.push_back(point.x);
		vertices.push_back(point.y);
		vertices.push_back(point.z);
	}

	static glm::vec3 extractVertex(std::vector<float> &vertices, int pointer)
	{
		float x = vertices[pointer];
		float z = vertices[pointer + 1];
		float code = vertices[pointer + 2];
		return glm::vec3(x, z, code);
	}

	static bool isUnderGround(glm::vec3 point, HeightFinder heightFinder, float waterHeight)
	{
		float terrainHeight = heightFinder.getHeight(point.x, point.y);
		return terrainHeight > waterHeight + Water::AMPLITUDE;
	}
};

int WaterMeshGenerator::waterPoint = 1;