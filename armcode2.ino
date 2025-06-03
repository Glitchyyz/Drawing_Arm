#include <Arduino.h>
#include <Servo.h>
#include <math.h>
#include <Stepper.h>


Servo BaseServo;
Servo LowerServo;
Servo UpperServo;
Servo PenServo;
Servo ClawServo;
double lowerarmlength = 147.14213;
double upperarmlength = 158.24287;
String inputString = "";   
double X = 0;
double Z = 120;
double xMultiplyer = 1;
double yMultiplyer = 1;
double zMultiplyer = 1;
bool stringComplete = false;
bool InterpolationEnabled = false;
int ClawOpenness = 10;
static double BaseDif_accumulator = 0;
static double lowerdif_accumulator = 0;
static double upperdif_accumulator = 0;
static double pendif_accumulator = 0;
const int BASE_SERVO_THRESHOLD = 0;  
const int LOWER_SERVO_THRESHOLD = 0;  
const int UPPER_SERVO_THRESHOLD = 0;  
const int PEN_SERVO_THRESHOLD = 0; 
int currentSteps = 0;
double currentbaseRotation = 0;
Stepper BaseStepper = Stepper(2048, A5, A3, A4, A2);
void setup() {
  
  BaseStepper.setSpeed(9);
  ClawServo.attach(10,565,2535);
  PenServo.attach(9);
  BaseServo.attach(8, 560, 1925);
  LowerServo.attach(7, 600, 1900);
  UpperServo.attach(6, 550, 1900);
  Serial.begin(9600);
  BaseServo.writeMicroseconds(590);
  LowerServo.write(90);
  UpperServo.writeMicroseconds(550);
  inputString.reserve(50);
  ClawServo.write(ClawOpenness);
  
  /*   --------- caLIBRATIOn squares ----------
  
  
  PointLineDraw(150, 0, 250, 0, 20, -28);
  PointLineDraw(250, 0, 250, 100, 20, -28);
  PointLineDraw(250, 100, 150, 100, 20, -28);
  PointLineDraw(150, 100, 150, 0, 20, -28);


  PointLineDraw(175, 25, 225, 25, 10, -28);
  PointLineDraw(225, 25, 225, 75, 10, -28);
  PointLineDraw(225, 75, 175, 75, 10, -28);
  PointLineDraw(175, 75, 175, 25, 10, -28);
  */
  
  PenServo.write(0);
  LowerServo.write(0);
  UpperServo.write(90);
 // MOVE(150, 100, 100, 0,1);
}

