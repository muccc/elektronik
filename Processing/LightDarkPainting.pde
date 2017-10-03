/**
 * Frame Differencing 
 * by Golan Levin. 
 *
 * Quantify the amount of movement in the video frame using frame-differencing.
 */ 

import processing.video.*;

int numPixels;
int[] lightFrame;
int colorDirection;
Capture video;

void setup() {
  size(640, 480);
  
  // defines if we keep light (1) or dark parts (-1)
  colorDirection = -1;
  
  // This the default video input, see the GettingStartedCapture 
  // example if it creates an error
  video = new Capture(this, width, height);
  
  // Start capturing the images from the camera
  video.start(); 
  
  numPixels = video.width * video.height;
  // Create an array to store the previously captured frame
  lightFrame = new int[numPixels];
  loadPixels();
  
}

void draw() {
  if (video.available()) {
    // When using video to manipulate the screen, use video.available() and
    // video.read() inside the draw() method so that it's safe to draw to the screen
    video.read(); // Read the new frame from the camera
    video.loadPixels(); // Make its pixels[] array available
    for (int i = 0; i < numPixels; i++) { // For each pixel in the video frame...
      color currColor = video.pixels[i];
      color lfColor = lightFrame[i];
      // Extract the red, green, and blue components from current pixel
      int currR = (currColor >> 16) & 0xFF; // Like red(), but faster
      int currG = (currColor >> 8) & 0xFF;
      int currB = currColor & 0xFF;
      // Extract red, green, and blue components from previous pixel
      int lfR = (lfColor >> 16) & 0xFF;
      int lfG = (lfColor >> 8) & 0xFF;
      int lfB = lfColor & 0xFF;
      // Set a new pixel when lighter/darker based on setting of colorDirection
      if (((currR+currG+currB)-(lfR+lfG+lfB))*colorDirection>0 | ((lfR+lfG+lfB)<1)) {
         lightFrame[i] = video.pixels[i];
      } 
      pixels[i] = lightFrame[i];
      
    }
    updatePixels();
 
  }
}