#include <iostream>
#include "Rasterizer.h"

using namespace CameraConstants;

 Rasterizer::Rasterizer(const Matrix44f& cam ) :m_camera{cam}
{
	

	setCamera();
	m_openGlProjM = Matrix44f{};
	setOpenGLProjM();

	m_world = Matrix44f(1.624241, 0, 2.522269, 0, 0, 3, 0, 0, -2.522269, 0, 1.624241, 0, 0, 0, 0, 1);
	
	m_zBuffer = std::vector<float>(imageWidth * imageHeight);
	m_frameBuffer=std::vector<Vec3<unsigned char>>(imageWidth * imageHeight);
	
	
	for (int i = 0; i < imageHeight * imageWidth;++i)	
		m_zBuffer[i] = farClipping;

	
}

 void Rasterizer::setCamera()
 {
	 //screen(canvas) dimensions are = right, left, top, bottom
	//We use imaginary viewing frustrum	

	 m_canvSet.canvasW = nearClipping * filmW / focalL;
	 m_canvSet.canvasH = nearClipping * filmH / focalL;

	 m_canvSet.r = m_canvSet.canvasW / 2;
	 m_canvSet.l = -m_canvSet.r;
	 m_canvSet.t = m_canvSet.canvasH / 2;
	 m_canvSet.b = -m_canvSet.t;

	 float xscale = 1;
	 float yscale = 1;

	 if (filmAspectRatio > deviceAspectRatio)
		 xscale = deviceAspectRatio / filmAspectRatio;
	 else
		 yscale = filmAspectRatio / deviceAspectRatio;


	 m_canvSet.r *= xscale;
	 m_canvSet.t *= yscale;

	 m_canvSet.b = -m_canvSet.t;
	 m_canvSet.l = -m_canvSet.r;


 }

 void Rasterizer::setOpenGLProjM()
 {
	 m_openGlProjM[0][0] = 2 * CameraConstants::nearClipping / (m_canvSet.r - m_canvSet.l);
	 m_openGlProjM[1][1] = 2 * CameraConstants::nearClipping / (m_canvSet.t - m_canvSet.b);
	 m_openGlProjM[2][0] = (m_canvSet.r + m_canvSet.l) / (m_canvSet.r - m_canvSet.l);
	 m_openGlProjM[2][1] = (m_canvSet.t + m_canvSet.b) / (m_canvSet.t - m_canvSet.b);
	 m_openGlProjM[2][2] = -(CameraConstants::farClipping + CameraConstants::nearClipping) / (CameraConstants::farClipping - CameraConstants::nearClipping);
	 m_openGlProjM[2][3] = -1;
	 m_openGlProjM[3][3] = 0;

 }

 Vec3f Rasterizer::objToWorld(const Vec3f& vObj)
 {
	 Vec3f vWorld;
	 m_world.multVecMatrix(vObj, vWorld);
	 return vWorld;

 }

Vec3f Rasterizer::worldToCam(const Vec3f& vWorld)
{
	Vec3f vCam;
	m_camera.multVecMatrix(vWorld, vCam);
	return vCam;
}

Vec3f Rasterizer::camToScreen(const Vec3f& vCam)
{
	Vec3f vScreen;
	vScreen.x = -nearClipping * vCam.x / vCam.z;
	vScreen.y= -nearClipping * vCam.y / vCam.z;
	vScreen.z = -vCam.z;

	return vScreen;
}

bool Rasterizer::isvScreenVisible(const Vec3f& vScreen)
{	
	return (vScreen.x > m_canvSet.r || vScreen.x <m_canvSet.l || vScreen.y>m_canvSet.t
		                            || vScreen.y < m_canvSet.b) ? false : true;			
}

Vec3f Rasterizer::screenToNDC(const Vec3f& vScreen)
{
	float denomX = m_canvSet.r - m_canvSet.l;
	float numerX = m_canvSet.r + m_canvSet.l;

	float denomY = m_canvSet.t - m_canvSet.b;
	float numerY = m_canvSet.t + m_canvSet.b;

	Vec3f vNDC = vScreen;
	vNDC.x = (2 * vNDC.x / denomX) - (numerX / denomX);
	vNDC.y= (2 * vNDC.y / denomY) - (numerY / denomY);

	return vNDC;
}

Vec3f Rasterizer::vToRaster(const Vec3f& vWorld)
{
	//Vec3f vRaster = objToWorld(vWorld);
	Vec3f vRaster = worldToCam(vWorld);
	vRaster = camToScreen(vRaster);	
	vRaster = screenToNDC(vRaster);

	vRaster.x = (vRaster.x+1)/2 * imageWidth;
	vRaster.y = (1 - vRaster.y)/2 * imageHeight;
	vRaster.z = -vRaster.z;
	return vRaster;

}