void loop() {

  
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    inputString += inChar;

    if (inChar == '\n') {
      stringComplete = true;
      inputString.trim(); // Remove newline 
    }
    if(stringComplete && inputString == "IN")// IF command says to enable interpolation
    {
      InterpolationEnabled = true;
      inputString = "";
      stringComplete = false;
      Serial.println("Done");
    }else if (stringComplete && inputString == "FT")// if command says to disable interpolation
    {
      InterpolationEnabled = false;
      inputString = "";
      stringComplete = false;
      Serial.println("Done");
    }
    else if (stringComplete) {
     
      

      int comma1Index = inputString.indexOf(',');
      int comma2Index = inputString.indexOf(',', comma1Index + 1); // Find second comma after the first
      int comma3Index = inputString.indexOf(',', comma2Index + 1 );

      // Check if both commas were found in valid positions
      if (comma1Index > 0 && comma2Index > comma1Index && comma2Index < inputString.length() - 1) {

        String number1String = inputString.substring(0, comma1Index);
        String number2String = inputString.substring(comma1Index + 1, comma2Index); // Between the two commas
        String number3String = inputString.substring(comma2Index + 1, comma3Index);              // After the second comma
        String number4String = inputString.substring(comma3Index + 1);
        int number1 = number1String.toDouble();
        int number2 = number2String.toDouble();
        int number3 = number3String.toDouble();
        int number4 = number4String.toDouble();
        
        if(InterpolationEnabled)
        {
          MOVE(number1, number2, number3, true, 1);
        }else 
        {
          MOVE(number1, number2, number3, false, 1);
          
        }
        
        ClawServo.write(number4);
        Serial.println("Done");
      

      // Reset for the next input
      inputString = "";
      stringComplete = false;
    }  
  }
  }
  
}
void StepperControl(double TargetRotation)
{

  int targetstepps =  (double)(5.68888888888888888888888888888 * TargetRotation);
  int offsetSteps = currentSteps - targetstepps ;

  
    BaseStepper.step(offsetSteps);
  
  currentSteps = targetstepps;
  currentbaseRotation = TargetRotation;
}
void PointDraw(double x , double y , double Zoffset)
{
     MOVE(x, y, Zoffset + 30, 1,1.5);
    delay(100);
    MOVE(x,y, Zoffset, 1,0.4);
    delay(100);
    MOVE(x , y, Zoffset + 30, 1,0.4);
    delay(100);
}
void PointLineDraw(double x1, double y1, double x2, double y2, int points, double Zoffset) { // Draws a line with dots

  double xdif = x1 - x2;
  double ydif = y1 - y2;
  for (double i = 0; i < points + 1; i++) {
    double num = i / points;
    MOVE(x1 - xdif * num, y1 - ydif * num, Zoffset + 30, 1,1.5);
    delay(100);
    MOVE(x1 - xdif * num, y1 - ydif * num, Zoffset, 1,0.1);
    delay(100);
    MOVE(x1 - xdif * num, y1 - ydif * num, Zoffset + 30, 1,0.1);
    delay(100);
  }
}
void StraigtLineDraw(double x1, double y1, double x2, double y2, int Resolution, double Zoffset){ // Draws a Solid straight line

    double xdif = x1 - x2;
    double ydif = y1 - y2;
    delay(100);
    MOVE(x1 , y1 , Zoffset + 30, 1 ,1.5);
    delay(100);
    MOVE(x1 , y1 , Zoffset , 1,0.1);
  for (double i = 0; i < Resolution + 1; i++) {
    double num = i / Resolution;
    MOVE(x1 - xdif * num, y1 - ydif * num, Zoffset, 1,5);
    //delay(10);
  }
    MOVE(x2, y2, Zoffset + 30, 1,0.3);
}

