Changes, Files and Functions modified-
scene.cpp- added to CustomDraw 4 viewports of size 256x256 each in a square, making up 512x512 total
main.cpp- 
main will create a 512x512 display, set up the camera, load the texture for the first image, and then create 3 new blank images and call 3 different functions to transform each of them into the final desired product.
For second image- added function cannyEdgeDetector which uses functions GaussianBlur to blur the image, sobelFilter to highlight edges and only edges, and three function NonMaximumSuppression, DoubleThreshold and HysteresisThresholding to attempt to keep only long, consistent edges and further filter out 'noise'.
For third image- added function halftone which gets a new blank 512x512 image, checks tone of each original image pixel in sequence and then calls function applyBlackWhite to paint 4 new black/white pixels in the corrosponding location of the 512x512 image.
For the fourth image- added function floyd which converts a 256 tone greyscale image to a 16 tone greyscale image and for each pixel distributes the 'carryover' to other neighboring pixels to stay as close as possible to the original image.
