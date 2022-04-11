#include<Wire.h>
#include <Servo.h>

const int MPU = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

//Placeholders. These will be addressed here for quick changes without having to dig through the code to find every instance
int servoOnePin; //front left
int servoTwoPin; //front rear left
int servoThreePin; //front right
int servoFourPin; //front rear right
int servoFivePin; //rear ring

int firstToggle;
int secondToggle;
int momentaryToggle;

int brakeSensor;

//Todo: Figure out what IMU readings correspond to certain g-forces
//These variables will be replaced with the IMU readings from down in the main loop
float aI, aC, aF;

//These must be tuned
//All listed angles are in degrees, but our code probably won't handle that well
int servoOneMinAngle = 0;
int servoOneMidLow = 5;
int servoOneMidHigh = 10;
int servoOneMaxAngle = 30;

int servoTwoMinAngle = 0;
int servoTwoMidLow = 29;
int servoTwoMidHigh = 58;
int servoTwoMaxAngle = 58;

int servoThreeMinAngle = 0;
int servoThreeMidLow = 5;
int servoThreeMidHigh = 10;
int servoThreeMaxAngle = 30;

int servoFourMinAngle = 0;
int servoFourMidLow = 29;
int servoFourMidHigh = 58;
int servoFourMaxAngle = 58;

int servoFiveMinAngle = 10;
int servoFiveMidAngle = 60;
int servoFiveMaxAngle = 80;


/*
  per CDR (for second wing):
  for aC => 0.3, wing angle = 5
  for aC => 0.8, wing angle = 10
*/


Servo servoOne, servoTwo, servoThree, servoFour, servoFive; //initializes servos

/*
//Position variables:
int pos = 10;       //default position
int posPrev = 0;    //stores previous position in test operation
int posIncrement = 10;  //how much position is to be incremented each loop in test operation

int scaleInput(int sensorData) {  //clamps sensor data between +/- 12,000
  if (sensorData > 12, 000) {
    sensorData = 12, 000;
  }
  if (sensorData < -12, 000) {
    sensorData = -12, 000;
  }
  int output = map(sensorData, -12000, 12000, 10 , 170);  //maps the +/-12,000 sensor input to a range between 10 and 170 degrees
  return output;
}
*/

int modeSelect(firstToggle, secondToggle) {
  if (firstToggle == 0 && secondToggle == 0) {
    return 0;
  } else if (firstToggle == 1 && secondToggle == 0) {
    return 1;
  } else if (firstToggle == 0 && secondToggle == 1) {
    return 2;
  } else if (firstToggle == 1 && secondToggle == 1) {
    return 3;
  }
}

void lowDragCheck() {
  if (momentaryToggle == 1) {
    wingControl(0, 0, 0, 0, 0);
  }
}

void brakeCheck() {
  if ((brakeSensor == 1) && (ai <= -0.5)) {
    wingControl(3, 3, 3, 3, 2);
  }
}

void discreteWing(accel) {
  if (accel >= AcHigh) {
    wingControl(2, 2, 0, 0, 1);
  } else if (accel >= AcLow) {
    wingControl(1, 1, 0, 0, 1);
  } else if (accel <= -AcLow) {
    wingControl(0, 0, 1, 1, 1);
  } else if (accel <= -AcHigh) {
    wingControl(0, 0, 2, 2, 1);
  }
  else {
    wingControl(0, 0, 0, 0, 1);
  }
}

/* Needs extensive revision before implementation
  void continuousWing(){
    //position is only updated if sensor reading changes by +/- 20%
  if((pos <= scaleInput(AcX)*1.20) || (pos >= scaleInput(AcX)*0.80)){
    pos = scaleInput(AcX);
    return pos;
  }
  }
*/

