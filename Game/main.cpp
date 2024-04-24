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
scene scn("../res/scene.txt");

float dot(glm::vec3 v1, glm::vec3 v2) {
	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

//glm::vec3 cross(glm::vec3 v1, glm::vec3 v2) {

//}

float max(float a, float b) {
	if (a > b) return a;
	else return b;
}

float distance(glm::vec3 v1, glm::vec3 v2) {
	return sqrt((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y) + (v1.z - v2.z) * (v1.z - v2.z));
}

float length(const glm::vec3 vec) {
	return std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}

glm::vec3 normalize(const glm::vec3 vec) {
	float len = length(vec);
	if (len != 0.0f) {
		return glm::vec3(vec.x / len, vec.y / len, vec.z / len);
	}
	else {
		// Handling zero-length vectors to prevent division by zero
		return glm::vec3(0.0f, 0.0f, 0.0f);
	}
}

glm::vec3 reflect(glm::vec3 L, glm::vec3 N)
{
	return L - 2.f * dot(L, N) * N;
}

/*glm::vec3 refract(glm::vec3 incident, glm::vec3 normal, float eta) {
	float cosTheta = dot(-incident, normal); // Negative incident vector because it's pointing inwards
	float k = 1.0f - eta * eta * (1.0f - cosTheta * cosTheta);
	if (k < 0.0f) {
		return glm::vec3(0.0f, 0.0f, 0.0f); // Total internal reflection, return zero vector
	}
	else {
		return eta * incident + (eta * cosTheta - std::sqrt(k)) * normal;
	}
}*/

glm::vec3 refract(glm::vec3 incidentVec, glm::vec3 normal, float eta)
{
	float N_dot_I = dot(normal, incidentVec);
	float k = 1.f - eta * eta * (1.f - N_dot_I * N_dot_I);
	if (k < 0.f)
		return glm::vec3(0.f, 0.f, 0.f);
	else
		return eta * incidentVec - (eta * N_dot_I + sqrtf(k)) * normal;
}

glm::vec3 cap(glm::vec3 vec) {
	if (vec.x < 0) vec.x = 0;
	if (vec.y < 0) vec.y = 0;
	if (vec.z < 0) vec.z = 0;
	if (vec.x > 1) vec.x = 1;
	if (vec.y > 1) vec.y = 1;
	if (vec.z > 1) vec.z = 1;
	return vec;
}

glm::vec3 make3(glm::vec4 v) { //cuts out the last dimension of a 4d vector
	return glm::vec3(v.x, v.y, v.z);
}

float sphereIntersection(glm::vec3 p0, glm::vec3 v, glm::vec4 sphere) {
	glm::vec3 o = make3(sphere);
	float r = sphere.w;
	glm::vec3 d = p0 - o;
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
		return min(x1, x2);
	}
	else if (x1 < epsilon && x2 < epsilon) {
		return -1.0;
	}
	else return max(x1, x2);
}

float planeIntersection(glm::vec3 p0, glm::vec3 v, glm::vec4 plane) {//p0-camera position, v-curret ray, plane- curret plane object checked for intersection
	glm::vec3 N = make3(plane);
	float d = plane.w;
	glm::vec3 q = N * (-d);
	float t;
	if (dot(N, v) != 0) t = -((dot(p0, N) + d) / dot(v, N));

	else t = -1;

	if (t < epsilon) {
		return -1.0;
	}
	else {
		return t;
	}
}

bool isSphere(glm::vec4 obj) {
	return obj.w > 0;
}

bool isDirectionalLight(glm::vec4 light) {
	return light.w == 0.0;
}

bool squareCoefficient(glm::vec3 p) {
	bool resx = (0 < p.x - round(p.x) < 0.5);
	bool resy = (0 < p.y - round(p.y) < 0.5);
	return (resx == resy);
}

float intersectionFunc(glm::vec3 p0, glm::vec3 v, glm::vec4 obj) {
	if (isSphere(obj)) {
		return sphereIntersection(p0, v, obj);
	}
	else {
		return planeIntersection(p0, v, obj);
	}
}

