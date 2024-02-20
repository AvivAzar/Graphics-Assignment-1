#include "InputManager.h"
// #include "../DisplayGLFW/display.h"
#include "game.h"
#include "../res/includes/glm/glm.hpp"
#include "stb_image.h"
#include <iostream>
#include <vector>
#include <fstream>
#define M_PI 3.14159265358979323846
using namespace std;
void GaussianBlur(const unsigned char* inputImage, unsigned char* outputImage, int width, int height) {
	// Create a Gaussian kernel 3x3
	vector<vector<float>> kernel = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	};
	// create a 5x5 Gaussian kernel
	// vector<vector<float>> kernel = {
	// 	{1, 4, 6, 4, 1},
	// 	{4, 16, 24, 16, 4},
	// 	{6, 24, 36, 24, 6},
	// 	{4, 16, 24, 16, 4},
	// 	{1, 4, 6, 4, 1}
	// };
	// Normalize the kernel
	float sum = 0;
	for (int i = 0; i < kernel.size(); i++)
		for (int j = 0; j < kernel[i].size(); j++)
			sum += kernel[i][j];
	for (int i = 0; i < kernel.size(); i++)
		for (int j = 0; j < kernel[i].size(); j++)
			kernel[i][j] /= sum;

	// Apply the kernel to the image
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			// Initialize sums for each channel
			float sumR = 0, sumG = 0, sumB = 0, sumA = 0;
			// Apply the kernel to the neighborhood of the pixel
			for (int ky = -1; ky <= 1; ky++) {
				for (int kx = -1; kx <= 1; kx++) {
					// Get the index of the current pixel in the input image
					int pixelIndex = ((y + ky) * width + (x + kx)) * 4;
					// Accumulate the weighted sum for each channel
					sumR += inputImage[pixelIndex] * kernel[ky + 1][kx + 1];
					sumG += inputImage[pixelIndex + 1] * kernel[ky + 1][kx + 1];
					sumB += inputImage[pixelIndex + 2] * kernel[ky + 1][kx + 1];
					sumA += inputImage[pixelIndex + 3] * kernel[ky + 1][kx + 1];
				}
			}
			// Set the output pixel values
			int outputIndex = (y * width + x) * 4;
			outputImage[outputIndex] = static_cast<unsigned char>(sumR);
			outputImage[outputIndex + 1] = static_cast<unsigned char>(sumG);
			outputImage[outputIndex + 2] = static_cast<unsigned char>(sumB);
			outputImage[outputIndex + 3] = static_cast<unsigned char>(sumA);
		}
	}
}
void SobelFilter(const unsigned char* inputImage, unsigned char* outputImage, int width, int height) {
	// Define Sobel operators for horizontal and vertical edges
	int sobelX[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};
	int sobelY[3][3] = {
		{-1, -2, -1},
		{0, 0, 0},
		{1, 2, 1}
	};

	// Apply Sobel operators to compute gradients
	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			int sumX = 0, sumY = 0;
			// Apply Sobel operators to the neighborhood of the pixel
			for (int ky = -1; ky <= 1; ++ky) {
				for (int kx = -1; kx <= 1; ++kx) {
					int pixelIndex = ((y + ky) * width + (x + kx)) * 4;
					sumX += inputImage[pixelIndex] * sobelX[ky + 1][kx + 1];
					sumY += inputImage[pixelIndex] * sobelY[ky + 1][kx + 1];
				}
			}
			// Compute the gradient magnitude
			int magnitude = std::sqrt(sumX * sumX + sumY * sumY);
			// Set the output pixel value
			int outputIndex = (y * width + x) * 4;
			outputImage[outputIndex] = static_cast<unsigned char>(magnitude);
			outputImage[outputIndex + 1] = static_cast<unsigned char>(magnitude);
			outputImage[outputIndex + 2] = static_cast<unsigned char>(magnitude);
			outputImage[outputIndex + 3] = 255;  // Set alpha channel to fully opaque
		}
	}
}

