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
    void RotateWall(char wallId, bool clockwise);
    void FlipRotationDirection();
    void DivideRotationAngle();
    void WhenRotate();
    void WhenTranslate();
    void Motion();
    ~Game(void);
};