int lightsCount() {
	return scn.sizes.y;
}

int objectsCount() {
	return scn.sizes.x;
}

bool isLightBlockedBySphere(glm::vec3 p, glm::vec3 dir, glm::vec4 sphere, int light) {
	glm::vec3 sphereCenter = glm::vec3(sphere.x, sphere.y, sphere.z);
	float sphereRadius = sphere.w;
	glm::vec3 dirToSphere = sphereCenter - p;
	float t;
	if (glm::dot(dir, dirToSphere) < 0.0) {
		return false;
	}
	else {
		float closestApproachDist = glm::dot(dirToSphere, dir);
		glm::vec3 closestPointOnRay = p + closestApproachDist * dir;
		float distToSphereCenter = glm::distance(closestPointOnRay, sphereCenter);

		if (distToSphereCenter <= sphereRadius) {
			return true; // Light is blocked
		}
		else {
			return false;
		}
	}
}

bool isLightBlockedByPlane(glm::vec3 p, glm::vec3 dir, glm::vec4 plane, int light) {
	return false;
}

/*
	Check if light with direction -dir to point p is blocked by object obj.
*/
bool isLightBlockedBy(glm::vec3 p, glm::vec3 dir, glm::vec4 obj, int light) {
	if (isSphere(obj)) {
		return isLightBlockedBySphere(p, -dir, obj, light);
	}
	else {
		return isLightBlockedByPlane(p, dir, obj, light);
	}
}

glm::vec3 getObjectNormal(int i, glm::vec3 p) {
	return normalize(isSphere(scn.objects[i]) ? glm::vec3(scn.objects[i].x - p.x, scn.objects[i].y - p.y, scn.objects[i].z - p.z) : glm::vec3(scn.objects[i].x, scn.objects[i].y, scn.objects[i].z));
}

glm::vec3 getLightDirection(int i, glm::vec3 p) {
	glm::vec3 L = glm::vec3(0.0, 0.0, 0.0);
	if (isDirectionalLight(scn.directions[i])) {
		L = normalize(glm::vec3(scn.directions[i].x, scn.directions[i].y, scn.directions[i].z));
	}
	else if (scn.lights[i].w < dot(normalize(make3(scn.directions[i])), normalize(glm::vec3(p.x - scn.lights[i].x, p.y - scn.lights[i].y, p.z - scn.lights[i].z)))) {
		L = normalize(p - make3(scn.lights[i]));
	}

	return L;
}

glm::vec3 applyAmbient(glm::vec3 intersectionColor) {
	return cap(make3(scn.ambient) * intersectionColor);
}

glm::vec3 applyDiffuse(int intersection, glm::vec3 p, glm::vec3 N, glm::vec3 intersectionColor) {
	glm::vec3 diffuse = glm::vec3(0.0, 0.0, 0.0);

	for (int i = 0; i < lightsCount(); i++) {
		glm::vec3 L = getLightDirection(i, p); // light's direction
		bool isBlocked = (L.x == 0 && L.y == 0 && L.z == 0); // for shadow

		for (int j = 0; j < objectsCount(); j++) {
			if (isLightBlockedBy(p, L, scn.objects[j], i)) {//shadow error probably here
				isBlocked = true;
			}
		}

		if (!isBlocked) {//always not blocked for some reason
			glm::vec3 I(make3(scn.intensities[i]));

			diffuse += (intersectionColor * dot(N, L)) * I;
		}
	}

	return cap(diffuse);
}