void NonMaximumSuppression(const unsigned char* inputImage, unsigned char* outputEdge, int width, int height) {
	// Iterate over every pixel in the image
	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			// Calculate the indices of neighboring pixels
			int index = (y * width + x) * 4;
			int left = ((y - 1) * width + (x - 1)) * 4;
			int center = (y * width + x) * 4;
			int right = ((y + 1) * width + (x + 1)) * 4;

			// Get the gradient directions
			float dx = inputImage[right] - inputImage[left];
			float dy = inputImage[center - width * 4] - inputImage[center + width * 4];

			// Compute the magnitude of the gradient
			float gradientMagnitude = sqrt(dx * dx + dy * dy);

			// Compute the direction of the gradient (in degrees)
			float gradientDirection = atan2(dy, dx) * 180.0 / M_PI;

			// Adjust negative angles
			if (gradientDirection < 0) {
				gradientDirection += 180;
			}

			// Determine the appropriate neighboring pixels for comparison based on gradient direction
			float leftPixel, rightPixel;
			if ((gradientDirection >= 0 && gradientDirection < 22.5) || (gradientDirection >= 157.5 && gradientDirection <= 180)) {
				leftPixel = inputImage[left];
				rightPixel = inputImage[right];
			}
			else if (gradientDirection >= 22.5 && gradientDirection < 67.5) {
				leftPixel = inputImage[left + width * 4];
				rightPixel = inputImage[right - width * 4];
			}
			else if (gradientDirection >= 67.5 && gradientDirection < 112.5) {
				leftPixel = inputImage[center + width * 4];
				rightPixel = inputImage[center - width * 4];
			}
			else {
				leftPixel = inputImage[right - width * 4];
				rightPixel = inputImage[left + width * 4];
			}

			// Perform non-maximum suppression
			if (inputImage[center] >= leftPixel && inputImage[center] >= rightPixel) {
				outputEdge[index] = inputImage[center];
				outputEdge[index + 1] = inputImage[center + 1];
				outputEdge[index + 2] = inputImage[center + 2];
				outputEdge[index + 3] = inputImage[center + 3];
			}
			else {
				outputEdge[index] = 0;
				outputEdge[index + 1] = 0;
				outputEdge[index + 2] = 0;
				outputEdge[index + 3] = 255; // Fully opaque
			}
		}
	}
}
void DoubleThreshold(const unsigned char* inputImage, unsigned char* outputEdge, int width, int height, int lowThreshold, int highThreshold) {
	// Iterate over every pixel in the image
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			// Calculate the index of the current pixel
			int index = (y * width + x) * 4;

			// Apply double thresholding
			if (inputImage[index] > highThreshold) {
				// Strong edge, preserve it
				outputEdge[index] = inputImage[index];
				outputEdge[index + 1] = inputImage[index + 1];
				outputEdge[index + 2] = inputImage[index + 2];
				outputEdge[index + 3] = inputImage[index + 3];
			}
			else if (inputImage[index] < lowThreshold) {
				// Weak edge or non-edge, suppress it
				outputEdge[index] = 0;
				outputEdge[index + 1] = 0;
				outputEdge[index + 2] = 0;
				outputEdge[index + 3] = 255; // Fully opaque
			}
			else {
				// Potential edge, perform further checks
				bool strongNeighbor = false;
				bool weakNeighbor = false;

				// Check 8-connected neighbors
				for (int ky = -1; ky <= 1; ++ky) {
					for (int kx = -1; kx <= 1; ++kx) {
						if (x + kx >= 0 && x + kx < width && y + ky >= 0 && y + ky < height) {
							int neighborIndex = ((y + ky) * width + (x + kx)) * 4;
							if (inputImage[neighborIndex] > highThreshold) {
								strongNeighbor = true;
							}
							else if (inputImage[neighborIndex] >= lowThreshold) {
								weakNeighbor = true;
							}
						}
					}
				}

				// Preserve potential edge if it has at least one strong neighbor
				if (strongNeighbor) {
					outputEdge[index] = inputImage[index];
					outputEdge[index + 1] = inputImage[index + 1];
					outputEdge[index + 2] = inputImage[index + 2];
					outputEdge[index + 3] = inputImage[index + 3];
				}
				else if (weakNeighbor) {
					// Suppress potential edge if it has only weak neighbors
					outputEdge[index] = 0;
					outputEdge[index + 1] = 0;
					outputEdge[index + 2] = 0;
					outputEdge[index + 3] = 255; // Fully opaque
				}
			}
		}
	}
}

