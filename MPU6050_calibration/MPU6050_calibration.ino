#include <MPU6050.h>

MPU6050 mpu;

void setup() {
  Serial.begin(9600);

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }

  mpu.setAccelOffsetX(0);
  mpu.setAccelOffsetY(0);
  mpu.setAccelOffsetZ(0);

  Serial.println("Reading average acceleration : ");
  Vector ma = mean_accel();
  print_vector(ma);
  delay(1000);
  Serial.println("Calibrating");
  calibration(ma);
  delay(1000);
  Serial.println("All done ! Offsets : ");
  Serial.print("offset X = ");
  Serial.print(mpu.getAccelOffsetX());
  Serial.print(", offset Y = ");
  Serial.print(mpu.getAccelOffsetY());
  Serial.print(", offset Z = ");
  Serial.println(mpu.getAccelOffsetZ());
  Serial.println("Reading average acceleration : ");
  ma = mean_accel();
  Serial.println("Reading normated acceleration : ");
  printNorm();
  delay(1000);
}

void print_vector(Vector v) {
  Serial.print("\tX = ");
  Serial.print(v.XAxis);
  Serial.print("\tY = ");
  Serial.print(v.YAxis);
  Serial.print("\tZ = ");
  Serial.print(v.ZAxis);
  Serial.println("");
}

void printNorm() {
  Vector normAccel = mpu.readNormalizeAccel();
  Serial.print(" Xnorm = ");
  Serial.print(normAccel.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(normAccel.YAxis);
  Serial.print(" Znorm = ");
  Serial.println(normAccel.ZAxis);
}

void loop() {
  printNorm();
  delay(1000);
}

Vector mean_accel(){
  int iter_max = 50;
  Vector mean;
  mean.XAxis = 0;
  mean.YAxis = 0;
  mean.ZAxis = 0;

  for (int i = 0; i < 10; i++) {
    mpu.readRawAccel();
    delay(10);
  }

  for (int i = 0; i < iter_max; i++) {
    Vector accel = mpu.readRawAccel();

    mean.XAxis += accel.XAxis/iter_max;
    mean.YAxis += accel.YAxis/iter_max;
    mean.ZAxis += accel.ZAxis/iter_max;

    delay(10);
  }

  return mean;
}

Vector calibration(Vector ma){
  float noise = 8;
  Vector offset;
  offset.XAxis = -ma.XAxis/noise;
  offset.YAxis = -ma.YAxis/noise;
  offset.ZAxis = (16384-ma.ZAxis)/noise;

  while (1) {
    int ready=0;
    mpu.setAccelOffsetX(offset.XAxis);
    mpu.setAccelOffsetY(offset.YAxis);
    mpu.setAccelOffsetZ(offset.ZAxis);

    ma = mean_accel();
    print_vector(ma);

    if (abs(ma.XAxis) <= noise) {
      ready++;
    } else {
      offset.XAxis = offset.XAxis - ma.XAxis/noise;
    }

    if (abs(ma.YAxis) <= noise) {
      ready++;
    } else {
      offset.YAxis = offset.YAxis - ma.YAxis/noise;
    }

    if (abs(16384-ma.ZAxis)<=noise) {
      ready++;
    } else {
      offset.ZAxis = offset.ZAxis + (16384-ma.ZAxis)/noise;
    }

    if (ready==3) break;
  }
  return offset;
}