glm::vec3 applySpecular(int intersection, glm::vec3 p, glm::vec3 N, glm::vec3 v) {
	glm::vec3 specular = glm::vec3(0.0, 0.0, 0.0);
	glm::vec3 Ks = glm::vec3(0.7, 0.7, 0.7);
	float n = scn.colors[intersection].w;

	for (int i = 0; i < lightsCount(); i++) {
		glm::vec3 L = getLightDirection(i, p); // light's direction
		bool isBlocked = (L.x == 0 && L.y == 0 && L.z == 0); // for shadow

		for (int j = 0; j < objectsCount(); j++) {
			if (isLightBlockedBy(p, L, scn.objects[j], i)) {
				isBlocked = true;
			}
		}

		if (!isBlocked) {
			glm::vec3 I = make3(scn.intensities[i]);

			glm::vec3 R = normalize(reflect(L, N));
			R.x = -R.x;
			R.y = -R.y;
			R.z = -R.z;
			float vDotR = max(0, dot(R, v));
			specular += (Ks * pow(vDotR, n)) * I;
		}
	}
	return cap(specular);
}

glm::vec3 colorCalc(glm::vec3 intersectionPoint, glm::vec3 v, int raysLeft, int prevObject);

glm::vec3 applyReflection(glm::vec3 p, glm::vec3 N, glm::vec3 v, int raysLeft) {
	glm::vec3 mirror = normalize(reflect(v, N));//bounce a ray off the object
	return colorCalc(p, mirror, raysLeft - 1, -1);
}

glm::vec3 applyTransparency(glm::vec3 p, glm::vec3 N, glm::vec3 v, glm::vec4 obj, int raysLeft, int intersection) {

	// Check if the object is a sphere
	if (!isSphere(obj)) {
		return glm::vec3(0.0f);
	}

	// Extract the object's color and transparency
	glm::vec3 objColor = make3(scn.colors[intersection]);
	float transparency = scn.objectProps[intersection].y;

	// Calculate angle
	float cos_theta = -glm::dot(v, N);

	//entering or exiting
	glm::vec3 normal = (cos_theta < 0.0f) ? -N : N;

	// Calculate the refractive index ratio
	float eta_ratio = (cos_theta < 0.0f) ? (AIR_REFRACTIVE_INDEX / SPHERE_REFRACTIVE_INDEX) : (SPHERE_REFRACTIVE_INDEX / AIR_REFRACTIVE_INDEX);

	//Snell's law 
	glm::vec3 refracted_dir = glm::refract(v, normal, eta_ratio);

	if (refracted_dir == glm::vec3(0.0f)) {
		return objColor;
	}
	glm::vec3 refractedColor = colorCalc(p, refracted_dir, raysLeft - 1, -1);
	return (1.0f - transparency) * objColor + transparency * refractedColor;
}

glm::vec3 colorCalc(glm::vec3 intersectionPoint, glm::vec3 v, int raysLeft, int prevObject)
{
	if (raysLeft == 0) return glm::vec3(0.0, 0.0, 0.0);
	float distance = 100000000;
	float t_obj;
	int intersection = -1;
	// find intersection between the eye and an object.
	for (int i = 0; i < objectsCount(); i++) {
		t_obj = intersectionFunc(intersectionPoint, v, scn.objects[i]);
		if (t_obj < distance && t_obj >= epsilon) {
			distance = t_obj;
			intersection = i;
		}
	}

	if (intersection == -1) {
		return glm::vec3(0.0, 0.0, 0.0);
	}


	// p represents the point in space of the relevant pixel.
	glm::vec3 p = intersectionPoint + v * distance;

	glm::vec3 intersectionColor = make3(scn.colors[intersection]);
	glm::vec3 N = getObjectNormal(intersection, p);

	// planes are divided to squares. this boolean will determine the pixel color according to the square.
	float coefficient;
	if (!isSphere(scn.objects[intersection]) && (squareCoefficient(p)))
		coefficient = 1.0;
	else coefficient = 0.5;
	if (!isSphere(scn.objects[intersection])) intersectionColor = intersectionColor * coefficient;
	if (scn.objectProps[intersection].x > 0) {
		intersectionColor = glm::vec3(0.0, 0.0, 0.0);
		coefficient = 1.0;
	}


	/******** Lighting + Phong Model *********/
	glm::vec3 result(0.0, 0.0, 0.0);
	if (scn.objectProps[intersection].x < 1 && scn.objectProps[intersection].y < 1) {
		// Ambient:
		result += applyAmbient(intersectionColor);
		// Diffuse:
		result += applyDiffuse(intersection, p, N, intersectionColor); //some problem with shadows here
		// Specular:
		result += applySpecular(intersection, p, N, v);
	}
	// Reflection:
	if (scn.objectProps[intersection].x > 0) result += applyReflection(p, N, v, raysLeft);
	// Transparency:
	if (scn.objectProps[intersection].y > 0) {
		result += applyTransparency(p, N, v, scn.objects[intersection], raysLeft, intersection);
	}
	//clamping
	return cap(result);
}

