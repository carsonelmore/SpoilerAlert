#include<Wire.h>
#include <Servo.h>

//Variables that affect IMU output
const int rollingAverageLengthX = 10;
int summandX[rollingAverageLengthX];
int summandCounterX = 0;
float averageAcX = 0.0;
const int rollingAverageLengthY = 10;
int summandY[rollingAverageLengthY];
int summandCounterY = 0;
float averageAcY = 0.0;

int printDelay = 50; //overall delay between loops of system in milliseconds
int brakeDelay = 500; //delay for brake triggering full drag mode in milliseconds
int airBrakeAcY = 2000; //threshold for deceleration to trigger air brake
int AcLow = 500; //low g-force threshold, needs to be tested 500 worked
int AcHigh = 1250; //high g-force threshold, needs to be tested 1000 worked
// datasheet says 16384 bits per g

const int MPU = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

//Placeholders. These will be addressed here for quick changes without having to dig through the code to find every instance
int servoOnePin = 11; //front left
int servoTwoPin = 17; //front rear left
int servoThreePin = 18; //front right
int servoFourPin = 16; //front rear right
int servoFivePin = 9; //rear wing
int mode;
int i;
int errorAcX = 0;
int offsetAcX = 0;
int errorAcY = 0;
int offsetAcY = 0;

//initialize switches and steering wheel button
int switchA;
int switchAPin = 10;
int switchB;
int switchBPin = 15;
int lowDragButtonPin = 7;
int brakeSensorPin = 2;

//initializes continuous mode variables
int servoOneCont, servoTwoCont, servoThreeCont, servoFourCont, servoFiveCont;

//These must be tuned
//All listed angles are in degrees, but our code probably won't handle that well
int servoOneMinAngle = 60;
int servoOneMidLow = 70;
int servoOneMidHigh = 85;
int servoOneMaxAngle = 112;

int servoTwoMinAngle = 130;
int servoTwoMidLow = 107;
int servoTwoMidHigh = 68;
int servoTwoMaxAngle = 68;

int servoThreeMinAngle = 46;
int servoThreeMidLow = 89;
int servoThreeMidHigh = 106;
int servoThreeMaxAngle = 106;

int servoFourMinAngle = 124;
int servoFourMidLow = 114;
int servoFourMidHigh = 99;
int servoFourMaxAngle = 64;

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
  if (sensorData < -12,000) {
    sensorData = -12,000;
  }
  int output = map(sensorData, -12000, 12000, 10 , 170);  //maps the +/-12,000 sensor input to a range between 10 and 170 degrees
  return output;
  }

  int scaleOutput(){
    //use map(sensor minimum reading, sensor maximum reading, servo minimum angle, servo maximum angle)
    //to convert AcX to servo angles?
  }
*/

int modeSelect() {
  //may be analog, need to test
  switchA = digitalRead(switchAPin);
  switchB = digitalRead(switchBPin);
  if (switchA == 0 && switchB == 0) {
    return 0;
  } else if (switchA == 1 && switchB == 0) {
    return 1;
  } else if (switchA == 0 && switchB == 1) {
    return 2;
  } else if (switchA == 1 && switchB == 1) {
    return 3;
  }
  else {
    return 0;
  }
}

void lowDragCheck() { //lowDragButtonPin was reading inconsistently, do not uncomment until it can be grounded
  //if (digitalRead(lowDragButtonPin) == 1) {
   // wingControl(0, 0, 0, 0, 0);
  //}
}

void brakeCheck() { // did not work, do not know how to make it work
  if ((digitalRead(brakeSensorPin) == 1) && (averageAcY >= airBrakeAcY)) {
    wingControl(3, 3, 3, 3, 2);
    delay(brakeDelay-printDelay);
  }
}

void discreteWing() {
  if (averageAcX >= AcHigh) {
    wingControl(0, 0, 2, 2, 1);
  } else if (averageAcX >= AcLow) {
    wingControl(0, 0, 1, 1, 1);
  } else if (averageAcX <= -AcHigh) {
    wingControl(2, 2, 0, 0, 1);
  } else if (averageAcX <= -AcLow) {
    wingControl(1, 1, 0, 0, 1);
  } else {
    wingControl(0, 0, 0, 0, 1);
  }
}

