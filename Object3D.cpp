#include "Object3D.h"

Sphere::Sphere() : m_c{Vec3f()}, m_r{1.0f} {}

Sphere::Sphere(const Vec3f& c, float r) : m_c{c}, m_r{r} {}

bool Sphere::intersect(const Vec3f& rayO, const Vec3f& rayDir, float& t) const
{
	float t0, t1;

    Vec3f L = m_c - rayO;
    float tca = L.dotProduct(rayDir);
    if (tca < 0) return false;
    float d2 = L.dotProduct(L) - tca * tca;
    if (d2 > m_r) return false;
    float thc = sqrt(m_r*m_r - d2);
    t0 = tca - thc;
    t1 = tca + thc;

    if (t0 > t1) std::swap(t0, t1);

    if (t0 < 0) {
        t0 = t1; // if t0 is negative, let's use t1 instead
        if (t0 < 0) return false; // both t0 and t1 are negative
    }

    t = t0;

    return true;
}


