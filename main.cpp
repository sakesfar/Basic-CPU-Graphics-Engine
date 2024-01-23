#include <iostream>
#include <fstream>
#include "Rasterizer.h"
#include "cow.h"

const uint32_t ntris = 3156;
constexpr int  imgW = CameraConstants::imageWidth;
constexpr int  imgH = CameraConstants::imageHeight;

void toVec(std::vector<uint32_t>& nverts, std::vector<Vec3f>& verts)
{
	
	for (uint32_t i = 0; i < ntris*3; ++i)
	{
		nverts[i] = nvertices[i];
	}

	

	for (int i = 0; i < ntris; ++i)
	{
		verts[i] = vertices[i];
	}
	
	
}


int main()
{

	std::vector<uint32_t> nverts(ntris*3);	
	std::vector<Vec3f> verts(ntris);	

	toVec(nverts, verts);

	
	const Matrix44f worldToCamera = { 0.707107, -0.331295, 0.624695, 0, 0, 
		0.883452, 0.468521, 0, -0.707107, -0.331295, 0.624695, 0, -1.63871, -5.747777, -40.400412, 1 };

	Rasterizer imgGenerator{worldToCamera};
	imgGenerator.render(ntris, nverts, verts);
	std::ofstream ofs;
	ofs.open("cow7.ppm", std::ios::binary);
	ofs << "P6\n" << CameraConstants::imageWidth << " " << CameraConstants::imageHeight << "\n255\n";
	
	std::vector<Vec3<unsigned char>> fr = imgGenerator.getFrameBuffer();
	ofs.write(reinterpret_cast<const char*>(fr.data()), fr.size()*(size_t)3 );	

	ofs.close();
	
	
	return 0;
}