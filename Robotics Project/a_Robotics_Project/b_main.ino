void loop() {
  // Detect the first bar
  detectBlack = detectBar('s');
  if (detectBlack == 1) {
#ifdef DEBUG
    setLEDs(1, 0, 0);
#endif
    int complete = barcodeDetection(); // If all four bars are detected this will return 1
    if (complete == 1) { 
      barcode.type = getType(); // Check which barcode was detected
      Serial.println(barcode.type);

      switch (barcode.type) {
        case 'l':
          //Turn left
          LBCODE();
          break;
        case 'r':
          // Turn right
          RBCODE();
          break;
        case 's':
          // Stop
          SBCODE();
          break;
        case 'u':
          // Turn round
          UBCODE();
          break;
        case 'd':
          // Do dance
          DBCODE();
          break;
        case 'n':
          break;
        default:
          break;
      }
    }

    eraseBarcode(); // Clear values from previous barcode

  }
  int detected = detect(); //Look for obstacles -- is finding obstacles all the time at the moment
  if (detected == 1) {
#ifdef DEBUG
    Serial.println("Obstacle Detected");
#endif
/* This is the manoeuvre to avoid obstacles */
    cease();
    setLEDs(1, 1, 1);
    delay(100);
    turnAngle(90);
    moveStraight(25);
    turnAngle(-90);
    detected = 0;
  }
  // Reset values
  setLEDs(0, 0, 0);
  setSpeeds(10, 10);
}
