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
struct Move
{
	int wall;
	float angle;

	char direction;

	Move(int w, float a, char d) : wall(w), angle(a), direction(d) {}
};
std::vector <Move> move;
std::vector <miniCube> cubeMatrix; // Representation of the Rubik's cube
int rubix[5][5][5];
float rotationInfo[3][5];
float angle = 90.0f;
char lock = 'u';
int wallIndex = 0;
int size = 3; //size of cube
std::vector<int> rightWallCubes;
std::vector<int> leftWallCubes;
std::vector<int> upWallCubes;
std::vector<int> downWallCubes;
std::vector<int> frontWallCubes;
std::vector<int> backWallCubes;

static void initRotationInfo() {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 5; j++)
			rotationInfo[i][j] = 0;
}
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
	return (rotMat * pos) - pos;
}
void rotateSliceX(int slice, int sign) {

	int temp[5][5];
	for (int l = 0; l < size; l++)
		for (int m = 0; m < size; m++) {
			temp[l][m] = rubix[slice][l][m];
		}
	for (int l = 0; l < size; l++)
		for (int m = 0; m < size; m++) {
			if (sign == 1) { // Clockwise rotation
				rubix[slice][l][m] = temp[m][size - l - 1];

			}
			else { // Counter-clockwise rotation
				rubix[slice][l][m] = temp[size - m - 1][l];
			}
		}
}
void rotateSliceY(int slice, int sign) {

	int temp[5][5];
	for (int l = 0; l < size; l++)
		for (int m = 0; m < size; m++) {
			temp[l][m] = rubix[l][slice][m];
		}
	for (int l = 0; l < size; l++)
		for (int m = 0; m < size; m++) {
			if (sign == 1) { // Clockwise rotation
				rubix[l][slice][m] = temp[m][size - l - 1];

			}
			else { // Counter-clockwise rotation
				rubix[l][slice][m] = temp[size - m - 1][l];
			}
		}
}

