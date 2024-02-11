#pragma once
#include <vector>
#include "Object3D.h"
#include "CameraConstants.h"

class RayTracer
{
private:
	Matrix44f m_camToWorld;
	Vec3f m_orig;
	Vec3f getRayScreen(int px, int py);
	std::vector<float> m_zBuffer;
	std::vector<Vec3<unsigned char>> m_frameBuffer;

public:
	RayTracer(const Matrix44f& camToW = Matrix44f{});

	Vec3f generateRayDir(const Vec3f& vScreen);
	bool rayObjectIntersect(const Object3D& object, const Vec3f& rayO, const Vec3f& rayD, float& t);

	void render(const Object3D& object);

	std::vector<Vec3<unsigned char>> getFrameBuffer() const;

	void rayO();
};
