class CVector
{
  float r = 0.0;
  float i = 0.0;

  CVector(float real, float imaginary) {
    set(real, imaginary);
  }  

  void set(float real, float imaginary) {
    r = real;
    i = imaginary;
  }


  float mod() {
    // return modulus (magnitude) of floating point number
    return((float)Math.sqrt(r * r + i * i));
  }
  float arg() {
    // return polar angle
    return ((float)Math.atan2((double)i, (double)r));
  }
  CVector mul(CVector c) {
    // multiply this complex number by the complex number c
    return new CVector((c.r*r) - (c.i*i), (c.i*r) + (c.r*i));
  }
  CVector add(CVector c) {
    // add number c to this complex number
    return new CVector(r + c.r, i + c.i);
  }
  CVector sub(CVector c) {
    // subtract number c from this complex number
    return new CVector(r - c.r, i - c.i);
  }
  CVector neg() {
    // return -z
    return new CVector(-r, -i);
  }
  CVector exp() {
    // return e^z
    return new CVector((float)(Math.exp(r)*Math.cos(i)), (float)(Math.exp(r)*Math.sin(i)));
  }
  CVector pow(CVector w) {
    // return z^w
    return new CVector((float)(Math.exp(r)*Math.cos(i)), (float)(Math.exp(r)*Math.sin(i)));
  }
  CVector log() {
    // return complex logarithm of z
    return new CVector((float)Math.log(mod()), arg());
  }
  CVector sqrt() {
    CVector temp = new CVector(0, 0);
    temp.frompolar((float)Math.sqrt(mod()), arg()/2);     
    return temp;
  }
  void frompolar(float theMod, float theArg) {
    // convert polar representation (magnitude, angle) to re,im
    r = theMod * (float)Math.cos(theArg);
    i = theMod * (float)Math.sin(theArg);
  }
  CVector inv() {
    // compute the inverse
    float tmag =  1/mod();
    float targ =  -arg();
    CVector temp = new CVector(0, 0);
    temp.frompolar(1/mod(), -arg());     
    return temp;
  }
  CVector div(CVector d) {
    // divide by vector d
    return mul(d.inv());
  }
  float re() {
    return r;
  }
  float im() {
    return i;
  }
  void pr(int n, int f) {
    // print complex number with n digits integral and f digits fraction
    println("r=", nf(r, n, f), "i=", nf(i, n, f));
  }
}

