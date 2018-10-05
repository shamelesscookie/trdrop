import processing.video.*;

Movie mov;
PImage image_01 = createImage(800,800, RGB);
PImage image_02 = createImage(800,800, RGB);
PImage masked_image = createImage(800,800, RGB);
int newFrame = 0;
int counter = 0;

int[] frame_counter = new int[60];

void setup() {
  
  size(800,800);
  background(0);
  mov = new Movie(this, "/home/rewrite/Documents/processing-sketchbook/progs/Frames/skyrim.avi");
  // Pausing the video at the first frame. 
  mov.play();
  mov.jump(0);
  mov.pause();
  image_01.copy(mov,0,0,mov.width, mov.height, 0,0, 800, 800);
  //frameRate(10);
}


void draw() {
  background(0);
  image_02.copy(image_01,0,0,image_01.width, image_01.height, 0,0, image_01.width, image_01.height);
  image_01.copy(mov,0,0,mov.width, mov.height, 0,0, 800, 800);
  masked_image.copy(image_02,0,0,image_02.width, image_02.height, 0,0, image_02.width, image_02.height);
  //masked_image.mask(image_01);
  int duplicate = 0;
  for( int i = 0; i < masked_image.width * masked_image.height; ++i){
    if ( masked_image.pixels[i] == image_01.pixels[i] ) {
        masked_image.pixels[i] = color(0,0,0); 
    } else {
        duplicate = 1;
        masked_image.pixels[i] = color(255,255,255);
    }
  }
  frame_counter[getFrame() % 60] = duplicate;
  //image(masked_image,0,0);
  fill(255);
  text(getFrame() + " / " + (getLength() - 1), 10, 30);
  text("FPS: " + getFrameRate(), 10, 50);


}

int getFrameRate(){
    int sum = 0;
    for(int i = 0; i < 60; ++i){
        sum += frame_counter[i];
    }
    return sum;
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == LEFT) {
      if (0 < newFrame) newFrame--; 
    } else if (keyCode == RIGHT) {
      if (newFrame < getLength() - 1) newFrame++;
    }
  } 
  setFrame(newFrame);  
}
  
int getFrame() {    
  int c = ceil(mov.time() * 60) - 1;
  return c;
}

void setFrame(int n) {
  mov.play();
    
  // The duration of a single frame:
  float frameDuration = 1.0 / mov.frameRate;
    
  // We move to the middle of the frame by adding 0.5:
  float where = (n + 0.5) * frameDuration; 
    
  // Taking into account border effects:
  float diff = mov.duration() - where;
  if (diff < 0) {
    where += diff - 0.25 * frameDuration;
  }
    
  mov.jump(where);
  mov.pause();  
}  

int getLength() {
  return int(mov.duration() * mov.frameRate);
}
/*void setup() {
  size(640, 360);
  background(0);
//  points = new GPointsArray();
//  plot = new GPlot(this);
  mov = new Movie(this, "/home/rewrite/Documents/processing-sketchbook/progs/Frames/skyrim.avi");
  // printArray();
  mov.play();
}

void movieEvent(Movie m) {
  m.read();
}

void draw() {
  background(0);
  image(mov, 0, 0, width, height);
  fill(255);
  //plot();
}

int getFrame() {    
  return ceil(mov.time() * 30) - 1;
}

int getLength() {
  return int(mov.duration() * mov.frameRate);
}
/*
void plot() {
  // Prepare the points for the plot
  if(frameCount <= 200 && frameCount%2 == 0){
    points.add(new GPoint(frameCount, 100*noise(frameCount*0.1)));
  }
  
  // set its position on the screen
  plot.setPos(25, 25);
  
  plot.setPoints(points);
  // Set the plot title and the axis labels
  plot.setTitleText("A very simple example");
  plot.getXAxis().setAxisLabelText("x axis");
  plot.getYAxis().setAxisLabelText("y axis");
  plot.getXAxis().setLineColor(255);
  plot.getYAxis().setLineColor(255);
  plot.getXAxis().setFontColor(255);
  plot.getYAxis().setFontColor(255);
  plot.getXAxis().getAxisLabel().setFontColor(255);
  plot.getYAxis().getAxisLabel().setFontColor(255);
  plot.setFontColor(255);
  plot.setLineColor(255);
  plot.setPointColor(255);
  plot.setXLim(0, 100);
  plot.setYLim(0, 100);
  // Draw it!
  plot.beginDraw();
  plot.drawLines();
  plot.drawXAxis();
  plot.drawYAxis();
  plot.drawLabels();
  plot.endDraw();
}  */
