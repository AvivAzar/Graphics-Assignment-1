#include "InputManager.h"
#include "game.h"
#include "sceneParser.h"
#include "../res/includes/glm/glm.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <fstream>
#define M_PI 3.14159265358979323846
float epsilon = 0.005;
const float AIR_REFRACTIVE_INDEX = 1.0f; // Refractive index of air
const float SPHERE_REFRACTIVE_INDEX = 1.5f; // Refractive index of the sphere material
using namespace std;
using namespace glm;
scene scn("../res/scene.txt");

vec3 cap(vec3 vec) { //caps vector values between 0 and 1, used for color vectors
	if (vec.x < 0) vec.x = 0;
	if (vec.y < 0) vec.y = 0;
	if (vec.z < 0) vec.z = 0;
	if (vec.x > 1) vec.x = 1;
	if (vec.y > 1) vec.y = 1;
	if (vec.z > 1) vec.z = 1;
	return vec;
}

vec3 make3(vec4 v) { //cuts out the last dimension of a 4d vector
	return vec3(v.x, v.y, v.z);
}

float sphereIntersection(vec3 p, vec3 v, vec4 sphere) {//p0-ray origin (camera if ray hasn't bounced yet), v-curret ray, sphere- sphere currently checked for intersection
	vec3 o = make3(sphere);
	float r = sphere.w;
	vec3 d = p - o;
	float a = dot(v, v);
	float b = 2.0 * dot(v, d);
	float c = dot(d, d) - (r * r);

	float delta = b * b - 4.0 * a * c;

	if (delta < epsilon) {
		return -1.0;
	}
	float x1 = (-b + sqrt(delta)) / (2.0 * a);
	float x2 = (-b - sqrt(delta)) / (2.0 * a);


	if (x1 > epsilon && x2 > epsilon) {
		return std::min(x1, x2);
	}
	else if (x1 < epsilon && x2 < epsilon) {
		return -1.0;
	}
	else return std::max(x1, x2);
}

float planeIntersection(vec3 p, vec3 v, vec4 plane) {//p0-ray origin (camera if ray hasn't bounced yet), v-curret ray, plane- plane currently checked for intersection
	vec3 N = make3(plane);
	float d = plane.w;
	vec3 q = N * (-d);
	float t;
	if (dot(N, v) != 0) t = -((dot(p, N) + d) / dot(v, N));

	else t = -1;

	if (t < epsilon) {
		return -1.0;
	}
	else {
		return t;
	}
}

bool isSphere(vec4 obj) {
	return obj.w > 0;
}

bool isDirectionalLight(vec4 light) {
	return light.w == 0.0;
}

bool squareCoefficient(vec3 p) {
	bool resx = (0 < p.x - round(p.x) < 0.5);
	bool resy = (0 < p.y - round(p.y) < 0.5);
	return (resx == resy);
}

float intersectionFunc(vec3 p, vec3 v, vec4 obj) {
	if (isSphere(obj)) {
		return sphereIntersection(p, v, obj);
	}
	else {
		return planeIntersection(p, v, obj);
	}
}

int lightsCount() {
	return scn.sizes.y;
}

int objectsCount() {
	return scn.sizes.x;
}

vec3 getObjectNormal(int i, vec3 p) {
	return normalize(isSphere(scn.objects[i]) ? vec3(scn.objects[i].x - p.x, scn.objects[i].y - p.y, scn.objects[i].z - p.z) : make3(scn.objects[i]));
}

bool isLightBlockedByPlane(vec3 p, vec3 dir, vec4 plane, int light) {
	if (scn.directions[light].w == 0.0f) 
		return(planeIntersection(p, -dir, plane) > epsilon);
	else return false;
}

bool isLightBlockedBySphere(vec3 p, vec3 dir, vec4 sphere, int light) {
	vec3 sphereCenter = make3(sphere);
	float sphereRadius = sphere.w;
	vec3 dirToSphere = sphereCenter - p;
	float t;
	if (dot(-dir, dirToSphere) < 0.0) {
		return false;
	}
	else {
		float closestApproachDist = dot(dirToSphere, -dir);
		vec3 closestPointOnRay = p + closestApproachDist * -dir;
		float distToSphereCenter = distance(closestPointOnRay, sphereCenter);

		if (distToSphereCenter <= sphereRadius) {
			return true; // Light is blocked
		}
		else {
			return false;
		}
	}
}

