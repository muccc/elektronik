class shape {
  PVector position;
  int size;

  shape(float _x, float _y, int _size) {
    position = new PVector(_x, _y);
    size = _size;
  }

  void show() {
    ellipse(position.x, position.y, size, size);
  }
}

shape s1;
ArrayList<shape> myShapes = new ArrayList<shape>();
int shapeCount =5000;
int maxSize=20;
PImage typo;


void setup() {
  size(500, 500);
  stroke(0);
  background(255);
  float x, y;
  int size;
  color pixel;
  typo = loadImage("background.png");
  for (int i = 0; i < shapeCount; i++) {
    do {
      x = random(500);
      y = random(500);
      size = (int) nearestDistance((int) x, (int)y)*2;
      pixel = typo.get((int)x,(int)y);
    } while (size < 0  || brightness(pixel) > 0);
    myShapes.add(new shape(x, y, size));
  }
}

void draw() {
  for (shape s : myShapes) {
    s.show();
  }
}

float nearestDistance(int x, int y) {
  float distance = maxSize;
  float col=0;
  for (shape s : myShapes) {
    col = dist(x, y, s.position.x, s.position.y)-s.size/2;
    if (col < distance) {
      distance = col;
    }
  }
  return distance;
}