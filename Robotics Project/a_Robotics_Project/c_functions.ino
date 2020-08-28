/*All functions for the robot*/

void calibrateServos() {
  int stopValue = 90;
  char key; // Variable to hold which button was pressed
  /* Calibrate right hand servo */
  while (1) { // Loop until break when both buttons are pressed
    rightServo.write(stopValue);
    key = getKey();
    Serial.print(stopValue);
    Serial.print(" ");
    Serial.println(key);
    if (key == 'r') {
      stopValue++; // If the right button (PB2) is pressed increase the stop value
    } else if (key == 'l') {
      stopValue--; // If the left button (PB4) is pressed decrease the stop value
    } else if (key == 'b') {
      RSTOP = stopValue; // When both buttons are pressed, break from the loop
      break;
    }
  }
  /* Calibrate left hand servo */
  while (1) {
    leftServo.write(stopValue);
    key = getKey();
    Serial.print(stopValue);
    Serial.print(" ");
    Serial.println(key);
    if (key == 'r') {
      stopValue++;
    } else if (key == 'l') {
      stopValue--;
    } else if (key == 'b') {
      LSTOP = stopValue;
      break;
    }
  }
}

void calibrateLDR () {

  int leftLDR, rightLDR, midLDR = 0;

  for (int index = 0; index < 2; index++) { // Perform the operation twice - for light and dark
    char button = getKey(); // Wait for a button to be pressed and retturn which one
    if (button == 'r') { // get dark calibration
      for (int index = 0; index < 5; index++) {
        // Get the LDR readings
        leftLDR = analogRead(L_LDR);
        rightLDR = analogRead(R_LDR) - 100; // This LDR was reading an average of 100 over the other two and was scewing the averages
        midLDR = analogRead(M_LDR);

        leftDark += leftLDR;
        midDark += midLDR;
        rightDark += rightLDR;
      }
      // Divide by 5 to get the average reading of each LDR
      leftDark /= 5;
      midDark /= 5;
      rightDark /= 5;
    } else if (button == 'l') { // get light calibration
      for (int index = 0; index < 5; index++) {
        leftLDR = analogRead(L_LDR);
        rightLDR = analogRead(R_LDR) - 100;
        midLDR = analogRead(M_LDR);

        leftLight += leftLDR;
        midLight += midLDR;
        rightLight += rightLDR;

      }
      // Divide by 5 to get the average reading of each LDR
      leftLight /= 5;
      midLight /= 5;
      rightLight /= 5;
    }
  }
}

/* MOVEMENT FUNCTIONS */

void setSpeeds(int leftSpeed, int rightSpeed) {
  rightServo.write(RSTOP - rightSpeed);
  leftServo.write(LSTOP + leftSpeed);
}

void moveStraight(int dist) { //dist in cm
  int timeMeter = 16140; // average time for a meter in ms
  float distMeter = dist / 100.0; // convert cm to m
  float delayTime = timeMeter * abs(distMeter);

  if (dist > 0 ) {
    setSpeeds(30, 30);
  } else {
    setSpeeds(-30, -30);
  }
  delay(delayTime);
  cease();
  delay(50);
  return;
}

void turnAngle(int angle) { // angle in degrees, negative = anti-clockwise

  float fullRotationTime = 6790.0; // average time for full rotation in ms
  float oneDegreeRotation = fullRotationTime / 360.0;

  if (angle > 0) { // Positive turns the robot right
    setSpeeds(30, -30);
  } else { // Negative turns the robot left
    setSpeeds(-30, 30);
  }

  delay(oneDegreeRotation * abs(angle)); // Turn for the of amount time calculated for the angle wanted
  cease(); // Stop turning
  delay(50);
  return;
}

void cease() {
  setSpeeds(0, 0);
  return;
}


/* BUTTON PRESS FUNCTIONS */

void waitKey(char button) { // 'r' = right button, 'l' = left button
  if (button == 'r') {
    while (digitalRead(PBR) == HIGH) {};
    delay(20);
    while (digitalRead(PBR) == LOW) {};
  } else if (button == 'l') {
    while (digitalRead(PBL) == HIGH) {};
    delay(20);
    while (digitalRead(PBL) == LOW) {};
  }
}