// Needs extensive revision before implementation
void continuousWing() {
  if (AcX > 16000) {
    AcX = 16000; //clamps the AcX reading between +/- 16,000
  }
  else if (AcX < -16000) {
    AcX = -16000;
  }

//should prevent the servos from jittering
//needs to be tested
  if (map(AcX, -16000, 16000, 112, 55) > servoOneCont * 1, 10 || map(AcX, -16000, 16000, 112, 55) < servoOneCont * 0.90) {
    servoOneCont = map(AcX, -16000, 16000, 112, 55);
  }
  if (map(AcX, -16000, 16000, 68, 145) > servoTwoCont * 1, 10 || map(AcX, -16000, 16000, 68, 145) < servoTwoCont * 0.90) {
    servoTwoCont = map(AcX, -16000, 16000, 68, 145);
  }
  if (map(AcX, -16000, 16000, 46, 106) > servoThreeCont * 1, 10 || map(AcX, -16000, 16000, 46, 106) < servoThreeCont * 0.90) {
    servoThreeCont = map(AcX, -16000, 16000, 46, 106);
  }
  if (map(AcX, -16000, 16000, 124, 64) > servoFourCont * 1, 10 || map(AcX, -16000, 16000, 124, 64) < servoFourCont * 0.90) {
    servoFourCont = map(AcX, -16000, 16000, 124, 64);
  }

  servoOneCont = map(AcX, -16000, 16000, 112, 55); //As AcX increases, servos 1 and 2 decrease in angle
  servoTwoCont = map(AcX, -16000, 16000, 68, 145);
  servoThreeCont = map(AcX, -16000, 16000, 46, 106); //As AcX increases, servos 3 and 4 increase in angle
  servoFourCont = map(AcX, -16000, 16000, 124, 64);

  servoOne.write(servoOneCont);
  servoTwo.write(servoTwoCont);
  servoThree.write(servoThreeCont);
  servoFour.write(servoFourCont);

  /*  //position is only updated if sensor reading changes by +/- 20%
    if((pos <= scaleInput(AcX)*1.20) || (pos >= scaleInput(AcX)*0.80)){
    pos = scaleInput(AcX);
    return pos;
    }*/
}

