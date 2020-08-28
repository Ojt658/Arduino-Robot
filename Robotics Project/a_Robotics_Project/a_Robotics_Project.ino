/* Initiate variables, pinModes and run setup() */
//#define DEBUG

#include <Servo.h>

#define LOOP 1

/* Buttons */
#define PBR 2
#define PBL 4

/*LDRs */
#define R_LDR A0
#define M_LDR A1
#define L_LDR A2

/* LEDs */
#define GREEN 7
#define YELLOW 12
#define RED 13

/* IRs */
#define IRLED 3
#define IRREC 2
#define IRfreq 38000

#define RServo 5
#define LServo 6

/* Auto stop values for servos */
int RSTOP = 91;
int LSTOP = 86;

/* Variables used for LDRs */
const int numOfLDRs = 3;
int readings[numOfLDRs], leftDark, leftLight, midDark, midLight, rightDark, rightLight = 0;


/* Variables for barcode reading */
const int numOfBars = 4;
int detectBlack, detectWhite;

struct bcode {
  long bars[numOfBars];
  char sequence[numOfBars];
  char type = 'n';
};

bcode barcode;

Servo rightServo;
Servo leftServo;

void setup() {

  Serial.begin(9600);

  cease();

  // Setup pins for buttons, LDRs, LEDs and attach servos
  pinMode(PBR, INPUT);
  pinMode(PBL, INPUT);

  pinMode(R_LDR, INPUT);
  pinMode(M_LDR, INPUT);
  pinMode(L_LDR, INPUT);

  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);

  pinMode(IRLED, OUTPUT);
  pinMode(IRREC, INPUT);

  leftServo.attach(LServo);
  rightServo.attach(RServo);

  char key = getKey();
  if (key == 'r') {
    calibrateServos();
    calibrateLDR();
#ifdef DEBUG
    Serial.print("LDR DARK VALUES: "); Serial.print(leftDark); Serial.print(" "); Serial.print(midDark); Serial.print(" "); Serial.println(rightDark);
    Serial.print("LDR LIGHT VALUES: "); Serial.print(leftLight); Serial.print(" "); Serial.print(midLight); Serial.print(" "); Serial.println(rightLight);
#endif
  } else if (key == 'l') {
    leftDark = 440;
    midDark = 450;
    rightDark = 430;
    leftLight = 725;
    midLight = 750;
    rightLight = 700;
  }
  cease();
  // Run an object detection
  int detected = detect();
  waitKey('l');
  setSpeeds(10, 10);

}
