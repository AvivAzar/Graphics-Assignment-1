https://github.com/AvivAzar/Graphics-Assignment-1/tree/main

Changes, Files and Functions modified-
In inputManager.h added appropriate callbacks

In scene.cpp added mouse processing for right click & left click movement

in moveableGLM.cpp & moveableGLM.h, added getRotMat and getTransMat to access their respective matrices

in game.cpp implemented most of the changes. Init will be called by main.cpp to create a 3x3 rubik's cube, an array linking the cube's index to a Shape pointer, and a vector for each wall listing the 9 indexes currently in that wall.

rotateWall, when called by inputManager.h, will first check which wall to rotate according to the input, then check if the cube is oriented properly or is one of the other walls already rotated, then if rotation is possible will call rotateMiniCube for each mini-cube in the wall to perform the rotation. It will then call isOriented to check if the cube ends in a position where rotation of a different wall is possible. If not, it will also lock the other walls.

rotateMiniCube will accept a rotation vector and matrix which depends on the wall and a cube ID and pointer, and will rotate that cube by using its inverse rotation matrix and update its new position and rotation in the 'cubeMatrix' int vector array.

isOriented will check in 'cubeMatrix' that all cubes are at an angle divisable by 90 to see if the walls should be locked or not. If they aren't locked, it will also call updateWalls to update the 6 wall vector arrays based on the position of each cube.

