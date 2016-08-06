#include <Arduino.h>
#include <MPU6050.h>
#include <LowPower.h>

#define MPU_INT 3

MPU6050 mpu;
volatile int motion_detected = 0;

void mpu_event() {
  motion_detected = 1;
}

void setup() {
  Serial.begin(9600);

  // The range/sensitivity of the mpu can be set here
  // DPS = Degree per second
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
  calibration();
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
  mpu.setIntMotionEnabled(true);
  Serial.println("Interrupt status : ");
  if (mpu.getIntMotionEnabled()) {
    Serial.println("Interrupt when motion starts : True");
  } else {
    Serial.println("Interrupt when motion stops : False");
  }
  mpu.setMotionDetectionThreshold(1); // What's the unit ??
  mpu.setMotionDetectionDuration(10); // In increment of 1ms
  Serial.print("Motion detection threshold : ");
  Serial.println(mpu.getMotionDetectionThreshold());
  Serial.print("Motion detection duration : ");
  Serial.println(mpu.getMotionDetectionDuration());
  delay(1000);
  attachInterrupt(digitalPinToInterrupt(MPU_INT), mpu_event, RISING);
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
  Serial.println("Woke up !");
  if (motion_detected) {
    printNorm();
    delay(1000);
    motion_detected = 0;
  }
  Serial.end();
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  Serial.begin(9600);
  delay(2000);
  Serial.println("");
}

Vector mean_accel(){
  int iter_max = 20;
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

void calibration(){
  int16_t noise = 10; // How noise can we tolerate, to low may not converge
  int range = 0;
  switch (mpu.getRange()) {
    case MPU6050_RANGE_16G:
      range = 16;
      break;
    case MPU6050_RANGE_8G:
      range = 8;
      break;
    case MPU6050_RANGE_4G:
      range = 4;
      break;
    case MPU6050_RANGE_2G:
      range = 2;
      break;
  }
  int16_t cf = 16/range;
  int16_t z_rest = 32767/range;
  int32_t ox = 0, oy = 0, oz = 0;

  while (1) {
    int ready=0;
    mpu.setAccelOffsetX(ox);
    mpu.setAccelOffsetY(oy);
    mpu.setAccelOffsetZ(oz);

    Vector ma = mean_accel();
    print_vector(ma);

    if (abs(ma.XAxis) <= noise) {
      ready++;
    } else {
      ox -= ma.XAxis/cf;
    }

    if (abs(ma.YAxis) <= noise) {
      ready++;
    } else {
      oy -= ma.YAxis/cf;
    }

    if (abs(z_rest-ma.ZAxis)<=noise) {
      ready++;
    } else {
      oz += (z_rest-ma.ZAxis)/cf;
    }

    if (ready==3) break;
  }
}