bool isLightBlockedBy(vec3 p, vec3 dir, vec4 object, int light) {
	if (isSphere(object)) return isLightBlockedBySphere(p, dir, object, light);
	else return isLightBlockedByPlane(p, dir, object, light);
}

vec3 getLightDirection(int i, vec3 p) {
	vec3 L = vec3(0.0, 0.0, 0.0);
	if (isDirectionalLight(scn.directions[i])) 
		L = normalize(make3(scn.directions[i]));
	else if (scn.lights[i].w < dot(normalize(make3(scn.directions[i])), normalize(vec3(p.x - scn.lights[i].x, p.y - scn.lights[i].y, p.z - scn.lights[i].z))))
		L = normalize(p - make3(scn.lights[i]));
	return L;
}

vec3 applyAmbient(vec3 intersectionColor) {
	return cap(make3(scn.ambient) * intersectionColor);
}

vec3 applyDiffuse(int intersection, vec3 p, vec3 N, vec3 intersectionColor) {
	vec3 diffuse = vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < lightsCount(); i++) {
		vec3 L = getLightDirection(i, p); // light's direction
		bool isBlocked = (L.x == 0 && L.y == 0 && L.z == 0); // for shadow

		for (int j = 0; j < objectsCount(); j++) {
			if (isLightBlockedBy(p, L, scn.objects[j], i))
				isBlocked = true;
		}

		if (!isBlocked) {
			vec3 I(make3(scn.intensities[i]));
			diffuse += (intersectionColor * dot(N, L)) * I;
		}
	}

	return cap(diffuse);
}

vec3 applySpecular(int intersection, vec3 p, vec3 N, vec3 v) {
	vec3 specular = vec3(0.0, 0.0, 0.0);
	vec3 Ks = vec3(0.7, 0.7, 0.7);
	float n = scn.colors[intersection].w;

	for (int i = 0; i < lightsCount(); i++) {
		vec3 L = getLightDirection(i, p); // light's direction
		bool isBlocked = (L.x == 0 && L.y == 0 && L.z == 0); // for shadow

		for (int j = 0; j < objectsCount(); j++) {
			if (isLightBlockedBy(p, L, scn.objects[j], i)) {
				isBlocked = true;
			}
		}

		if (!isBlocked) {
			vec3 I = make3(scn.intensities[i]);

			vec3 R = normalize(reflect(L, N));
			R.x = -R.x;
			R.y = -R.y;
			R.z = -R.z;
			float vDotR = std::max(0.0f, dot(R, v));
			specular += (Ks * pow(vDotR, n)) * I;
		}
	}
	return cap(specular);
}

vec3 colorCalc(vec3 intersectionPoint, vec3 v, int raysLeft, int prevObject);

vec3 applyReflection(vec3 p, vec3 N, vec3 v, int raysLeft) {
	vec3 mirror = normalize(reflect(v, N));//bounce a ray off the object
	return colorCalc(p, mirror, raysLeft - 1, -1);
}

vec3 applyTransparency(vec3 p, vec3 N, vec3 v, vec4 obj, int raysLeft, int intersection) {

	// Check if the object is a sphere
	if (!isSphere(obj))
		return vec3(0.0f);

	// Extract the object's color and transparency
	vec3 objColor = make3(scn.colors[intersection]);
	float transparency = scn.objectProps[intersection].y;

	// Calculate angle
	float cos_theta = -dot(v, N);

	//entering or exiting
	vec3 normal = (cos_theta < 0.0f) ? -N : N;

	// Calculate the refractive index ratio
	float eta_ratio = (cos_theta < 0.0f) ? (AIR_REFRACTIVE_INDEX / SPHERE_REFRACTIVE_INDEX) : (SPHERE_REFRACTIVE_INDEX / AIR_REFRACTIVE_INDEX);

	//Snell's law 
	vec3 refracted_dir = refract(v, normal, eta_ratio);

	if (refracted_dir == vec3(0.0f)) {
		return objColor;
	}
	vec3 refractedColor = colorCalc(p, refracted_dir, raysLeft - 1, -1);
	return (1.0f - transparency) * objColor + transparency * refractedColor;
}

