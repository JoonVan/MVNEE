#pragma once
#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iostream>

#include <math.h>

#include "StringParser.h"

#include <glm/glm.hpp>

#include <embree2/rtcore.h>
#include <embree2/rtcore_ray.h>
#include <embree2/rtcore_geometry.h>
#include <embree2/rtcore_scene.h>

#include "Camera.h"
#include "LightSource.h"
#include "Settings.h"

using namespace std;
using glm::vec3;

struct vector3{ float x, y, z; };
struct EmbreeVertex   { float x, y, z, r; };
struct Triangle { int v0, v1, v2; };
struct Quad     { int v0, v1, v2, v3; };

struct ObjectData{ string name; vec3 albedo; };

const int MAX_OBJECT_COUNT = 10;

class Scene
{

private:
	RTCScene scene; //Scene object for embree	
	ObjectData objectData[MAX_OBJECT_COUNT];

public:
	Camera camera;
	LightSource* lightSource;
	
public:

	Scene(RTCScene scene) : scene(scene) {
		
		//initialize camera, light and set light geometry in embree scene
		camera.cameraOrigin = CameraSettings::cameraOrigin;
		camera.camLookAt = CameraSettings::camLookAt;
		camera.camRight = CameraSettings::camRight;
		camera.camUp = CameraSettings::camUp;
		camera.distanceToImagePlane = CameraSettings::distanceToImagePlane;
		camera.imagePlaneHeight = CameraSettings::imagePlaneHeight;
		camera.imagePlaneWidth = CameraSettings::imagePlaneWidth;		

		LightDisk* lightDisk = new LightDisk(LightSettings::lightCenter, LightSettings::lightNormal, LightSettings::lightU, LightSettings::lightV, LightSettings::lightColor, LightSettings::lightBrightness, LightSettings::lightRadius);
		lightSource = lightDisk;

		//add light geometry to embree scene:
		int objectID = addCircularPlane(LightSettings::lightCenter, LightSettings::lightNormal, LightSettings::lightU, LightSettings::lightV, LightSettings::lightRadius, 100, "light", LightSettings::lightColor);
	}

	~Scene() {
		if (lightSource->getType() == TypeLightDisk) {
			LightDisk* lightDisk = (LightDisk*) lightSource;
			delete lightDisk;
		}
	}

	/**
	* Adds a ground plane to the scene .
	* Returns the Embree geometry ID;
	*/
	unsigned int addGroundPlane(const string& name, const vec3& albedo, const float& sideLength, const float& yPosition)
	{
		/* create a triangulated plane with 2 triangles and 4 vertices */
		unsigned int mesh = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, 2, 4);

		/* set vertices */
		EmbreeVertex* vertices = (EmbreeVertex*)rtcMapBuffer(scene, mesh, RTC_VERTEX_BUFFER);
		vertices[0].x = -sideLength; vertices[0].y = yPosition; vertices[0].z = -sideLength;
		vertices[1].x = -sideLength; vertices[1].y = yPosition; vertices[1].z = +sideLength;
		vertices[2].x = +sideLength; vertices[2].y = yPosition; vertices[2].z = -sideLength;
		vertices[3].x = +sideLength; vertices[3].y = yPosition; vertices[3].z = +sideLength;
		rtcUnmapBuffer(scene, mesh, RTC_VERTEX_BUFFER);

		/* set triangles */
		Triangle* triangles = (Triangle*)rtcMapBuffer(scene, mesh, RTC_INDEX_BUFFER);
		triangles[0].v0 = 0; triangles[0].v1 = 2; triangles[0].v2 = 1;
		triangles[1].v0 = 1; triangles[1].v1 = 2; triangles[1].v2 = 3;
		rtcUnmapBuffer(scene, mesh, RTC_INDEX_BUFFER);

		ObjectData objectData;
		objectData.albedo = albedo;
		objectData.name = name;
		addObjectData(mesh, objectData);