void HysteresisThresholding(unsigned char* output, int width, int height) {
//	// Threshold values for weak and strong edges
	int highThreshold = 100;
	int lowThreshold = 100;
//
//	// Perform hysteresis thresholding
	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width - 1; ++x) {
			int pixelIndex = (y * width + x) * 4;
			if (output[pixelIndex] >= highThreshold) {
				output[pixelIndex] = 255;
				output[pixelIndex + 1] = 255;
				output[pixelIndex + 2] = 255;
			}
			else if (output[pixelIndex] >= lowThreshold && output[pixelIndex] < highThreshold) {
				output[pixelIndex] = 100; // Weak edge
				output[pixelIndex+1] = 100;
				output[pixelIndex+2] = 100;
			}
			else {
				output[pixelIndex] = 0; // Non-relevant edge
				output[pixelIndex + 1] = 0;
				output[pixelIndex + 2] = 0;
			}
		}
	}
}

void cannyEdgeDetector(const unsigned char* inputImage, unsigned char* outputEdge, int width, int height) {
	// Step 1: Gaussian blur
	unsigned char* blurredImage = new unsigned char[width * height * 4];
	unsigned char* outputSobel = new unsigned char[width * height * 4];
	unsigned char* outputNonMaxSuppression = new unsigned char[width * height * 4];
	GaussianBlur(inputImage, blurredImage, width, height);

	// Step 2: Sobel filter
	SobelFilter(blurredImage, outputSobel, width, height);
	// Step 3: Non-maximum suppression

	NonMaximumSuppression(outputSobel, outputNonMaxSuppression, width, height);
	// Step 4: Double threshold
	DoubleThreshold(outputNonMaxSuppression, outputEdge, width, height, 100, 150);
	// Step 5: Edge tracking by hysteresis
	HysteresisThresholding(outputEdge, width, height);
	// Clean up
	delete[] blurredImage;
	delete[] outputSobel;
	delete[] outputNonMaxSuppression;

	//write values to file
	ofstream MyFile("img4.txt");
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {	
			MyFile << static_cast<int>(outputEdge[(x+y*width)*4]);
			MyFile << ",";
		}
	}
	MyFile.close();
}

void applyBlackWhite(unsigned char* image, int pixel, bool white) {
	if (white == true) {
		image[pixel] = 255;
		image[pixel+1] = 255;
		image[pixel+2] = 255;
		image[pixel+3] = 255;
	}
	else {
		image[pixel] = 0;
		image[pixel + 1] = 0;
		image[pixel + 2] = 0;
		image[pixel + 3] = 255;
	}
}

void halftone(const unsigned char* inputImage, unsigned char* outputImage, int width, int height) {
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int inputPixelIndex = (y * width + x) * 4;
			int outputPixelIndex= (y * width * 2 + x) * 8;
			bool firstWhite = false, secondWhite = false, thirdWhite = false, fourthWhite = false;
			if (inputImage[inputPixelIndex] >= 51) firstWhite = true;
			if (inputImage[inputPixelIndex] >= 102) secondWhite = true;
			if (inputImage[inputPixelIndex] >= 153) thirdWhite = true;
			if (inputImage[inputPixelIndex] >= 204) fourthWhite = true;
			applyBlackWhite(outputImage, outputPixelIndex + width * 8, firstWhite); //one pixel down
			applyBlackWhite(outputImage, outputPixelIndex + 4, secondWhite); //one pixel right 
			applyBlackWhite(outputImage, outputPixelIndex + width * 8 + 4, thirdWhite); //one down, one right
			applyBlackWhite(outputImage, outputPixelIndex, fourthWhite); //starting pixel
			
		}
	}
	ofstream MyFile("img5.txt");
	for (int y = 0; y < height*2; y++) {
		for (int x = 0; x < width*2; x++) {
			MyFile << static_cast<int>(outputImage[(x + y * width) * 4]/255);
			MyFile << ",";
		}
	}
	MyFile.close();
}

