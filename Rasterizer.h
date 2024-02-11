#pragma once
#include <vector>
#include <memory>
#include "CameraConstants.h"

struct BoundingBox
{
	float xmax{};
	float xmin{};
	float ymax{};
	float ymin{};
};


class Rasterizer
{
private:

	struct CanvasSettings
	{
		float canvasW ;
		float canvasH ;
		float r ;
		float l;
		float t ;
		float b;	
	};

	CanvasSettings m_canvSet;
	Matrix44f m_camera;
	Matrix44f m_world;
	Matrix44f m_openGlProjM;
	BoundingBox m_bndbox;
	std::vector<float> m_zBuffer;	
	std::vector<Vec3<unsigned char>> m_frameBuffer;

	void setCamera();
	void setOpenGLProjM();

	Vec3f objToWorld(const Vec3f& vObj);
	Vec3f worldToCam(const Vec3f& vWorld);
	Vec3f camToScreen(const Vec3f& vCam);
	Vec3f screenToNDC(const Vec3f& vScreen);
	Vec3f vToRaster(const Vec3f& vWorld);
	bool triangleInScreen(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2);
	void buildFrameBuffer(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2);

	void vToRasterOpenGL(Vec3f& v);

	float edge(const Vec3f& v0, const Vec3f& v1, const Vec3f& v3);
	float max3(const float& a, const float& b, const float& c);
	float min3(const float& a, const float& b, const float& c);
	bool isvScreenVisible(const Vec3f& vScreen);	

public:
	explicit Rasterizer(const Matrix44f& cam = Matrix44f{}) ;
	void render(uint32_t numTris, std::vector<uint32_t>& nverts, std::vector<Vec3f>& vertices);
	void openGLrender(uint32_t numTris, std::vector<uint32_t>& nverts, std::vector<Vec3f>& vertices);
	std::vector<Vec3<unsigned char>> getFrameBuffer() const;
};

