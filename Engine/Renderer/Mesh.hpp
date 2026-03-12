#pragma once
#include <vector> 
#include <Maths/Vector.hpp>


struct Face
{
	uint32_t index0 = 0, index1 = 0, index2 = 0;
};

struct Mesh
{
	void AddVertex(const Vector3f& position, const Vector2f& textureCoordinate, const Vector3f& normal)
	{
		positions.push_back(position);
		textureCoordinates.push_back(textureCoordinate);
		normals.push_back(normal);
	}

	void AddFace(uint32_t index0, uint32_t index1, uint32_t index2)
	{
		faces.push_back({ index0, index1, index2 });
	}

	std::vector<Vector3f> positions;
	std::vector<Vector2f> textureCoordinates;
	std::vector<Vector3f> normals;

	std::vector<Face> faces;
};