void rotateSliceZ(int slice, int sign) {
	int temp[5][5];
	for (int l = 0; l < size; l++)
		for (int m = 0; m < size; m++) {
			temp[l][m] = rubix[l][m][slice];
		}
	for (int l = 0; l < size; l++)
		for (int m = 0; m < size; m++) {
			if (sign == 1) { // Clockwise rotation
				rubix[l][m][slice] = temp[m][size - l - 1];

			}
			else { // Counter-clockwise rotation
				rubix[l][m][slice] = temp[size - m - 1][l];
			}
		}
}
void updateWalls() {
	//// Print rotationInfo
	//for (int i = 0; i < 3; i++) {
	//	std::cout << std::endl;
	//	for (int j = 0; j < size; j++) {
	//		std::cout << rotationInfo[i][j] << " ";
	//	}
	//	
	//}
	//// print all rubix
	//std::cout <<"b" << std::endl;

	//for (int i = 0; i < size; i++) {
	//	std::cout << std::endl;
	//	for (int j = 0; j < size; j++) {
	//		for (int k = 0; k < size; k++) {
	//			std::cout << rubix[i][j][k] << " ";
	//		}
	//		std::cout << std::endl;
	//	}
	//}
	//std::cout << std::endl;

	//for (int axis = 0; axis < 3; axis++) {
	//	for (int face = 0; face < size; face++) {
	//		if (rotationInfo[axis][face] != 0) {
	//			float numRot = rotationInfo[axis][face] / 90.0f;
	//			int sign = (numRot > 0) ? 1 : -1;
	//			numRot = std::abs(numRot);

	//			for (int k = 0; k < numRot; k++) {
	//				switch (axis) {
	//				case 0: // X-axis rotation
	//					/*for (int l = 0; l < size; ++l) {
	//						for (int m = 0; m < size; ++m) {
	//							std::cout << rubix[face][l][m] << " ";
	//						}
	//						std::cout << std::endl;
	//					}
	//					std::cout << std::endl;*/

	//					rotateSliceX(face, sign);

	///*					for (int l = 0; l < size; ++l) {
	//						for (int m = 0; m < size; ++m) {
	//							std::cout << rubix[face][l][m] << " ";
	//						}
	//						std::cout << std::endl;
	//					}*/
	//					std::cout << std::endl;
	//					break;

	//				case 1: // Y-axis rotation
	//	/*				for (int l = 0; l < size; ++l) {
	//						for (int m = 0; m < size; ++m) {
	//							std::cout << rubix[l][face][m] << " ";
	//						}
	//						std::cout << std::endl;
	//					}*/
	//					std::cout << std::endl;

	//					rotateSliceY(face, sign);

	//	/*				for (int l = 0; l < size; ++l) {
	//						for (int m = 0; m < size; ++m) {
	//							std::cout << rubix[l][face][m] << " ";
	//						}
	//						std::cout << std::endl;
	//					}*/
	//					std::cout << std::endl;
	//					break;

	//				case 2: // Z-axis rotation
	//			/*		for (int l = 0; l < size; ++l) {
	//						for (int m = 0; m < size; ++m) {
	//							std::cout << rubix[l][m][face] << " ";
	//						}
	//						std::cout << std::endl;
	//					}*/
	//					std::cout << std::endl;

	//					rotateSliceZ(face, sign);

	//					/*for (int l = 0; l < size; ++l) {
	//						for (int m = 0; m < size; ++m) {
	//							std::cout << rubix[l][m][face] << " ";
	//						}
	//						std::cout << std::endl;
	//					}*/
	//					std::cout << std::endl;
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}
	//std::cout<< "A" << std::endl;

	//for (int i = 0; i < size; i++) {
	//	std::cout << std::endl;
	//	for (int j = 0; j < size; j++) {
	//		for (int k = 0; k < size; k++) {
	//			std::cout << rubix[i][j][k] << " ";
	//		}
	//		std::cout << std::endl;
	//	}
	//}
	std::cout << std::endl;
	rightWallCubes.clear();
	leftWallCubes.clear();
	upWallCubes.clear();
	downWallCubes.clear();
	frontWallCubes.clear();
	backWallCubes.clear();
	for (int i = 0; i < size * size * size; i++) {
		if (cubeMatrix[i].transform.translation.x > 0.99f * (float)(size / 2)) rightWallCubes.push_back(cubeMatrix[i].index);
		if (cubeMatrix[i].transform.translation.x < -0.99f * (float)(size / 2)) leftWallCubes.push_back(cubeMatrix[i].index);
		if (cubeMatrix[i].transform.translation.y > 0.99f * (float)(size / 2)) upWallCubes.push_back(cubeMatrix[i].index);
		if (cubeMatrix[i].transform.translation.y < -0.99f * (float)(size / 2)) downWallCubes.push_back(cubeMatrix[i].index);
		if (cubeMatrix[i].transform.translation.z > 0.99f * (float)(size / 2)) frontWallCubes.push_back(cubeMatrix[i].index);
		if (cubeMatrix[i].transform.translation.z < -0.99f * (float)(size / 2)) backWallCubes.push_back(cubeMatrix[i].index);
	}

	initRotationInfo();

}