glm::vec3 multiSamplePixel(glm::vec3 intersectionPoint, glm::vec3 imageCenter, int raysLeft, int x, int y, int width, int height, int numSamples) {
	glm::vec3 totalColor(0.0f);

	for (int i = 0; i < numSamples; ++i) {
		// Calculate the ray direction for the current sub-pixel sample
		float offsetX = (x + (i / (float)numSamples)) / (float)width * 2.0f - 1.0f;
		float offsetY = 1.0f - (y + (i % numSamples)) / (float)height * 2.0f;
		glm::vec3 position = glm::vec3(offsetX, offsetY, 0.0f);
		glm::vec3 v = normalize(position - imageCenter);

		// Trace the ray and accumulate the color
		totalColor += colorCalc(intersectionPoint, v, raysLeft, -1);
	}

	// Return the average color
	return totalColor / (float)numSamples;
}

void renderScene(unsigned char* image, int width, int height) {
	glm::vec3 imageCenter = make3(scn.eye);
	glm::vec3 startingPos(-1.0, 1.0, 0.0);
	const int numSamples = 4;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			glm::vec3 colors = multiSamplePixel(make3(scn.eye), imageCenter, 5, x, y, width, height, numSamples);
			image[(y * width + x) * 4] = (colors.x * 255.0);
			image[(y * width + x) * 4 + 1] = (colors.y * 255.0);
			image[(y * width + x) * 4 + 2] = (colors.z * 255.0);
			image[(y * width + x) * 4 + 3] = 255;
		}
	}
}
//void renderScene(unsigned char* image, int width, int height) {
//	glm::vec3 imageCenter = make3(scn.eye);
//	glm::vec3 startingPos(-1.0, 1.0, 0.0);
//	for (int y = 0; y < height; y++) {
// 		for (int x = 0; x < width; x++) {
//			
//			glm::vec3 position1 = startingPos;
//			position1.x = startingPos.x + ((2.0 / (float) width) * 0.5) + (x * 2.0 / (float) width);
//			position1.y = startingPos.y - ((2.0 / (float) height) * 0.5) - (y * 2.0 /(float)height);
//			glm::vec3 v = normalize(position1 - imageCenter);
// 			glm::vec3 colors = colorCalc(make3(scn.eye),v,5, -1);
// 			image[(y * width + x) * 4] = (colors.x * 255.0);
// 			image[(y * width + x) * 4 + 1] = (colors.y * 255.0);
// 			image[(y * width + x) * 4 + 2] = (colors.z * 255.0);
// 			image[(y * width + x) * 4 + 3] = 255;
// 		}
// 	}
// }

int main(int argc, char* argv[])
{
	const int DISPLAY_WIDTH = 800;
	const int DISPLAY_HEIGHT = 800;
	const float CAMERA_ANGLE = 45.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;

	Game* scn = new Game(CAMERA_ANGLE, (float)DISPLAY_WIDTH / DISPLAY_HEIGHT, NEAR, FAR);

	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	Init(display);
	scn->Init();
	display.SetScene(scn);
	int width = 800;
	int height = 800;
	unsigned char* scene = new unsigned char[width * height * 16]; // Assuming 512x512 size
	//renderScene(scene, width, height);

	scn->AddTexture(800, 800, scene);

	scn->SetShapeTex(0, 0);
	while (!display.CloseWindow())
	{
		scn->CustomDraw(1, 0, scn->BACK, true, false, 0);
		scn->Motion();
		display.SwapBuffers();
		display.PollEvents();

	}
	delete scn;
	return 0;
}