Changes, Files and Functions modified-
scene.cpp- added to CustomDraw 4 viewports of size 256x256 each in a square, making up 512x512 total
Main- 
For second image- added function cannyEdgeDetector which uses functions GaussianBlur to blur the image, sobelFilter to highlight edges and only edges, and three function NonMaximumSuppression, DoubleThreshold and HysteresisThresholding to attempt to keep only long, consistent edges and further filter out 'noise'.
