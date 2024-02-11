#pragma once
#include "geometry.h"

class Object3D
{
public:
	virtual ~Object3D(){}

	virtual bool intersect(const Vec3f& , const Vec3f& , float& ) const = 0;

};

class Sphere :public Object3D
{
private:
	Vec3f m_c; //center
	float m_r; // radius

public:
	Sphere();
	Sphere(const Vec3f&, float);

	bool intersect(const Vec3f& , const Vec3f& , float& ) const override;



};

