#pragma once

#include "glm/glm.hpp"
#include "RenderingUtility.h"
#include "Settings.h"

enum BSDFType
{
	TYPE_DIFFUSE,
	TYPE_SPECULAR
};

/* Abstract BSDF super class */
class BSDF
{

public:
	BSDF() {

	}

	~BSDF() {

	}

	/* Calls sub class destructor */
	virtual void deleteBSDF() = 0;

	/* Returns the BSDF Type */
	virtual BSDFType getType() = 0;

	/* Checks wether the output direction is valid (surface normal culling!) */
	virtual bool validOutputDirection(const vec3& normal, const vec3& outDir) = 0;

	/* Evaluates the BSDF value for the given output and input direction. */
	virtual vec3 evalBSDF(const vec3& normal, const vec3& outDir, const vec3& inDir) = 0;

	/* Sampling of a BSDF direction! */
	virtual vec3 sampleBSDFDirection(const vec3& normal, const vec3& inDir, const double& xi1, const double& xi2) = 0;

	/* Calculates the PDF for sampling the direction */
	virtual double getBSDFDirectionPDF(const vec3& normal, const vec3& sampledDir, const vec3& inDir) = 0;
};

/* Diffuse Lambertian BSDF */
class DiffuseLambertianBSDF : public BSDF
{

private:
	vec3 albedo;

public:
	DiffuseLambertianBSDF(const vec3& albedo) : BSDF(), albedo(albedo)
	{

	}

	~DiffuseLambertianBSDF() {

	}

	void deleteBSDF() {
		this->~DiffuseLambertianBSDF();
	}

	/* Returns the BSDF Type */
	BSDFType getType()
	{
		return TYPE_DIFFUSE;
	}

	/* Checks wether the output direction is valid (surface normal culling!) */
	bool validOutputDirection(const vec3& normal, const vec3& outDir)
	{
		return (dot(normal, outDir) > 0.0f);
	}

	/* Evaluates the BSDF value for the given output direction. */
	vec3 evalBSDF(const vec3& normal, const vec3& outDir, const vec3& inDir)
	{
		float cosTheta = dot(normal, outDir);
		assert(cosTheta > 0.0f);
		return (float)M_1_PI * cosTheta * albedo;
	}

	/* Sampling of a BSDF direction! */
	vec3 sampleBSDFDirection(const vec3& normal, const vec3& inDir, const double& xi1D, const double& xi2D)
	{
		//uniform sampling of the hemsiphere
		vec3 u, v;
		coordinateSystem(normal, u, v);

		float xi1 = (float)xi1D;

		float inner = sqrtf(1.0f - xi1);
		float theta = acosf(inner);
		float phi = (float)(2.0 * M_PI * xi2D);

		vec3 result = sin(theta)*cos(phi)*u + sin(theta)*sin(phi)*v + cos(theta)*normal;

		return result;
	}

	/* Calculates the PDF for sampling the direction */
	double getBSDFDirectionPDF(const vec3& normal, const vec3& sampledDir, const vec3& inDir)
	{
		float cos_Theta = dot(normal, sampledDir);
		if (cos_Theta > 0.0f) {
			return M_1_PI * (double)cos_Theta;
		}
		else {
			cout << "Lambertian BSDF: invalid sampledDir!" << endl;
			return 0.0;
		}
	}
};