char getKey() { // returns 'b' - both, 'r' - right, 'l' - left

  char button;

  while (digitalRead(PBR) == HIGH && digitalRead(PBL) == HIGH) {}; //Wait for either button to be pressed

  if (digitalRead(PBL) == LOW) { // If it's left first
    delay(50);
    if (digitalRead(PBR) == LOW) { // Test if both are pressed
      button = 'b';
    } else { // Otherwise just left is pressed
      button = 'l';
    }
  } else if (digitalRead(PBR) == LOW) { // If it's right first
    delay(50);
    if (digitalRead(PBL) == LOW) { // Test if both are pressed
      button = 'b';
    } else { // Otherwise just right is pressed
      button = 'r';
    }
  }

  delay(15);

  while (digitalRead(PBR) == LOW || digitalRead(PBL) == LOW) {}; // Wait for either button to be released
  return button;
}


/* LDR FUNCTIONS */

void getLDRReadings(int readings[numOfLDRs]) {
  readings[0] = analogRead(L_LDR);
  readings[1] = analogRead(M_LDR);
  readings[2] = analogRead(R_LDR) - 100; // This LDR was reading an average of 100 more than the other 2 which was scewing the average

#ifdef DEBUG
  Serial.println("Left   Mid  Right");
  for (int index = 0; index < numOfLDRs; index++) {
    Serial.print(readings[index]);
    Serial.print("   ");
  }
  Serial.println();
  delay(100);
#endif
}

char liOrDar (int readings[3], char ldr) {
  int bounds = 40;
  if (ldr == 'l') { // Testing left LDR
    if (readings[0] <= leftDark + bounds) { // If the reading is less than the dark average for LDR
      return 'd'; // DARK
    } else if (readings[0] >= leftLight - bounds) { // Else it is greater than the light average
      return 'l'; // LIGHT
    } else {
      return 'n';
    }
  } else if (ldr == 'm') { // Testing mid LDR
    if (readings[1] <= midDark + bounds) {
      return 'd';
    } else if (readings[1] >= midLight - bounds) {
      return 'l';
    } else {
      return 'n';
    }
  } else if (ldr == 'r') { // Testing right LDR
    if (readings[2] <= rightDark + bounds) {
      return 'd';
    } else if (readings[2] >= rightLight - bounds) {
      return 'l';
    } else {
      return 'n';
    }
  }
}

/* LED FUNCTIONS */

void setLEDs(int green_state, int yellow_state, int red_state) {
  digitalWrite(GREEN, green_state);
  digitalWrite(RED, red_state);
  digitalWrite(YELLOW, yellow_state);
  return;
}

/* IR FUNCTIONS */

int detect() {
  tone(IRLED, IRfreq);
  delay(5);
  if (digitalRead(IRREC) == LOW) {
    return 1; // Obstacle detected
  } else {
    return 0;
  }
  noTone(IRLED);
}

/* BARCODE FUNCTIONS */

int barcodeDetection() {
  unsigned long startTime, endTime, width;
  unsigned long timeout = millis() + 7000;
  unsigned long timeNow = millis();

  for ( int index = 0; index < numOfBars; index++) {
    startTime = millis();
    detectWhite = detectBar('e'); //detect end of black bar
    while (detectWhite != 1) {
      detectWhite = detectBar('e'); //Try to find the bar end

      timeNow = millis(); // Update the timer
      if (timeNow >= timeout) { // Check to see if needs to timeout
        Serial.println("TIMEOUT");
        return 0; // Break from the function
      }
    }

    endTime = millis();
    width = endTime - startTime; // Calculate the width of the bar from the time difference and store in the array
    barcode.bars[index] = width;

#ifdef DEBUG
    setLEDs(0, 0, 0);
#endif

    if (index == 3) { // Reached the end of the barcode so dont need to check for new bar
      for (int index = 0; index < numOfBars; index++) {
        barcode.sequence[index] = barcodeRead(index); // Work out the order of the thin or thick bars and store the sequence
      }
      return 1; // Break out of function with value 1 to continue running the barcode detection algorithm
    } else { // go to next bar

      detectBlack = detectBar('s'); //detect start of next bar
      while (detectBlack != 1) {
        detectBlack = detectBar('s');

        timeNow = millis(); // Update the timer
        if (timeNow >= timeout) { // Check to see if it should timeout
          Serial.println("TIMEOUT");
          return 0; // break from the function
        }
      }
#ifdef DEBUG
      setLEDs(1, 0, 0);
#endif
    }
  }
}

