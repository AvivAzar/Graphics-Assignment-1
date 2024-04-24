#pragma once   //maybe should be static class
#include "display.h"
#include "game.h"


void mouse_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        Game* scn = (Game*)glfwGetWindowUserPointer(window);
        double x2, y2;
        glfwGetCursorPos(window, &x2, &y2);
        scn->Picking((int)x2, (int)y2);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* scn = (Game*)glfwGetWindowUserPointer(window);
    if(yoffset<0) scn->MyScale(glm::vec3(0.96f,0.96f,0.96f));
    else scn->MyScale(glm::vec3(1.04f, 1.04f, 1.04f));
    //glm::vec3 rot = glm::mat3(glm::transpose(scn->getRotMat())) * glm::vec3(0, 0, yoffset);
    //scn->MyTranslate(rot, 0);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Game* scn = (Game*)glfwGetWindowUserPointer(window);
    glm::vec3 rot;
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        case GLFW_KEY_SPACE:
            //if (scn->IsActive())
              //  scn->Deactivate();
            //else
              //  scn->Activate();
            scn->FlipRotationDirection();
            break;
        case GLFW_KEY_Z:
            scn->DivideRotationAngle();
            break;
        case GLFW_KEY_A:
            scn->MultiplyRotationAngle();
            break;
        case GLFW_KEY_UP:
            rot = glm::mat3(glm::transpose(scn->getRotMat())) * glm::vec3(-1.0f, 0.0f, 0.0f);
            scn->MyRotate(4.0f, rot, 0);
            break;
        case GLFW_KEY_DOWN:
            rot = glm::mat3(glm::transpose(scn->getRotMat())) * glm::vec3(1.0f, 0.0f, 0.0f);
            scn->MyRotate(4.0f, rot, 0);
            break;
        case GLFW_KEY_LEFT:
            rot = glm::mat3(glm::transpose(scn->getRotMat())) * glm::vec3(0.0f, 1.0f, 0.0f);
            scn->MyRotate(4.0f, rot, 0);
            break;
        case GLFW_KEY_RIGHT:
            rot = glm::mat3(glm::transpose(scn->getRotMat())) * glm::vec3(0.0f, -1.0f, 0.0f);
            scn->MyRotate(4.0f, rot, 0);
            break;
        case GLFW_KEY_R:
            scn->RotateWall('R', true); // Rotate right wall clockwise
            break;
        case GLFW_KEY_L:
            scn->RotateWall('L', true); // Rotate left wall clockwise
            break;
        case GLFW_KEY_U:
            scn->RotateWall('U', true); // Rotate up wall clockwise
            break;
        case GLFW_KEY_D:
            scn->RotateWall('D', true); // Rotate down wall clockwise
            break;
        case GLFW_KEY_B:
            scn->RotateWall('B', true); // Rotate back wall clockwise
            break;
        case GLFW_KEY_F:
            scn->RotateWall('F', true); // Rotate front wall clockwise
            break;
        default:
            break;
        }
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    Game* scn = (Game*)glfwGetWindowUserPointer(window);

    scn->UpdatePosition((float)xpos, (float)ypos);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        scn->MouseProccessing(GLFW_MOUSE_BUTTON_RIGHT);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        scn->MouseProccessing(GLFW_MOUSE_BUTTON_LEFT);
    }

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    Game* scn = (Game*)glfwGetWindowUserPointer(window);

    scn->Resize(width, height);

}

void Init(Display& display)
{
    display.AddKeyCallBack(key_callback);
    display.AddMouseCallBacks(mouse_callback, scroll_callback, cursor_position_callback);
    display.AddResizeCallBack(window_size_callback);
}