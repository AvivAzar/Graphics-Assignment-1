#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
using namespace glm;
# define M_PI           3.14159265358979323846
enum Color { YELLOW, BLUE, PINK, GREEN, CYAN, RED };
class Transform {
public:
	glm::vec3 translation;
	glm::vec3 rotation;
	glm::vec3 scale;
	Transform() : translation(0.0f), rotation(0.0f), scale(1.0f) {}
};
struct miniCube {
	int index; // Unique index for identifying the cube
	Transform transform; // Transformation information for the cube
	Color color; // Color of the cube

	miniCube(int idx) : index(idx) {}
};
std::vector <miniCube> cubeMatrix; // Representation of the Rubik's cube
std::vector<int> rightWallCubes;
std::vector<int> leftWallCubes;
std::vector<int> upWallCubes;
std::vector<int> downWallCubes;
std::vector<int> frontWallCubes;
std::vector<int> backWallCubes;
float angle = 45.0f;
char lock = 'u';

int size = 3; //size of cube

static void printMat(const glm::mat4 mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat[j][i] << " ";
		std::cout << std::endl;
	}
}

Game::Game() : Scene()
{

}

Game::Game(float angle, float relationWH, float near1, float far1) : Scene(angle, relationWH, near1, far1)
{
}

float rad(float degrees) {
	return degrees * M_PI / 180;
}

glm::vec3 getPosShift(glm::vec3 pos, glm::mat3 rotMat) {
	return (rotMat * pos)-pos;
}

void updateWalls() {
	rightWallCubes.clear();
	leftWallCubes.clear();
	upWallCubes.clear();
	downWallCubes.clear();
	frontWallCubes.clear();
	backWallCubes.clear();
	for (int i = 0; i < size * size * size; i++) {
				if (cubeMatrix[i].transform.translation.x > 0.99f * (float)(size/2)) rightWallCubes.push_back(cubeMatrix[i].index); 
				if (cubeMatrix[i].transform.translation.x < -0.99f * (float)(size / 2)) leftWallCubes.push_back(cubeMatrix[i].index);
				if (cubeMatrix[i].transform.translation.y > 0.99f * (float)(size / 2)) upWallCubes.push_back(cubeMatrix[i].index);
				if (cubeMatrix[i].transform.translation.y < -0.99f * (float)(size / 2)) downWallCubes.push_back(cubeMatrix[i].index);
				if (cubeMatrix[i].transform.translation.z > 0.99f * (float)(size / 2)) frontWallCubes.push_back(cubeMatrix[i].index);
				if (cubeMatrix[i].transform.translation.z < -0.99f * (float)(size / 2)) backWallCubes.push_back(cubeMatrix[i].index);
	}
}

bool isOriented() {
	for (int i = 0; i < size * size * size; i++) {
				if (mod(cubeMatrix[i].transform.rotation.x,90.0f) > 0.1f && mod(cubeMatrix[i].transform.rotation.x, 90.0f) < 89.9f) return false;
				if (mod(cubeMatrix[i].transform.rotation.y, 90.0f) > 0.1f && mod(cubeMatrix[i].transform.rotation.y, 90.0f) < 89.9f) return false;
				if (mod(cubeMatrix[i].transform.rotation.z, 90.0f) > 0.1f && mod(cubeMatrix[i].transform.rotation.z, 90.0f) < 89.9f) return false;
	}
	updateWalls();
	return true;
}

glm::mat3 rotMatX(float angle) {
	glm::mat3 rotMat(1);
	rotMat[1][1] = cos(rad(angle));
	rotMat[2][1] = -sin(rad(angle));
	rotMat[1][2] = sin(rad(angle));
	rotMat[2][2] = cos(rad(angle));
	return rotMat;
}

glm::mat3 rotMatY(float angle) {
	glm::mat3 rotMat(1);
	rotMat[0][0] = cos(rad(angle));
	rotMat[2][0] = sin(rad(angle));
	rotMat[0][2] = -sin(rad(angle));
	rotMat[2][2] = cos(rad(angle));
	return rotMat;
}

glm::mat3 rotMatZ(float angle) {
	glm::mat3 rotMat(1);
	rotMat[0][0] = cos(rad(angle));
	rotMat[1][0] = -sin(rad(angle));
	rotMat[0][1] = sin(rad(angle));
	rotMat[1][1] = cos(rad(angle));
	return rotMat;
}

void rotateMiniCube(mat3 rotMat, int cubeIndex, vec3 rotVec, Shape *shape) {
	//tracking cube positions
	vec3 posShift = getPosShift(cubeMatrix[cubeIndex].transform.translation, rotMat);
	cubeMatrix[cubeIndex].transform.translation += posShift;
	
	//rotation
	glm::mat4 tRot = glm::transpose(shape->getRotMat());
	rotVec = glm::mat3(tRot) * rotVec;
	shape->MyRotate(angle, rotVec, 0);
	cubeMatrix[cubeIndex].transform.rotation += rotVec * angle;
}