void testWing() {
  //This code swings the servo arms between different wing control positions
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

void wingControl(int servoOneAngle, int servoTwoAngle, int servoThreeAngle, int servoFourAngle, int servoFiveAngle) {

  switch (servoOneAngle) {
    case 0:
      servoOne.write(servoOneMinAngle);
      Serial.print("  ");
      Serial.print(servoOneMinAngle);
      Serial.print("  ");
      break;
    case 1:
      servoOne.write(servoOneMidLow);
      Serial.print("  ");
      Serial.print(servoOneMidLow);
      Serial.print("  ");
      break;
    case 2:
      servoOne.write(servoOneMidHigh);
      Serial.print("  ");
      Serial.print(servoOneMidHigh);
      Serial.print("  ");
      break;
    case 3:
      servoOne.write(servoOneMaxAngle);
      Serial.print("  ");
      Serial.print(servoOneMaxAngle);
      Serial.print("  ");
      break;
  }

  switch (servoTwoAngle) {
    case 0:
      servoTwo.write(servoTwoMinAngle);
      Serial.print(servoTwoMinAngle);
      Serial.print("  ");
      break;
    case 1:
      servoTwo.write(servoTwoMidLow);
      Serial.print(servoTwoMidLow);
      Serial.print("  ");
      break;
    case 2:
      servoTwo.write(servoTwoMidHigh);
      Serial.print(servoTwoMidHigh);
      Serial.print("  ");
      break;
    case 3:
      servoTwo.write(servoTwoMaxAngle);
      Serial.print(servoTwoMaxAngle);
      Serial.print("  ");
      break;
  }

  switch (servoThreeAngle) {
    case 0:
      servoThree.write(servoThreeMinAngle);
      Serial.print(servoThreeMinAngle);
      Serial.print("  ");
      break;
    case 1:
      servoThree.write(servoThreeMidLow);
      Serial.print(servoThreeMidLow);
      Serial.print("  ");
      break;
    case 2:
      servoThree.write(servoThreeMidHigh);
      Serial.print(servoThreeMidHigh);
      Serial.print("  ");
      break;
    case 3:
      servoThree.write(servoThreeMaxAngle);
      Serial.print(servoThreeMaxAngle);
      Serial.print("  ");
      break;
  }

  switch (servoFourAngle) {
    case 0:
      servoFour.write(servoFourMinAngle);
      Serial.println(servoFourMinAngle);
      break;
    case 1:
      servoFour.write(servoFourMidLow);
      Serial.println(servoFourMidLow);
      break;
    case 2:
      servoFour.write(servoFourMidHigh);
      Serial.println(servoFourMidHigh);
      break;
    case 3:
      servoFour.write(servoFourMaxAngle);
      Serial.println(servoFourMaxAngle);
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

void rollingAverageX() {
  if (summandCounterX < rollingAverageLengthX) {
    summandX[summandCounterX] = AcX;
    summandCounterX++;
  } else {
    summandCounterX = 0;
    summandX[summandCounterX] = AcX;
  }
  float sumX = 0;
  for (int i = 0; i <= rollingAverageLengthX; i++) {
    sumX = sumX + summandX[i];
  }
  averageAcX = sumX / (rollingAverageLengthX);
  Serial.print("Rolling Average AcX:  ");
  Serial.print(averageAcX);
}

void rollingAverageY() {
  if (summandCounterY < rollingAverageLengthY) {
    summandY[summandCounterY] = AcY;
    summandCounterY++;
  } else {
    summandCounterY = 0;
    summandY[summandCounterY] = AcY;
  }
  float sumY = 0;
  for (int i = 0; i <= rollingAverageLengthY; i++) {
    sumY = sumY + summandY[i];
  }
  averageAcY = sumY / (rollingAverageLengthY);
  Serial.print("Rolling Average AcY:  ");
  Serial.print(averageAcY);
}

void setup() {

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  Serial.begin(9600);
  delay(4000);

  for (i = 0; i <= 50; i++) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 12, true);
    AcX = Wire.read() << 8 | Wire.read();
    errorAcX = AcX + errorAcX;
    AcY = Wire.read() << 8 | Wire.read();
    errorAcY = AcY + errorAcY;
  }

  errorAcX = errorAcX / i;
  errorAcY = errorAcY / i;

  offsetAcX = -errorAcX;
  offsetAcY = -errorAcY;

  Wire.endTransmission(true);

  servoOne.attach(servoOnePin);
  servoTwo.attach(servoTwoPin);
  servoThree.attach(servoThreePin);
  servoFour.attach(servoFourPin);
  servoFive.attach(servoFivePin);

  pinMode(switchAPin, INPUT_PULLUP);
  pinMode(switchBPin, INPUT_PULLUP);
  pinMode(lowDragButtonPin, INPUT);

  Serial.print("Switch A: ");
  Serial.print(digitalRead(switchAPin));

  Serial.print("   Switch B: ");
  Serial.print(digitalRead(switchBPin));

  Serial.print("   Low Drag Button: ");
  Serial.println(digitalRead(lowDragButtonPin));

  delay(3000);

  //because mode selection is in setup, it can only be done while the system is powered off
  mode = modeSelect();
  //mode = 0;

  //initialization test
  testWing();

  //initializes continuous variables to minimum anglw values
  servoOneCont = 55;
  servoTwoCont = 145;
  servoThreeCont = 46;
  servoFourCont = 124;
}

void loop() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 12, true);
  AcX = (Wire.read() << 8 | Wire.read()) + offsetAcX;
  AcY = (Wire.read() << 8 | Wire.read()) + offsetAcY;
  AcZ = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  /*
    Serial.print("Accelerometer: ");
    Serial.print("X = "); Serial.print(AcX);
    Serial.print(" | Y = "); Serial.print(AcY);
    Serial.print(" | Z = "); Serial.println(AcZ);

    Serial.print("Gyroscope: ");
    Serial.print("X = "); Serial.print(GyX);
    Serial.print(" | Y = "); Serial.print(GyY);
    Serial.print(" | Z = "); Serial.println(GyZ);
    Serial.println("");
  */

  Serial.print(AcX);
  Serial.print("  ");
  Serial.print(AcY);
  Serial.print("  ");
  Serial.print(AcZ);
  Serial.print("  ");

  /* First, we monitor the brake sensor. If that requires action it takes priority.
      Second, we check for the momentary switch which indicates the drivers desires low drag mode
      Finally, if we see neither the brake nor low drag triggers, we move into the selected mode
  */

  rollingAverageX();
  rollingAverageY();

  switch (mode) {
    case 0:
      wingControl(0, 0, 0, 0, 0); //low drag mode
      break;
    case 1:
      brakeCheck();
      lowDragCheck();
      discreteWing();
      break;
    case 2:
      brakeCheck();
      lowDragCheck();
      continuousWing();
      break;
    case 3:
      wingControl(3, 3, 3, 3, 2); // high downforce mode
  }
  delay(printDelay);
  /*
    Serial.print(servoOne.read());
    Serial.print("  ");
    Serial.print(servoTwo.read());
    Serial.print("  ");
    Serial.print(servoThree.read());
    Serial.print("  ");
    Serial.println(servoFour.read());
  */
  /*
    Serial.print("Servo 1: ");
    Serial.print(servoOne.read());
    Serial.print("  Servo 2: ");
    Serial.print(servoTwo.read());
    Serial.print("  Servo 3: ");
    Serial.print(servoThree.read());
    Serial.print("  Servo 4: ");
    Serial.println(servoFour.read());
  */
}
