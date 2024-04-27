#pragma once
#include "scene.h"

class Game : public Scene
{
public:

    Game();
    Game(float angle, float relationWH, float near, float far);
    void MultiplyRotationAngle();
    void Init();
    void Update(const glm::mat4& MVP, const glm::mat4& Model, const int  shaderIndx);
    void RotateWall(char wallId);
    void Game::Shuffle(Game* scn);
    void Game::solver(Game* scn);
    void Game::changeWall(bool in);
    void FlipRotationDirection();
    void DivideRotationAngle();
    void WhenRotate();
    void WhenTranslate();
    void Motion();
    ~Game(void);
};