vec3 colorCalc(vec3 intersectionPoint, vec3 v, int raysLeft, int prevObject)
{
	if (raysLeft == 0) return vec3(0.0, 0.0, 0.0);
	float distance = 100000000;
	float t_obj;
	int intersectingObjectIndex = -1;
	// find intersection between the eye and an object.
	for (int i = 0; i < objectsCount(); i++) {
		t_obj = intersectionFunc(intersectionPoint, v, scn.objects[i]);
		if (t_obj < distance && t_obj >= epsilon) {
			distance = t_obj;
			intersectingObjectIndex = i;
		}
	}

	if (intersectingObjectIndex == -1) {
		return vec3(0.0, 0.0, 0.0);
	}


	// p represents the point in space of the relevant pixel.
	vec3 p = intersectionPoint + v * distance;

	vec3 intersectionColor = make3(scn.colors[intersectingObjectIndex]);
	vec3 N = getObjectNormal(intersectingObjectIndex, p);

	// planes are divided to squares. this boolean will determine the pixel color according to the square.
	if (!isSphere(scn.objects[intersectingObjectIndex]) && (!squareCoefficient(p)))
		intersectionColor = intersectionColor * 0.5f;

	//Combining the Phong Model
	vec3 result(0.0, 0.0, 0.0);
	if (scn.objectProps[intersectingObjectIndex].x < 1 && scn.objectProps[intersectingObjectIndex].y < 1) {
		// Ambient:
		result += applyAmbient(intersectionColor);
		// Diffuse:
		result += applyDiffuse(intersectingObjectIndex, p, N, intersectionColor);
		// Specular:
		result += applySpecular(intersectingObjectIndex, p, N, v);
	}
	// Reflection:
	if (scn.objectProps[intersectingObjectIndex].x > 0) result += applyReflection(p, N, v, raysLeft);
	// Transparency:
	if (scn.objectProps[intersectingObjectIndex].y > 0) {
		result += applyTransparency(p, N, v, scn.objects[intersectingObjectIndex], raysLeft, intersectingObjectIndex);
	}
	//clamping combined value
	return cap(result);
}

vec3 multiSamplePixel(vec3 intersectionPoint, vec3 imageCenter, int raysLeft, int x, int y, int width, int height, int numSamples) {
	vec3 totalColor(0.0f);

	for (int i = 0; i < numSamples; ++i) {
		// Calculate the ray direction for the current sub-pixel sample
		float offsetX = (x + (i / (float)numSamples)) / (float)width * 2.0f - 1.0f;
		float offsetY = 1.0f - (y + (i / (float)numSamples)) / (float)height * 2.0f;
		vec3 position = vec3(offsetX, offsetY, 0.0f);
		vec3 v = normalize(position - imageCenter);

		// Trace the ray and accumulate the color
		totalColor += colorCalc(intersectionPoint, v, raysLeft, -1);
	}

	// Return the average color
	return totalColor / (float)numSamples;
}

void renderScene(unsigned char* image, int width, int height) {
	vec3 imageCenter = make3(scn.eye);
	vec3 startingPos(-1.0, 1.0, 0.0);
	const int numSamples = 1;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			vec3 colors = multiSamplePixel(make3(scn.eye), imageCenter, 5, x, y, width, height, numSamples);
			image[(y * width + x) * 4] = (colors.x * 255.0);
			image[(y * width + x) * 4 + 1] = (colors.y * 255.0);
			image[(y * width + x) * 4 + 2] = (colors.z * 255.0);
			image[(y * width + x) * 4 + 3] = 255;
		}
	}
}

int main(int argc, char* argv[])
{
	const int DISPLAY_WIDTH = 800;
	const int DISPLAY_HEIGHT = 800;
	const float CAMERA_ANGLE = 0.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;

	Game* scn = new Game(CAMERA_ANGLE, (float)DISPLAY_WIDTH / DISPLAY_HEIGHT, NEAR, FAR);

	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	Init(display);
	scn->Init();
	display.SetScene(scn);
	int width = 800;
	int height = 800;
	unsigned char* scene = new unsigned char[width * height * 16];
	renderScene(scene, width, height); //This function call does most of the work- it gets an empty picture and returns the rendered picture
	scn->AddTexture(800, 800, scene);
	scn->SetShapeTex(0, 0);
	scn->CustomDraw(1, 0, scn->BACK, true, false, 0);

	scn->Motion();
	display.SwapBuffers();

	while (!display.CloseWindow())
	{

		display.PollEvents();

	}
	delete scn;
	return 0;
}