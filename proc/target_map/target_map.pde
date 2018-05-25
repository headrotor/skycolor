import gifAnimation.*;
GifMaker gifExport;
boolean makeGif = false;
boolean gifdone = false;

int xw = 60;
int yw = 60;

float ph = 0.0;

int xdim;
int ydim;

float offset = -1;

Table rgbcols;
int nrgb =0;
int[] Ra;
int[] Ba;
int[] Ga;
color[] rgbc;

int loadrgb() {

  rgbcols = loadTable("rgb.csv", "header");

  println(rgbcols.getRowCount() + " total rows in table"); 
  nrgb = rgbcols.getRowCount();
  Ra = new int[nrgb];
  Ba = new int[nrgb];
  Ga = new int[nrgb];
  rgbc = new color[nrgb];
  int i =0;
  for (TableRow row : rgbcols.rows()) {

    Ra[i] = row.getInt("R");
    Ga[i] = row.getInt("G");
    Ba[i] = row.getInt("B");

    rgbc[i] = color(Ra[i],Ga[i],Ba[i]);
    //String species = row.getString("species");
    //String name = row.getString("name");
    ++i;
    //println(r + " found at " + row);
  }
  return i;

}

void setup() {
  xdim = 200;
  ydim = 200;
  size(200, 200);
  noStroke();
  smooth();
  background(#2810B2);
  nrgb = loadrgb();

  if (makeGif) {
    gifExport = new GifMaker(this, "target1.gif");
    gifExport.setRepeat(1);             // make it an "endless" animation
    // twitter seeems to ignore this
    gifExport.setDelay(33);
  }  //gifExport.setTransparent(0, 0, 0);    // black is transparent
}



void draw() {
  int index = 0;
  //number of transitions

  ph =  ph + PI/100;  
  int xd = xdim/2;
  int yd = ydim/2;
  println(fmouseX());
  noStroke();
  float scale = 8;
  for (int y  = 0; y < ydim; y++) {
    for (int x = 0; x < xdim; x++) {

      float r = sqrt( (x-xd)*(x-xd) + (y-yd)*(y-yd));
      //float th = atan((y-yd)/(x-xd));
      float th =0;
      //int red = int(80*sin(r*cos(ph*0.9)/scale)) + 127;
      //int grn = int(800*sin(r*cos(ph)/scale)) + 127;
      //int blu = int(800*sin(r*cos(ph*1.1)/scale)) + 127;
      fill(cmap(r,th));
      //fill(red);
      rect(x, y, 1, 1);
    }
  }

  if (ph > 0) {
    gifdone = true;
  }
  if (!makeGif) {
    return;
  }

  if (gifdone) {
    gifExport.finish();
    makeGif = false;
    return;
  }
  gifExport.addFrame();
  //println(offset);
}


color cmap(float r, float th) {
  // color map for this channel given r, theta 
  float cval=0; // channel value
  float index = map(r, 0, 500, 0, float(nrgb)); 
  int i = int(round(index));
  if (i >= nrgb) {
    i = nrgb -1;
  }
  return rgbc[i];
}

float fmouseX() {

  return 2.0*map(mouseX, 0, width, -1.0, 1.0);
}
float fmouseY() {

  return 1.0*map(mouseY, 0, height, -1.0, 1.0);
}