void floyd(unsigned char* inputImage, unsigned char* outputImage, int width, int height) {
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int PixelIndex = (y * width + x) * 4;
			int errorVal = inputImage[PixelIndex] % 16;
			outputImage[PixelIndex] = inputImage[PixelIndex] - errorVal;
			outputImage[PixelIndex+1] = inputImage[PixelIndex] - errorVal;
			outputImage[PixelIndex+2] = inputImage[PixelIndex] - errorVal;
			outputImage[PixelIndex+3] = inputImage[PixelIndex] - errorVal;
			inputImage[PixelIndex + 4] += errorVal * (7.0 / 16.0);
			inputImage[PixelIndex + width*4 -4] += errorVal * (3.0 / 16.0);
			inputImage[PixelIndex + width * 4] += errorVal * (5.0 / 16.0);
			inputImage[PixelIndex + width * 4 + 4] += errorVal * (1.0 / 16.0);
		}
	}
	ofstream MyFile("img6.txt");
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			MyFile << static_cast<int>(outputImage[(x + y * width) * 4]/16);
			MyFile << ",";
		}
	}
	MyFile.close();
}


int main(int argc, char* argv[])
{
	const int DISPLAY_WIDTH = 512;
	const int DISPLAY_HEIGHT = 512;
	const float CAMERA_ANGLE = 0.0f;
	const float NEAR = 1.0f;
	const float FAR = 100.0f;

	Game* scn = new Game(CAMERA_ANGLE, (float)DISPLAY_WIDTH / DISPLAY_HEIGHT, NEAR, FAR);

	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "OpenGL");
	Init(display);
	scn->Init();
	display.SetScene(scn);
	int width, height, numChannels;
	unsigned char* imageData = stbi_load("../res/textures/lena256.jpg", &width, &height, &numChannels, 4);
	if (imageData == NULL)
		std::cerr << "Unable to load texture: " << "../res/textures/lena256.jpg" << std::endl;
	//First image (unchanged)
	scn->AddTexture("../res/textures/lena256.jpg", false);
	scn->SetShapeTex(0, 0);
	scn->CustomDraw(1, 0, scn->BACK, true, false, 0);

	//Second image (edges)
	unsigned char* edgeDetectedImage = new unsigned char[width * height * 4]; // Assuming 256x256 size
	cannyEdgeDetector(imageData, edgeDetectedImage, 256, 256);
	scn->AddTexture(256, 256, edgeDetectedImage);
	scn->SetShapeTex(0, 1);
	scn->CustomDraw(1, 0, scn->BACK, false, false, 1);

	//Third image (halftone)
	unsigned char* halftoneImage = new unsigned char[width * height * 16]; //*4 for RGBA and *4 for upscaling to 512x512 resolution
	halftone (imageData, halftoneImage, 256, 256);
	scn->AddTexture(512, 512, halftoneImage);
	scn->SetShapeTex(0, 2);
	scn->CustomDraw(1, 0, scn->BACK, false, false, 2);

	//Fourth image (floyd Steinberg 16 tones)
	unsigned char* floydImage = new unsigned char[width * height * 4];
	floyd(imageData, floydImage, 256, 256);
	scn->AddTexture(256, 256, floydImage);
	scn->SetShapeTex(0, 3);
	scn->CustomDraw(1, 0, scn->BACK, false, false, 3);

	scn->Motion();
	display.SwapBuffers();

	while (!display.CloseWindow())
	{

		display.PollEvents();

	}
	delete scn;
	return 0;
}