Vec3f Rasterizer::vToNDCOpengl(const Vec3f& vWorld)
{
	Vec3f out;
	out.x = vWorld.x * m_openGlProjM[0][0] + vWorld.y * m_openGlProjM[1][0] + vWorld.z * m_openGlProjM[2][0] + /* in.z = 1 */ m_openGlProjM[3][0];
	out.y = vWorld.x * m_openGlProjM[0][1] + vWorld.y * m_openGlProjM[1][1] + vWorld.z * m_openGlProjM[2][1] + /* in.z = 1 */ m_openGlProjM[3][1];
	out.z = vWorld.x * m_openGlProjM[0][2] + vWorld.y * m_openGlProjM[1][2] + vWorld.z * m_openGlProjM[2][2] + /* in.z = 1 */ m_openGlProjM[3][2];
	float w = vWorld.x * m_openGlProjM[0][3] + vWorld.y * m_openGlProjM[1][3] + vWorld.z * m_openGlProjM[2][3] + /* in.z = 1 */ m_openGlProjM[3][3];

	if (w != 1) 
	{
		out.x /= w;		out.y /= w;		out.z /= w;
	}

	return out;

}

float Rasterizer::edge(const Vec3f& v0, const Vec3f& v1, const Vec3f& p)
{
	// the p is inside of the triangle if cross product,  " p X v0-v1 >= 0 "
	//we see it as positive when the resulting vec points towards us

	return (p[0] - v0[0]) * (v1[1] - v0[1]) - (v1[0] - v0[0])*(p[1] - v0[1]) ;
}

float Rasterizer::min3(const float& a, const float& b, const float& c)
{
	return std::min(a, std::min(b, c));
}

float Rasterizer::max3(const float& a, const float& b, const float& c)
{
	return std::max(a, std::max(b, c));
}

void Rasterizer::render(uint32_t numTris, std::vector<uint32_t>& nverts, std::vector<Vec3f>& vertices)
{
	for (int i = 0; i < numTris; ++i)
	{
		Vec3f vWorld0 = vertices[nverts[i * 3]];
		Vec3f vWorld1 = vertices[nverts[i * 3+1]];
		Vec3f vWorld2 = vertices[nverts[i * 3+2]];

		Vec3f vRaster0 = vToRaster(vWorld0);
		Vec3f vRaster1 = vToRaster(vWorld1);
		Vec3f vRaster2 = vToRaster(vWorld2);

		//invert z coords for z-buffer calculations
		vRaster0.z = 1 / vRaster0.z;
		vRaster1.z = 1 / vRaster1.z;
		vRaster2.z = 1 / vRaster2.z;


		float xmin = min3(vRaster0.x, vRaster1.x, vRaster2.x);
		float ymin = min3(vRaster0.y, vRaster1.y, vRaster2.y);
		float xmax = max3(vRaster0.x, vRaster1.x, vRaster2.x);
		float ymax = max3(vRaster0.y, vRaster1.y, vRaster2.y);	
		
		// if the triangle is out of screen, then skip
		if (xmin > imageWidth - 1 || xmax < 0 || ymin > imageHeight - 1 || ymax < 0) continue;

		uint32_t x0 = std::max(int32_t(0), (int32_t)(std::floor(xmin)));
		uint32_t x1 = std::min(int32_t(imageWidth) - 1, (int32_t)(std::floor(xmax)));
		uint32_t y0 = std::max(int32_t(0), (int32_t)(std::floor(ymin)));
		uint32_t y1 = std::min(int32_t(imageHeight) - 1, (int32_t)(std::floor(ymax)));

		//loop over the pixels going through the bounding box

		float area = edge(vRaster0, vRaster1, vRaster2);

		for (uint32_t  i = y0; i < y1; ++i)
		{
			for (uint32_t  j = x0; j < x1; ++j)
			{
				Vec3f p = { j + 0.5f, i + 0.5f,0.0f };
				float w0 = edge(vRaster1, vRaster2, p);
				float w1 = edge(vRaster2, vRaster0, p);
				float w2 = edge(vRaster0, vRaster1, p);
				

				if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				{
					w0 /= area;
					w1 /= area;
					w2 /= area;					

					float invertedZ = vRaster0.z * w0 + vRaster1.z * w1 + vRaster2.z * w2;
					float z = 1 / invertedZ;

					if (m_zBuffer[i * imageWidth + j] > z)
					{	
						m_zBuffer[i * imageWidth + j] = z;
						m_frameBuffer[i * imageWidth + j].x = 0;
						m_frameBuffer[i * imageWidth + j].y = 120;
						m_frameBuffer[i * imageWidth + j].z = 0;		
										
					}
				}

			}
		}

		

	}
}

void Rasterizer::openGLrender(uint32_t numTris, std::vector<uint32_t>& nverts, std::vector<Vec3f>& vertices)
{
	for (int i = 0; i < numTris; ++i)
	{
		Vec3f vWorld0 = vertices[nverts[i * 3]];
		Vec3f vWorld1 = vertices[nverts[i * 3 + 1]];
		Vec3f vWorld2 = vertices[nverts[i * 3 + 2]];


	}
}



std::vector<Vec3<unsigned char>> Rasterizer::getFrameBuffer() const
{
	return m_frameBuffer;
}