		return mesh;
	}

	/* adds a cube to the scene */
	unsigned int addCube(const vec3& center, float sideLengthHalf, const string& name, const vec3& albedo)
	{
		/* create a triangulated cube with 12 triangles and 8 vertices */
		unsigned int mesh = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, 12, 8);

		/* set vertices and vertex colors */
		EmbreeVertex* vertices = (EmbreeVertex*)rtcMapBuffer(scene, mesh, RTC_VERTEX_BUFFER);
		vertices[0].x = center.x - sideLengthHalf; vertices[0].y = center.y - sideLengthHalf; vertices[0].z = center.z - sideLengthHalf;
		vertices[1].x = center.x - sideLengthHalf; vertices[1].y = center.y - sideLengthHalf; vertices[1].z = center.z + sideLengthHalf;
		vertices[2].x = center.x - sideLengthHalf; vertices[2].y = center.y + sideLengthHalf; vertices[2].z = center.z - sideLengthHalf;
		vertices[3].x = center.x - sideLengthHalf; vertices[3].y = center.y + sideLengthHalf; vertices[3].z = center.z + sideLengthHalf;
		vertices[4].x = center.x + sideLengthHalf; vertices[4].y = center.y - sideLengthHalf; vertices[4].z = center.z - sideLengthHalf;
		vertices[5].x = center.x + sideLengthHalf; vertices[5].y = center.y - sideLengthHalf; vertices[5].z = center.z + sideLengthHalf;
		vertices[6].x = center.x + sideLengthHalf; vertices[6].y = center.y + sideLengthHalf; vertices[6].z = center.z - sideLengthHalf;
		vertices[7].x = center.x + sideLengthHalf; vertices[7].y = center.y + sideLengthHalf; vertices[7].z = center.z + sideLengthHalf;
		rtcUnmapBuffer(scene, mesh, RTC_VERTEX_BUFFER);

		/* set triangles and face colors */
		int tri = 0;
		Triangle* triangles = (Triangle*)rtcMapBuffer(scene, mesh, RTC_INDEX_BUFFER);

		// left side
		triangles[tri].v0 = 0; triangles[tri].v1 = 2; triangles[tri].v2 = 1; tri++;
		triangles[tri].v0 = 1; triangles[tri].v1 = 2; triangles[tri].v2 = 3; tri++;

		// right side
		triangles[tri].v0 = 4; triangles[tri].v1 = 5; triangles[tri].v2 = 6; tri++;
		triangles[tri].v0 = 5; triangles[tri].v1 = 7; triangles[tri].v2 = 6; tri++;

		// bottom side
		triangles[tri].v0 = 0; triangles[tri].v1 = 1; triangles[tri].v2 = 4; tri++;
		triangles[tri].v0 = 1; triangles[tri].v1 = 5; triangles[tri].v2 = 4; tri++;

		// top side
		triangles[tri].v0 = 2; triangles[tri].v1 = 6; triangles[tri].v2 = 3; tri++;
		triangles[tri].v0 = 3; triangles[tri].v1 = 6; triangles[tri].v2 = 7; tri++;

		// front side
		triangles[tri].v0 = 0; triangles[tri].v1 = 4; triangles[tri].v2 = 2; tri++;
		triangles[tri].v0 = 2; triangles[tri].v1 = 4; triangles[tri].v2 = 6; tri++;

		// back side
		triangles[tri].v0 = 1; triangles[tri].v1 = 3; triangles[tri].v2 = 5; tri++;
		triangles[tri].v0 = 3; triangles[tri].v1 = 7; triangles[tri].v2 = 5; tri++;

		rtcUnmapBuffer(scene, mesh, RTC_INDEX_BUFFER);

		ObjectData objectData;
		objectData.albedo = albedo;
		objectData.name = name;
		addObjectData(mesh, objectData);

		return mesh;
	}

	/**
	* Adds a circular plane specified by its center and radius, as well as the u and v vectors which together with the normal form a left handed coordinate system.
	* The resulting circualr plane lies in the u-v-plane.
	*/
	unsigned int addCircularPlane(const vec3& center, const vec3& normal, const vec3& u, const vec3& v, const float& radius, const int triangleCount, const string& name, const vec3& albedo)
	{
		/* create a triangulated circular plane with triangleCount triangles*/
		unsigned int mesh = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, triangleCount, triangleCount + 1);

		/* set vertices */
		EmbreeVertex* vertices = (EmbreeVertex*)rtcMapBuffer(scene, mesh, RTC_VERTEX_BUFFER);
		//first set center vertex:
		vertices[0].x = center.x; vertices[0].y = center.y; vertices[0].z = center.z;

		float theta;
		vec3 currentVertex;
		for (int i = 0; i < triangleCount; i++) {
			theta = ((float)i / (float)triangleCount) * (float)(2.0 * M_PI);
			currentVertex = center + radius * sinf(theta) * u + radius * cosf(theta) * v;
			vertices[i + 1].x = currentVertex.x; vertices[i + 1].y = currentVertex.y; vertices[i + 1].z = currentVertex.z;
		}
		rtcUnmapBuffer(scene, mesh, RTC_VERTEX_BUFFER);

		/* set triangle faces */
		Triangle* triangles = (Triangle*)rtcMapBuffer(scene, mesh, RTC_INDEX_BUFFER);
		for (int i = 0; i < triangleCount - 1; i++) {
			triangles[i].v0 = 0; triangles[i].v1 = i + 2; triangles[i].v2 = i + 1;
		}
		//last triangle to close to circle:
		triangles[triangleCount - 1].v0 = 0; triangles[triangleCount - 1].v1 = 1; triangles[triangleCount - 1].v2 = triangleCount;
		rtcUnmapBuffer(scene, mesh, RTC_INDEX_BUFFER);

		ObjectData objectData;
		objectData.albedo = albedo;
		objectData.name = name;
		addObjectData(mesh, objectData);

		return mesh;
	}


	/**
	* Adds the geometry of the specified obj. file to the scene.
	* Returns the Embree geometry ID;
	*/
	unsigned int addObject(const string& objFilePath, const string& name, const vec3& albedo, const vec3& translationVector, const float& scaling) {

		cout << "Object: " << name;

		ifstream file;
		file.open(objFilePath, ios::in); // opens as ASCII!
		if (!file) {
			cout << "Object file could not be opened!" << endl;
			cout << "Please check the file path: " << objFilePath << endl;
			return RTC_INVALID_GEOMETRY_ID;
		}

		string currentLine;
		vector<vec3> vertices = vector<vec3>();
		vector<Triangle> faces = vector<Triangle>();

		while (!file.eof()) {
			getline(file, currentLine);
			StringParser line = StringParser(currentLine);
			if (!line.startsWith("#")) {
				//handle all different specifiers
				if (line.startsWith("vn")) {
				}
				else if (line.startsWith("vt")) {
				}
				else if (line.startsWith("v ")) {
					vec3 v = line.getVec3Param("v");
					vec3 vAdjusted = vec3(v.x, v.y, -v.z);
					vec3 transformedVec = (vAdjusted * scaling) + translationVector;
					vertices.push_back(transformedVec);
				}
				else if (line.startsWith("f")) {

					StringParser faceStrings = StringParser(line.retrieveArgumentForParam("f"));
					//first split at ' '
					vector<StringParser> faceList = vector<StringParser>();
					faceStrings.split(' ', &faceList);

					int faceVertexCount = faceList.size();
					//only triangels allowed for now!;
					assert(faceVertexCount == 3);

					Triangle currentFace;

					vector<StringParser> perVertexIndices;
					faceList.at(0).split('/', &perVertexIndices);
					int v0 = atoi(perVertexIndices[0].getCharacterData());
					currentFace.v0 = v0 - 1;

					vector<StringParser> perVertexIndices2;
					faceList.at(1).split('/', &perVertexIndices2);
					int v1 = atoi(perVertexIndices2[0].getCharacterData());
					currentFace.v1 = v1 - 1;

					vector<StringParser> perVertexIndices3;
					faceList.at(2).split('/', &perVertexIndices3);
					int v2 = atoi(perVertexIndices3[0].getCharacterData());
					currentFace.v2 = v2 - 1;

					faces.push_back(currentFace);
				}
			}
		}

		int vertexCount = vertices.size();
		int faceCount = faces.size();

		cout << " has " << faceCount << " triangles." << endl;

		/* create a mesh with faceCount triangles and vertexCount vertices */
		unsigned int mesh = rtcNewTriangleMesh(scene, RTC_GEOMETRY_STATIC, faceCount, vertexCount);

		///////////////
		//read vertices
		///////////////

		/* set vertices */
		EmbreeVertex* verticesBuffer = (EmbreeVertex*)rtcMapBuffer(scene, mesh, RTC_VERTEX_BUFFER);
		for (int i = 0; i < vertexCount; i++) {
			//convert to our coordinate system:
			vec3 vertex = vertices.at(i);

			//set data
			verticesBuffer[i].x = vertex.x;
			verticesBuffer[i].y = vertex.y;
			verticesBuffer[i].z = vertex.z;
		}
		rtcUnmapBuffer(scene, mesh, RTC_VERTEX_BUFFER);

		///////////////
		//read faces
		///////////////				

		/* set triangles */
		Triangle* triangles = (Triangle*)rtcMapBuffer(scene, mesh, RTC_INDEX_BUFFER);
		for (int i = 0; i < faceCount; i++) {
			//set data: make sure indices start at 0!! 
			Triangle triangle = faces.at(i);

			triangles[i].v0 = triangle.v0;
			triangles[i].v1 = triangle.v1;
			triangles[i].v2 = triangle.v2;

			//triangles[i].v0 = triangle.v0;
			//triangles[i].v1 = triangle.v2;
			//triangles[i].v2 = triangle.v1;

		}
		rtcUnmapBuffer(scene, mesh, RTC_INDEX_BUFFER);

		ObjectData objectData;
		objectData.albedo = albedo;
		objectData.name = name;
		addObjectData(mesh, objectData);

		return mesh;
	}

	bool addObjectData(int objectID, const ObjectData& data) {
		assert(objectID < MAX_OBJECT_COUNT);
		if (objectID >= 0 && objectID < MAX_OBJECT_COUNT) {
			objectData[objectID] = data;
			return true;
		}
		else {
			cout << "object id out of range! -> increase MAX_OBJECT_COUNT" << endl;
			return false;
		}
	}

	/* Returns the object data for the given geometryID in the ObjectData struct pointer
	* Return true if obejct was found, false otherwise.
	*/
	bool getObjectData(int geometryID, ObjectData* result) {

		if (geometryID >= 0 && geometryID < MAX_OBJECT_COUNT) {
			*result = objectData[geometryID];
			return true;
		}
		else {
			result->name = "none";
			result->albedo = vec3(0.0f);
			return false;
		}		
	}

	/**
	* Shoots a ray specified by its origin and direction through the Scene. Returns true if intersection is found.
	* The struct ray is used to output the intersection information when a hit was found.
	*
	* The intersection normal is stored in the output variable intersectionNormal
	*/
	bool intersectScene(const vec3& rayOrigin, const vec3& rayDir, RTCRay& ray, vec3& intersectionNormal)
	{
		ray.time = 0.0f;
		ray.org[0] = rayOrigin.x;
		ray.org[1] = rayOrigin.y;
		ray.org[2] = rayOrigin.z;
		ray.dir[0] = rayDir.x;
		ray.dir[1] = rayDir.y;
		ray.dir[2] = rayDir.z;
		ray.tnear = 0.0f;
		ray.tfar = FLT_MAX;
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
		ray.primID = RTC_INVALID_GEOMETRY_ID;
		ray.mask = -1;

		rtcIntersect(scene, ray);

		bool hit = false;
		if (ray.geomID != RTC_INVALID_GEOMETRY_ID) {
			hit = true;
			vec3 ng = vec3(ray.Ng[0], ray.Ng[1], ray.Ng[2]);
			intersectionNormal = normalize(ng);
		}
		return hit;
	}

	/**
	* Checks for occlusion using a shadowray specified by a surface vertex and the direction to the light. 
	* Returns true if the surface point is occluded, i.e. the surface point is NOT visible from the light.
	* The struct ray is used to output the intersection information when a hit was found.
	*/
	bool surfaceOccluded(const vec3& surfacePosition, const vec3& dirToLight, const float& distanceToLight, RTCRay& shadowRay)
	{
		shadowRay.org[0] = surfacePosition[0];
		shadowRay.org[1] = surfacePosition[1];
		shadowRay.org[2] = surfacePosition[2];
		shadowRay.dir[0] = dirToLight[0];
		shadowRay.dir[1] = dirToLight[1];
		shadowRay.dir[2] = dirToLight[2];
		shadowRay.tnear = RenderingSettings::epsilon;
		shadowRay.tfar = distanceToLight - RenderingSettings::epsilon;
		shadowRay.geomID = 1;
		shadowRay.primID = 0;
		shadowRay.mask = -1;
		shadowRay.time = 0;

		/* trace shadow ray */
		rtcOccluded(scene, shadowRay);

		return (shadowRay.geomID == 0);
	}

	void readNextLine(ifstream& inputData, StringParser& output) {
		string currentLine;
		StringParser line;
		bool jumpOver = true;

		while (jumpOver) {
			getline(inputData, currentLine);
			line = StringParser(currentLine);
			if (!line.startsWith("#") || currentLine == "") {
				jumpOver = false;
			}
		}
		output = line;
	}

	/**
	* Reads in all the settings from the .set file.
	* Returns true if successfull, false otherwise, in this case the default settings in Settings.h are used!
	*/
	bool readSettingsFile(const string& settingsFilePath) {
		ifstream file;
		file.open(settingsFilePath, ios::in); // opens as ASCII!
		if (!file) {
			cout << "Settings file could not be opened!" << endl;
			cout << "Please check the file path: " << settingsFilePath << endl;
			return false;
		}

		StringParser line;

		readNextLine(file, line);
		if (line.startsWith("RenderingSettings:")) {
			readNextLine(file, line);
			if (line.startsWith("\tWIDTH =")) {
				int width = line.getIntParam("\tWIDTH =");
			}
			else {
				cout << "invalid Rendering Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tHEIGHT =")) {
				int height = line.getIntParam("\tHEIGHT =");
			}
			else {
				cout << "invalid Rendering Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tSAMPLE_COUNT =")) {
				int sampleCount = line.getIntParam("\tSAMPLE_COUNT =");
			}
			else {
				cout << "invalid Rendering Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tMAX_SEGMENT_COUNT =")) {
				int maxSegCount = line.getIntParam("\tMAX_SEGMENT_COUNT =");
			}
			else {
				cout << "invalid Rendering Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tINTEGRATOR =")) {
				string integratorName = line.retrieveArgumentForParam("\tINTEGRATOR =");
				IntegratorEnum intEnum;
				/*if (integratorName == "PATH_TRACING_MVNEE_OPTIMIZED2") {
					intEnum = PATH_TRACING_MVNEE_OPTIMIZED2;
				}
				else if (integratorName == "PATH_TRACING_MVNEE_OPTIMIZED") {
					intEnum = PATH_TRACING_MVNEE_OPTIMIZED;
				}*/
				if (integratorName == "PATH_TRACING_MVNEE") {
					intEnum = PATH_TRACING_MVNEE;
				}
				else if (integratorName == "PATH_TRACING_NEE_MIS") {
					intEnum = PATH_TRACING_NEE_MIS;
				}
				else if (integratorName == "PATH_TRACING_NEE_MIS_NO_SCATTERING") {
					intEnum = PATH_TRACING_NEE_MIS_NO_SCATTERING;
				}
				else if (integratorName == "PATH_TRACING_RANDOM_WALK") {
					intEnum = PATH_TRACING_RANDOM_WALK;
				}
				else if (integratorName == "PATH_TRACING_NO_SCATTERING") {
					intEnum = PATH_TRACING_NO_SCATTERING;
				}
				else {
					cout << "invalid integrator name: see IntegratorEnum!" << endl; return false;
				}
			}
			else {
				cout << "invalid Rendering Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (!line.startsWith("---")) {
				cout << "invalid Rendering Setting!!" << endl; return false;
			}			
		}
		else {
			cout << "invalid setup file! use provided setup file as basis!" << endl; return false;
		}

		readNextLine(file, line);
		if (line.startsWith("MediumParameters:")) {
			readNextLine(file, line);
			if (line.startsWith("\tmu_s =")) {
				double muS = line.getDoubleParam("\tmu_s =");
			}
			else {
				cout << "invalid Medium Parameter Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tHG_g =")) {
				double HGg = line.getIntParam("\tHG_g =");
			} 
			else {
				cout << "invalid Medium Parameter Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tmu_a =")) {
				int muA = line.getIntParam("\tmu_a =");
			}
			else {
				cout << "invalid Medium Parameter Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (!line.startsWith("---")) {
				cout << "invalid Medium Parameter Setting!!" << endl; return false;
			}
		}
		else {
			cout << "invalid setup file! use provided setup file as basis!" << endl; return false;
		}

		readNextLine(file, line);
		if (line.startsWith("LightSettings:")) {
			readNextLine(file, line);
			if (line.startsWith("\tLightType =")) {
				string lightTypeS = line.retrieveArgumentForParam("\tLightType =");
				LightTypeEnum lightType;
				if (lightTypeS == "TypeLightDisk") {
					lightType = TypeLightDisk;
				}
				else {
					cout << "invalid light type: see TypeLightDisk enum!" << endl; return false;
				}
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tcenter =")) {
				vec3 lCenter = line.getVec3Param("\tcenter =");
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tlightNormal =")) {
				vec3 lNormal = normalize(line.getVec3Param("\tlightNormal ="));
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tlightUp =")) {
				vec3 lUp = normalize(line.getVec3Param("\tlightUp ="));
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tlightBrightness =")) {
				float lightBrightness = line.getFloatParam("\tlightBrightness =");
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tlightColor =")) {
				vec3 lColor = line.getVec3Param("\tlightColor =");
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (line.startsWith("\tlightRadius =")) {
				float lightRadius = line.getFloatParam("\tlightRadius =");
			}
			else {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}

			readNextLine(file, line);
			if (!line.startsWith("---")) {
				cout << "invalid Medium Light Setting!!" << endl; return false;
			}
		}
		else {
			cout << "invalid setup file! use provided setup file as basis!" << endl; return false;
		}

		readNextLine(file, line);
		if (line.startsWith("CameraSettings:")) {

		}
		else {
			cout << "invalid setup file! use provided setup file as basis!" << endl; return false;
		}

		//loop over all objects
		readNextLine(file, line);
		while (line.startsWith("Object")) {

		}

		file.close();
	}
};