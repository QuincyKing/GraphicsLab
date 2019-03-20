#pragma once

class WaterTile
{
public:
	static float TILE_SIZE;
	
private:
	float height;
	float x,z;
	
public:
	WaterTile(float centerX, float centerZ, float height)
	{
		this->x = centerX;
		this->z = centerZ;
		this->height = height;
	}

	float getHeight()
	{
		return height;
	}

	float getX() 
	{
		return x;
	}

	float getZ()
	{
		return z;
	}
};

float WaterTile::TILE_SIZE = 60;