void testWing() {
  //This code swings the servo arms left and right between 0 and 180 degrees
  wingControl(0, 0, 0, 0, 0);
  Serial.println("Low Drag");
  delay(1000);

  wingControl(3, 3, 3, 3, 2);
  Serial.println("High Downforce");
  delay(1000);

  wingControl(1, 1, 0, 0, 1);
  Serial.println("Slow Left");
  delay(1000);

  wingControl(2, 2, 0, 0, 1);
  Serial.println("Fast Left");
  delay(1000);

  wingControl(0, 0, 1, 1, 1);
  Serial.println("Slow Right");
  delay(1000);

  wingControl(0, 0, 2, 2, 1);
  Serial.println("Fast Right");
  delay(1000);
}

void wingControl(servoOneAngle, servoTwoAngle, servoThreeAngle, servoFourAngle, servoFiveAngle) {

  switch (servoOneAngle) {
    case 0:
      servoOne.write(servoOneMinAngle);
      break;
    case 1:
      servoOne.write(servoOneMidLow);
      break;
    case 2:
      servoOne.write(servoOneMidHigh);
      break;
    case 3:
      servoOne.write(servoOneMaxAngle);
      break;
  }

  switch (servoTwoAngle) {
    case 0:
      servoOne.write(servoTwoMinAngle);
      break;
    case 1:
      servoOne.write(servoTwoMidLow);
      break;
    case 2:
      servoOne.write(servoTwoMidHigh);
      break;
    case 3:
      servoOne.write(servoTwoMaxAngle);
      break;
  }

  switch (servoThreeAngle) {
    case 0:
      servoThree.write(servoThreeMinAngle);
      break;
    case 1:
      servoThree.write(servoThreeMidLow);
      break;
    case 2:
      servoThree.write(servoThreeMidHigh);
      break;
    case 3:
      servoThree.write(servoThreeMaxAngle);
      break;
  }

  switch (servoFourAngle) {
    case 0:
      servoThree.write(servoFourMinAngle);
      break;
    case 1:
      servoThree.write(servoFourMidLow);
      break;
    case 2:
      servoThree.write(servoFourMidHigh);
      break;
    case 3:
      servoThree.write(servoFourMaxAngle);
      break;
  }

  switch (servoFiveAngle) {
    case 0:
      servoFive.write(servoFiveMinAngle);
      break;
    case 1:
      servoFive.write(servoFiveMidAngle);
      break;
    case 2:
      servoFive.write(servoFiveMaxAngle);
      break;
  }
}


void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);

  servoOne.attach(servoOnePin);
  servoTwo.attach(servoTwoPin);
  servoThree.attach(servoThreePin);
  servoFour.attach(servoFourPin);
  servoFive.attach(servoFivePin)

  pinMode(firstToggle, INPUT);
  pinMode(secondToggle, INPUT);
  pinMode(momentaryToggle, INPUT);

  //digitalRead(firstToggle);
  //digitalRead(secondToggle);
  analogRead(firstToggle);
  analogRead(secondToggle);

  //because mode selection is in setup, it can only be done while the system is powered off
  int mode = modeSelect();

  //initialization test
  testWing();
}

void loop() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 12, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = "); Serial.println(AcZ);

  Serial.print("Gyroscope: ");
  Serial.print("X = "); Serial.print(GyX);
  Serial.print(" | Y = "); Serial.print(GyY);
  Serial.print(" | Z = "); Serial.println(GyZ);
  Serial.println(" ");

  Serial.print("Position variable: ");
  Serial.println(pos);


  /* First, we monitor the brake sensor. If that requires action it takes priority.
      Second, we check for the momentary switch which indicates the drivers desires low drag mode
      Finally, if we see neither the brake nor low drag triggers, we move into the selected mode
  */

  brakeCheck();

  digitalRead(momentaryToggle);
  lowDragCheck();

  switch (mode) {
    case 0:
      wingControl(0, 0, 0, 0, 0); //low drag mode
      break;
    case 1:
      discreteWing(AcY);
      break;
    case 2:
      continuousWing();
      break;
    case 3:
      wingControl(3, 3, 3, 3, 2); // high downforce mode
  }
}