void MOVE(double x, double y, double z, bool Interpolate, double speedMultiplier)  // Uses all the calculates and moves the arm smoothly to the position
{
  
  double originalposBot = BaseServo.readMicroseconds();
  double originalposlow = LowerServo.readMicroseconds();
  double orignalposUp = UpperServo.readMicroseconds();
  double orignalpospen = PenServo.readMicroseconds();
  double BottomservoDif = originalposBot - round(560.0 + ((1925.0 - 560.0) / 180.0) * (double)BaseCalculate(x, y));
  double LowerServoDif = originalposlow -round(600.0 + ((1900.0 - 600.0) / 180.0) * (double)LowerCalculate(x, y, z));
  double UpperServoDif = orignalposUp -round(550.0 + ((1900.0 - 550.0) / 180.0) * (double)UpperCalculate(x, y, z));
  double PenServoDif = orignalpospen -round(565.0 + ((2535.0 - 565.0) / 180.0) * (double)PenCalculate(x, y, z));
  double rotationdiffrence = currentbaseRotation - BaseCalculate(x, y);
  double origianbasepos = currentbaseRotation;
  double Progress = 0;
  double interpolatedProgress = 0;
  
  if (Interpolate) {
    double div = sqrt((double)BottomservoDif * BottomservoDif + (double)LowerServoDif * LowerServoDif + (double)UpperServoDif * UpperServoDif);
    double I = div * ((double)1/speedMultiplier);
    for (double i = 1; i < I + 1; i++) {
      Progress = (i / I);
      interpolatedProgress = interpolate(0, 1, Progress);
      //interpolatedProgress = Progress;
      BaseServo.writeMicroseconds((int)(originalposBot - ((double)BottomservoDif * interpolatedProgress)));
      StepperControl(origianbasepos - (double)(rotationdiffrence * interpolatedProgress));
      LowerServo.writeMicroseconds((int)(originalposlow - ((double)LowerServoDif * interpolatedProgress)));
      UpperServo.writeMicroseconds((int)(orignalposUp - ((double)UpperServoDif * interpolatedProgress)));
      PenServo.writeMicroseconds((int)(orignalpospen - ((double)PenServoDif * interpolatedProgress)));
      delay(5);
    }
  } else { 
    int baseTargetCurrentCmd = round(560.0 + ((1925.0 - 560.0) / 180.0) * (double)BaseCalculate(x, y));
    int lowerTargetCurrentCmd = round(600.0 + ((1900.0 - 600.0) / 180.0) * (double)LowerCalculate(x, y, z));
    int upperTargetCurrentCmd = round(550.0 + ((1900.0 - 550.0) / 180.0) * (double)UpperCalculate(x, y, z));
    int penTargetCurrentCmd = round(565.0 + ((2535.0 - 565.0) / 180.0) * (double)PenCalculate(x, y, z));

    int currentBaseActualPos = BaseServo.readMicroseconds();
    int currentLowerActualPos = LowerServo.readMicroseconds();
    int currentUpperActualPos = UpperServo.readMicroseconds();
    int currentPenActualPos = PenServo.readMicroseconds();

    double BottomservoDif = (double)baseTargetCurrentCmd - currentBaseActualPos;
    double LowerServoDif = (double)lowerTargetCurrentCmd - currentLowerActualPos;
    double UpperServoDif = (double)upperTargetCurrentCmd - currentUpperActualPos;
    double PenServoDif = (double)penTargetCurrentCmd - currentPenActualPos;

   
        BaseServo.writeMicroseconds(baseTargetCurrentCmd);
        StepperControl(BaseCalculate(x, y));

    
      LowerServo.writeMicroseconds(lowerTargetCurrentCmd);
     

      UpperServo.writeMicroseconds(upperTargetCurrentCmd);
     

        PenServo.writeMicroseconds(penTargetCurrentCmd);
       
  }

}
float interpolate(float start, float end, float t) {  // Found this interpolation equasion online
  float easedT = (t < 0.5 ? 4 * t * t * t : 1 - pow(-2 * t + 2, 3) / 2);
  return start + (end - start) * easedT;
}
double BaseCalculate(double x, double y)  // returns the rotation calculated from the coordinates
{
  

  double rotation  = atan2(y, x) * 180 / PI;
  if (x == 0 && y == 0) return 0;  // if the direction is null go to 0
  return (rotation);
}
double LowerCalculate(double x, double y, double z) { //retunes the roation calculed for the lower servo
  double distance_sq = (x * x) + (y * y);
  double firstangle = atan2(z, sqrt(distance_sq)) * 180 / PI;
  double armdistance_sq = (z * z) + (distance_sq);
  
  double acosincal_numerator = armdistance_sq + (double)lowerarmlength * lowerarmlength - (double)upperarmlength * upperarmlength;
  double acosincal_denominator = (2.0 * sqrt(armdistance_sq) * lowerarmlength);
  double acosincal = acosincal_numerator / acosincal_denominator;
  double secondangle = acos(acosincal) * 180.0 / PI;
  double finalangle = firstangle + secondangle;
  
 
  return (map(finalangle,0,90 ,180,0));
}
double PenCalculate(double x, double y, double z) {// returns the rotation calculated for the pen holding servo
  double distance_sq = (x * x) + (y * y);
  double firstangle = atan2(z, sqrt(distance_sq)) * 180 / PI;
  double armdistance_sq = (z * z) + (distance_sq);
  double acosincal_numerator = armdistance_sq + (double)lowerarmlength * lowerarmlength - (double)upperarmlength * upperarmlength;
  double acosincal_denominator = (2.0 * sqrt(armdistance_sq) * lowerarmlength);
  double acosincal = acosincal_numerator / acosincal_denominator;
  double secondangle = acos(acosincal) * 180.0 / PI;
  double finalangle = secondangle - firstangle;
  finalangle = (double)180 - finalangle;
  return (finalangle);
}
double UpperCalculate(double x, double y, double z) {// returns the rotation calculated for the Upper servo
  double distance_sq = (x * x) + (y * y);
  double armdistance_sq = (z * z) + (distance_sq);
  double acosincal_numerator = (double)upperarmlength * upperarmlength + (double)lowerarmlength * lowerarmlength - armdistance_sq;
  double acosincal_denominator = (2.0 * upperarmlength * lowerarmlength);
  double acosincal = acosincal_numerator / acosincal_denominator;
  
  double secondangle = acos(acosincal) * 180.0 / PI;

  if (isnan(secondangle)) {
    Serial.println("error calculating the upperangle");
    secondangle = 180;
  }
  return (180- secondangle);
}

