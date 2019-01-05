#include "P3RGB64x32MatrixPanel.h"
byte world[WIDTH][HEIGHT][2];



void UpdateGameOfLife()
{
  DisplayCurrentGeneration();
  delay(DELAY);

  // Birth and death cycle
  for (int x = 0; x < WIDTH; x++)
  {
    for (int y = 0; y < HEIGHT; y++)
    {
      // Default is for cell to stay the same
      world[x][y][1] = world[x][y][0];
      int count = neighbours(x, y);
      if (count == 3 && world[x][y][0] == 0)
      {
        // A new cell is born
        world[x][y][1] = 1;
        matrix.drawPixel(x, y, matrix.color444(200, 0, 0));
      }
      if ((count < 2 || count > 3) && world[x][y][0] == 1)
      {
        // Cell dies
        matrix.drawPixel(x, y, matrix.color444(0, 5, 0));
        world[x][y][1] = 0;
      }
    }
  }

  // Copy next generation into place
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      world[x][y][0] = world[x][y][1];
    }
  }
}

int neighbours(int x, int y)
{
  return world[(x + 1) % WIDTH][y][0] +
         world[x][(y + 1) % HEIGHT][0] +
         world[(x + WIDTH - 1) % WIDTH][y][0] +
         world[x][(y + HEIGHT - 1) % HEIGHT][0] +
         world[(x + 1) % WIDTH][(y + 1) % HEIGHT][0] +
         world[(x + WIDTH - 1) % WIDTH][(y + 1) % HEIGHT][0] +
         world[(x + WIDTH - 1) % WIDTH][(y + HEIGHT - 1) % HEIGHT][0] +
         world[(x + 1) % WIDTH][(y + HEIGHT - 1) % HEIGHT][0];
}

void DisplayCurrentGeneration()
{
  boolean alive = 0;
  //clearLeds();
  // Display current generation
  for (int i = 0; i < WIDTH; i++)
    for (int j = 0; j < HEIGHT; j++)
      if (world[i][j][0] == 1)
      {
        //SetLED(i,j,LP.color(0,16,0));
        matrix.drawPixel(i, j, matrix.color444(255, 0, 0));
        alive = 1;
      }
  if (DEBUG) {
    if (alive)
      Serial.print("Alive :");
    else
      Serial.print("Death :");
    Serial.println(count);
  }
  count++;
}

void Shuffle()
{
  Serial.println("Shuffle");
  for (int i = 0; i < WIDTH; i++)
  {
    randomSeed(analogRead(5));
    for (int j = 0; j < HEIGHT; j++)
    {
      if (int(random(0, 4)) == 1)
        world[i][j][0] = 1;
    }
  }
}