void Game::RotateWall(char wallId, bool clockwise)
{
	if (wallId == 'R') {
		if (lock == 'u' || lock == 'x') {

			glm::mat3 rotMat=rotMatX(angle);
			glm::vec3 rotVec(1.0f, 0.0f, 0.0f);

			for (int i = 0; i < size * size; i++)
				rotateMiniCube(rotMat, rightWallCubes[i], rotVec, shapes[rightWallCubes[i]]);

			
			if (!isOriented()) lock = 'x';
			else lock = 'u';
		}
	}

	if (wallId == 'L') {
		if (lock == 'u' || lock == 'x') {

			glm::mat3 rotMat = rotMatX(angle);
			glm::vec3 rotVec(1.0f, 0.0f, 0.0f);

			for (int i = 0; i < size * size; i++)
				rotateMiniCube(rotMat, leftWallCubes[i], rotVec, shapes[leftWallCubes[i]]);

			if (!isOriented()) lock = 'x';
			else lock = 'u';
		}
	}

	if (wallId == 'U') {
		if (lock == 'u' || lock == 'y') {

			glm::mat3 rotMat = rotMatY(angle);
			glm::vec3 rotVec(0.0f, 1.0f, 0.0f);

			for (int i = 0; i < size * size; i++)
				rotateMiniCube(rotMat, upWallCubes[i], rotVec, shapes[upWallCubes[i]]);

			if (!isOriented()) lock = 'y';
			else lock = 'u';
		}
	}

	if (wallId == 'D') {
		if (lock == 'u' || lock == 'y') {

			glm::mat3 rotMat = rotMatY(angle);
			glm::vec3 rotVec(0.0f, 1.0f, 0.0f);

			for (int i = 0; i < size * size; i++)
				rotateMiniCube(rotMat, downWallCubes[i], rotVec, shapes[downWallCubes[i]]);

			if (!isOriented()) lock = 'y';
			else lock = 'u';
		}
	}

	if (wallId == 'B') {
		if (lock == 'u' || lock == 'z') {
			glm::mat3 rotMat = rotMatZ(angle);
			glm::vec3 rotVec(0.0f, 0.0f, 1.0f);

			for (int i = 0; i < size * size; i++)
				rotateMiniCube(rotMat, backWallCubes[i], rotVec, shapes[backWallCubes[i]]);

			if (!isOriented()) lock = 'z';
			else lock = 'u';
		}
	}

	if (wallId == 'F') {
		if (lock == 'u' || lock == 'z') {

			glm::mat3 rotMat = rotMatZ(angle);
			glm::vec3 rotVec(0.0f, 0.0f, 1.0f);

			for (int i = 0; i < size * size; i++)
				rotateMiniCube(rotMat, frontWallCubes[i], rotVec, shapes[frontWallCubes[i]]);

			if (!isOriented()) lock = 'z';
			else lock = 'u';
		}
	}

}

void Game::FlipRotationDirection()
{
	angle = -angle;
}

void Game::DivideRotationAngle()
{
	angle = angle / 2;
}

void Game::MultiplyRotationAngle()
{
	angle = angle * 2;
	if (angle > 180) angle = 180;
	if (angle < -180) angle = -180;
}
void Game::Init()
{
	AddShader("../res/shaders/pickingShader");
	AddShader("../res/shaders/basicShader");

	AddTexture("../res/textures/plane.png", false);

	float distance = 0;
	if (size%2==1) distance = (size / 2);
	int index = 0;
	
	for (float i = -distance; i <= distance; i += 1) {
		for (float j = -distance; j <= distance; j += 1) {
			for (float k = -distance; k <= distance; k += 1) {
				AddShape(Scene::Cube, -1, Scene::TRIANGLES);
				SetShapeTex(index, 0);
				shapes[index]->MyTranslate(glm::vec3(i, j, k), 0);
				shapes[index]->MyScale(glm::vec3(0.5, 0.5, 0.5));
	
					if (i == distance)
						rightWallCubes.push_back(index);
					if (i == -distance)
						leftWallCubes.push_back(index);
					if (j == distance)
						upWallCubes.push_back(index);
					if (j == -distance)
						downWallCubes.push_back(index);
					if (k == distance)
						frontWallCubes.push_back(index);
					if (k == -distance)
						backWallCubes.push_back(index);

					miniCube curr {index};
					curr.transform = Transform();
					curr.transform.translation = glm::vec3(i, j, k);

					cubeMatrix.push_back (curr);
					index++;
			}
		}
	}
	MoveCamera(0, zTranslate, 10);
}

void Game::Update(const glm::mat4& MVP, const glm::mat4& Model, const int  shaderIndx)
{
	Shader* s = shaders[shaderIndx];
	int r = ((pickedShape + 1) & 0x000000FF) >> 0;
	int g = ((pickedShape + 1) & 0x0000FF00) >> 8;
	int b = ((pickedShape + 1) & 0x00FF0000) >> 16;
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal", Model);
	s->SetUniform4f("lightDirection", 0.0f, 0.0f, -1.0f, 0.0f);
	if (shaderIndx == 0)
		s->SetUniform4f("lightColor", r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);
	else
		s->SetUniform4f("lightColor", 0.7f, 0.8f, 0.1f, 1.0f);
	s->Unbind();
}

void Game::WhenRotate()
{
}

void Game::WhenTranslate()
{
}

void Game::Motion()
{
	if (isActive)
	{
	}
}

Game::~Game(void)
{
}