int detectBar (char bar) { // Bar represents which transition looking for, 's' for w -> b, 'e' for b -> w

  char left, mid, right;

  getLDRReadings(readings);

  // check to see if individual ldrs are light or dark
  left = liOrDar(readings, 'l');
  mid = liOrDar(readings, 'm');
  right = liOrDar(readings, 'r');
  if (bar == 's') {
    if (mid == 'd' && (left == 'd' || right == 'd')) { // If 2 out of 3 record dark 
      return 1; // return 'bar detected'
    } else {
      return 0; // return 'no bar'
    }
  } else if (bar == 'e') {
    if (mid == 'l' && (left == 'l' || right == 'l')) { // If 2 out of 3 read light 
      return 1; // return 'end of bar detected'
    } else {
      return 0; // return 'no bar'
    }
  }
}

char barcodeRead(int index) {  // determines sequence of thick and thin stripes: 's' for thin stripes, 'b' for thick stripes
  const int threshold = 680;
  int bar;
  bar = barcode.bars [index];
  if (bar < threshold) {
    return 's';
  } else if ( bar > threshold ) {
    return 'b';
  }


}

char getType() {
  const int numOfBCodes = 5;
  const char BARCODES[numOfBCodes][numOfBars] = {
    {'b', 'b', 's', 's'}, //left
    {'s', 'b', 's', 'b'}, //right
    {'s', 's', 'b', 's'}, //uturn
    {'s', 'b', 'b', 's'}, //stop
    {'b', 's', 's', 'b'}  //dance
  };
  int count;
  for  (count = 0; count < numOfBCodes; count++) { // loop through the different barcodes
    for  (int index = 0; index < numOfBars; index++) { // loop through the sequences of each barcode in turn
      if (barcode.sequence[index] == ' ') { // If gap in the read barcode sequnce
        // to avoid incorrect reading of barcode if there are missing values
        return 'n';
      }
      // if the barcode at index is the same, continue with for loop unless at end of array then have found the correct barcode
      if (barcode.sequence[index] == BARCODES[count][index] ) {
#ifdef DEBUG
        Serial.print(barcode.sequence[index]);
        Serial.print(" ");
#endif
        if (index == 3) {
#ifdef DEBUG
          Serial.println();
#endif
          goto ENDCHECK; // If all values are the same, dont bother going through rest of barcodes
        }
      } else { // if they aren't the same at this index, go to next barcode
        break;
      }
    }



  }
ENDCHECK: switch (count) { // count sows which barcode was found to be the correct one
    case 0:
      //left
      return 'l';
      break;
    case 1:
      //right
      return  'r';
      break;
    case 2:
      //uturn
      return  'u';
      break;
    case 3:
      //stop
      return  's';
      break;
    case 4:
      //dance
      return  'd';
      break;
    default:
      return 'n';
      break;
  }

}

void eraseBarcode() {
  for (int index = 0; index < numOfBars; index ++) {
    barcode.sequence[index] = ' ';
  }
  barcode.type = ' ';
  return;
}

void LBCODE () {
  cease();
  setLEDs(1, 0, 0);
  delay(200);
  turnAngle(-90);
  return;
}
void RBCODE () {
  cease();
  setLEDs(0, 1, 0);
  delay(200);
  turnAngle(90);
  return;
}
void UBCODE () {
  cease();
  setLEDs(1, 1, 0);
  delay(200);
  turnAngle(180);
  return;
}
void SBCODE () {
  cease();
  setLEDs(0, 0, 1);
  waitKey('l');
  return;
}
void DBCODE () {
  unsigned long timer = millis() + 20000;
  while (millis() < timer) {
    setLEDs(1, 1, 1);
    moveStraight(5);
    setLEDs(0, 0, 1);
    moveStraight(-5);
    setLEDs(1, 0, 1);
    turnAngle(45);
    setLEDs(0, 1, 0);
    moveStraight(5);
    setLEDs(1, 1, 0);
    moveStraight(-5);
    setLEDs(0, 0, 0);
    turnAngle(135);
    setLEDs(1, 1, 1);
    moveStraight(-10);
    setLEDs(1, 1, 0);
    turnAngle(-180);
    setLEDs(0, 1, 1);
    moveStraight(10);
    setLEDs(1, 1, 1);
    turnAngle(360);
    if (millis() >= timer) {
      setLEDs(1, 0, 0);
      delay(20);
      setLEDs(1, 1, 0);
      delay(20);
      setLEDs(1, 1, 1);
      delay(30);
      break;
    }
  }
  return;
}