bool isOriented() {
	for (int i = 0; i < 3; i++)
		for (int j = 1; j < size; j++)
			if (mod(rotationInfo[i][0], 90.0f) != mod(rotationInfo[i][j], 90.0f))

				return false;
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

void rotateMiniCube(mat3 rotMat, int i, int j, int k, vec3 rotVec, Shape* shape) {
	//tracking cube positions
	std::cout << rubix[i][j][k] << " ";
	std::cout << cubeMatrix[rubix[i][j][k]].index << std::endl;

	vec3 posShift = getPosShift(cubeMatrix[rubix[i][j][k]].transform.translation, rotMat);
	cubeMatrix[rubix[i][j][k]].transform.translation += posShift;

	//rotation
	glm::mat4 tRot = glm::transpose(shape->getRotMat());
	rotVec = glm::mat3(tRot) * rotVec;
	shape->MyRotate(angle, rotVec, 0);
	cubeMatrix[rubix[i][j][k]].transform.rotation += rotVec * angle;
}
void rotateMiniCube(mat3 rotMat, int cubeIndex, vec3 rotVec, Shape* shape) {
	//tracking cube positions
	vec3 posShift = getPosShift(cubeMatrix[cubeIndex].transform.translation, rotMat);
	cubeMatrix[cubeIndex].transform.translation += posShift;

	//rotation
	glm::mat4 tRot = glm::transpose(shape->getRotMat());
	rotVec = glm::mat3(tRot) * rotVec;
	shape->MyRotate(angle, rotVec, 0);
	cubeMatrix[cubeIndex].transform.rotation += rotVec * angle;
}

void Game::RotateWall(char wallId)
{
	std::cout << '\n';

	if (wallId == 'R') {
		if (lock == 'u' || lock == 'x') {

			glm::mat3 rotMat = rotMatX(angle);
			glm::vec3 rotVec(1.0f, 0.0f, 0.0f);
			rotationInfo[0][size - 1 - wallIndex] += angle;
			std::cout << "Rotation info: " << rotationInfo[0][size - 1 - wallIndex] << std::endl;
			//for (int i = 0; i < size; i++)
			//	for (int j = 0; j < size; j++)
			for (int i = 0; i < size * size; i++)
				/*rotateMiniCube(rotMat, size- 1 -wallIndex,j,i , rotVec, shapes[rubix[size - 1 - wallIndex][j][i]]);*/
				rotateMiniCube(rotMat, rightWallCubes[i], rotVec, shapes[rightWallCubes[i]]);

			if (!isOriented()) lock = 'x';
			else {
				move.push_back(Move(wallIndex, angle, 'R'));
				lock = 'u';
			}
			std::cout << '\n';
		}
	}

	if (wallId == 'L') {
		if (lock == 'u' || lock == 'x') {

			glm::mat3 rotMat = rotMatX(angle);
			glm::vec3 rotVec(1.0f, 0.0f, 0.0f);
			rotationInfo[0][wallIndex] += angle;
			std::cout << "Rotation info: " << rotationInfo[0][wallIndex] << std::endl;
			//for (int i = 0; i < size; i++)
			//	for (int j = 0; j < size; j++)
			for (int i = 0; i < size * size; i++)

				//rotateMiniCube(rotMat, 0 + wallIndex,j,i, rotVec, shapes[rubix[0 + wallIndex][j][i]]);
				rotateMiniCube(rotMat, leftWallCubes[i], rotVec, shapes[leftWallCubes[i]]);

			if (!isOriented()) lock = 'x';
			else {
				move.push_back(Move(wallIndex, angle, 'L'));
				lock = 'u';
			}
		}
	}

	if (wallId == 'U') {
		if (lock == 'u' || lock == 'y') {

			glm::mat3 rotMat = rotMatY(angle);
			glm::vec3 rotVec(0.0f, 1.0f, 0.0f);
			rotationInfo[1][size - 1 - wallIndex] += angle;
			std::cout << "Rotation info: " << rotationInfo[1][size - 1 - wallIndex] << std::endl;
			//for (int i = 0; i < size; i++)
			//	for (int j = 0; j < size; j++)
			for (int i = 0; i < size * size; i++)
				//rotateMiniCube(rotMat, j,size - 1 - wallIndex,i, rotVec, shapes[rubix[j][size - 1 - wallIndex][i]]);
				rotateMiniCube(rotMat, upWallCubes[i], rotVec, shapes[upWallCubes[i]]);

			if (!isOriented()) lock = 'y';
			else {
				move.push_back(Move(wallIndex, angle, 'U'));
				lock = 'u';
			}
		}
	}

	if (wallId == 'D') {
		if (lock == 'u' || lock == 'y') {

			glm::mat3 rotMat = rotMatY(angle);
			glm::vec3 rotVec(0.0f, 1.0f, 0.0f);
			rotationInfo[1][wallIndex] += angle;
			std::cout << "Rotation info: " << rotationInfo[1][wallIndex] << std::endl;
			//for (int i = 0; i < size; i++)
			//	for (int j = 0; j < size; j++)
			for (int i = 0; i < size * size; i++)
				//		rotateMiniCube(rotMat, j,0 + wallIndex,i, rotVec, shapes[rubix[j][0 + wallIndex][i]]);
				rotateMiniCube(rotMat, downWallCubes[i], rotVec, shapes[downWallCubes[i]]);


			if (!isOriented()) lock = 'y';
			else {
				move.push_back(Move(wallIndex, angle, 'D'));
				lock = 'u';
			}
		}
	}

	if (wallId == 'B') {
		if (lock == 'u' || lock == 'z') {
			glm::mat3 rotMat = rotMatZ(angle);
			glm::vec3 rotVec(0.0f, 0.0f, 1.0f);
			rotationInfo[2][size - 1 - wallIndex] += angle;
			std::cout << "Rotation info: " << rotationInfo[2][size - 1 - wallIndex] << std::endl;
			//for (int i = 0; i < size; i++)
			//	for (int j = 0; j < size; j++)
			for (int i = 0; i < size * size; i++)
				//rotateMiniCube(rotMat, j,i,0 + wallIndex, rotVec, shapes[rubix[j][i][0 + wallIndex]]);
				rotateMiniCube(rotMat, backWallCubes[i], rotVec, shapes[backWallCubes[i]]);

			if (!isOriented()) lock = 'z';
			else {
				lock = 'u';
				move.push_back(Move(wallIndex, angle, 'B'));
			}
		}
	}

	if (wallId == 'F') {
		if (lock == 'u' || lock == 'z') {

			glm::mat3 rotMat = rotMatZ(angle);
			glm::vec3 rotVec(0.0f, 0.0f, 1.0f);
			rotationInfo[2][wallIndex] += angle;
			std::cout << "Rotation info: " << rotationInfo[2][wallIndex] << std::endl;
			//for (int i = 0; i < size; i++)
			//	for (int j = 0; j < size; j++)
			for (int i = 0; i < size * size; i++)
				//rotateMiniCube(rotMat,j,i,size - 1 - wallIndex, rotVec, shapes[rubix[j][i][size - 1 - wallIndex]]);
				rotateMiniCube(rotMat, frontWallCubes[i], rotVec, shapes[frontWallCubes[i]]);

			if (!isOriented()) lock = 'z';
			else {
				move.push_back(Move(wallIndex, angle, 'F'));
				lock = 'u';
			}
		}
	}

}
void Game::solver(Game* scn)
{
	int tempangle = angle;
	int tempwallIndex = wallIndex;
	int index = move.size();
	while (index != 0) {
		Move curr = move[index - 1];
		move.pop_back();
		angle = -curr.angle;
		wallIndex = curr.wall;
		if (curr.direction == 'R') {
			scn->RotateWall('R');
			std::cout << "R" << std::endl;
		}
		if (curr.direction == 'L') {
			scn->RotateWall('L');
			std::cout << "L" << std::endl;
		}
		if (curr.direction == 'U') {
			scn->RotateWall('U');
			std::cout << "U" << std::endl;
		}
		if (curr.direction == 'D') {
			scn->RotateWall('D');
			std::cout << "D" << std::endl;
		}
		if (curr.direction == 'B') {
			scn->RotateWall('B');
			std::cout << "B" << std::endl;
		}
		if (curr.direction == 'F') {
			scn->RotateWall('F');
			std::cout << "F" << std::endl;
		}
		index--;
	}
	move.clear();
	angle = tempangle;
	wallIndex = tempwallIndex;
}
void Game::Shuffle(Game* scn) {
	int tempangle = angle;
	int tempwallIndex = wallIndex;
	for (int i = 0; i < 100; i++) {
		angle = 90.0f;
		int dir = rand() % 6;
		wallIndex = rand() % size;

		if (dir == 0) {
			scn->RotateWall('R');
		}
		if (dir == 1) {
			scn->RotateWall('L');
		}
		if (dir == 2) {
			scn->RotateWall('U');
		}
		if (dir == 3) {
			scn->RotateWall('D');
		}
		if (dir == 4) {
			scn->RotateWall('B');
		}
		if (dir == 5) {
			scn->RotateWall('F');
		}
	}
	angle = tempangle;
	wallIndex = tempwallIndex;

}
void Game::changeWall(bool in) {
	if (in && wallIndex != size - 1) wallIndex++;
	else if (!in && wallIndex != 0) wallIndex--;
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
	if (size % 2 == 1) distance = (size / 2);
	else distance = (size / 2) - 0.5;
	int index = 0, index_i = 0, index_j = 0, index_k = 0;
	for (float i = -distance; i <= distance; i += 1, index_i++) {
		for (float j = -distance; j <= distance; j += 1, index_j++) {
			for (float k = -distance; k <= distance; k += 1, index_k++) {
				if (index_i == size) {
					index_i = 0;
				}
				if (index_j == size) {
					index_j = 0;
				}
				if (index_k == size) {
					index_k = 0;
				}
				AddShape(Scene::Cube, -1, Scene::TRIANGLES);
				SetShapeTex(index, 0);
				shapes[index]->MyTranslate(glm::vec3(i, j, k), 0);
				shapes[index]->MyScale(glm::vec3(0.5, 0.5, 0.5));
				rubix[index_i][index_j][index_k] = index;
				std::cout << i << " " << j << " " << k << std::endl;
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



				miniCube curr{ index };
				curr.transform = Transform();
				curr.transform.translation = glm::vec3(i, j, k);

				cubeMatrix.push_back(curr);
				index++;
			}

		}
	}
	//rotateMiniCube(rotMatX(90), rubix[2][2][1], glm::vec3(1.0f, 0.0f, 0.0f), shapes[rubix[2][2][1]]);
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