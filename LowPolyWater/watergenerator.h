#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "water.h"

class WaterGenerator 
{
public:
	static int VERTICES_PER_SQUARE;// 2 triangles
	static int VERTEX_SIZE_BYTES;// x,z position + indicator

public:
	static std::vector<float> generate(int gridCount, float waterHeight)
	{
		int totalVertexCount = gridCount * gridCount * VERTICES_PER_SQUARE;
		std::vector<float> data;
		for (int row = 0; row < gridCount; row++)
			for (int col = 0; col < gridCount; col++)
			{
				std::vector<glm::vec2> cornerPos = calculateCornerPositions(col, row);
				bool even = (col + row) % 2 == 0;
				storeTriangle(cornerPos, true, even, waterHeight, data);
				storeTriangle(cornerPos, false, even, waterHeight, data);
			}

		return data;
	}

private:
	static std::vector<glm::vec2> calculateCornerPositions(int col, int row)
	{
		std::vector<glm::vec2> vertices;
		vertices.push_back(glm::vec2(col, row));
		vertices.push_back(glm::vec2(col, row + 1));
		vertices.push_back(glm::vec2(col + 1, row));
		vertices.push_back(glm::vec2(col + 1, row + 1));
		return vertices;
	}

	static void storeTriangle(std::vector<glm::vec2> &cornerPos, bool left, bool normalSquare, float waterHeight, std::vector<float> &data)
	{
		int index0 = left ? 0 : 2;
		int index1 = (!normalSquare && !left) ? 0 : 1;
		int index2 = (normalSquare && left) ? 2 : 3;
		if (!isUnderGround(cornerPos[index0], cornerPos[index1], cornerPos[index2], waterHeight))
		{
			glm::vec2 v0 = cornerPos[index0];
			glm::vec2 v1 = cornerPos[index1];
			glm::vec2 v2 = cornerPos[index2];
			storeVertex(v0, v1, v2, data);
			storeVertex(v1, v2, v0, data);
			storeVertex(v2, v0, v1, data);
		}
	}

	static bool isUnderGround(glm::vec2 vert1, glm::vec2 vert2, glm::vec2 vert3, float waterHeight) 
	{
		return isVertexUnderGround(vert1, waterHeight)
			&& isVertexUnderGround(vert2, waterHeight)
			&& isVertexUnderGround(vert3, waterHeight);
	}

	static bool isVertexUnderGround(glm::vec2 point, float waterHeight)
	{
		//float terrainHeight = heightFinder.getHeight(point.x, point.y);
		float terrainHeight = 100000000.0f;
		return false;
	}

	static void storeVertex(glm::vec2 currentVertex, glm::vec2 vertex1, glm::vec2 vertex2, std::vector<float> &data)
	{
		data.push_back(currentVertex.x);
		data.push_back(currentVertex.y);
		
		glm::vec2 offset1 = vertex1 - currentVertex;
		glm::vec2 offset2 = vertex2 - currentVertex;
		data.push_back(offset1.x); data.push_back(offset1.y);
		data.push_back(offset2.x); data.push_back(offset2.y);
	}
};

int WaterGenerator::VERTICES_PER_SQUARE = 3 * 2;
int WaterGenerator::VERTEX_SIZE_BYTES = 6;