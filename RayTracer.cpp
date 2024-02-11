#include "RayTracer.h"
#include <iostream>

using namespace CameraConstants;

RayTracer::RayTracer(const Matrix44f& camToW) : m_camToWorld{ camToW }
{
	m_camToWorld.multVecMatrix(m_orig, m_orig);
	m_zBuffer = std::vector<float>(imageWidth * imageHeight);
	m_frameBuffer = std::vector<Vec3<unsigned char>>(imageWidth * imageHeight);

	for (int i = 0; i < imageHeight * imageWidth; ++i)
		m_zBuffer[i] = std::numeric_limits<float>::max();
}

Vec3f RayTracer::getRayScreen(int px, int py)
{
	float x = (px + 0.5f) / imageWidth;
	float y = (py + 0.5f) / imageHeight;

	x = (2 * x - 1) * tanAOV * deviceAspectRatio;
	y = (1 - 2 * y) * tanAOV;

	Vec3f dir{ x,y,-1 };
	dir.normalize();

	return dir;
}

Vec3f RayTracer::generateRayDir(const Vec3f& vScreen)
{

	Vec3f dirWorld;
	m_camToWorld.multVecMatrix(-vScreen, dirWorld);
	dirWorld = dirWorld - m_orig;

	return dirWorld;

}

bool RayTracer::rayObjectIntersect(const Object3D& object, const Vec3f& rayO, const Vec3f& rayD, float& t)
{
	if (object.intersect(rayO, rayD, t))
		return true;
	else
		return false;
}

void RayTracer::render(const Object3D& object)
{
	Vec3f rayDir;

	for (int x = 0; x < imageWidth; ++x)
		for (int y = 0; y < imageHeight; ++y)
		{
			rayDir = getRayScreen(x, y);
			rayDir = generateRayDir(rayDir);
			//std::cout << m_orig << '\n';
			float t = INFINITY;

			if (rayObjectIntersect(object, m_orig, rayDir, t))
			{
				m_frameBuffer[y * imageWidth + x].x = 0;
				m_frameBuffer[y * imageWidth + x].y = 0;
				m_frameBuffer[y * imageWidth + x].z = 250;
			}

		}
}

std::vector<Vec3<unsigned char>> RayTracer::getFrameBuffer() const
{
	return m_frameBuffer;
}

void RayTracer::rayO()
{
	std::cout << m_orig << '\n';
}

