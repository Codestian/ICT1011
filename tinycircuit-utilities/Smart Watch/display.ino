const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;
const uint8_t displayStateStopwatch = 0x04;

uint8_t currentDisplayState = displayStateHome;
void (*menuHandler)(uint8_t) = NULL;
uint8_t (*editorHandler)(uint8_t, int*, char*, void (*)()) = NULL;

const uint8_t upButton = TSButtonUpperRight;
const uint8_t downButton = TSButtonLowerRight;
const uint8_t selectButton = TSButtonLowerLeft;
const uint8_t backButton = TSButtonUpperLeft;
const uint8_t menuButton = TSButtonLowerLeft;
const uint8_t viewButton = TSButtonLowerRight;
const uint8_t clearButton = TSButtonLowerRight;

void buttonPress(uint8_t buttons) {
  if (currentDisplayState == displayStateHome) {
    if (buttons == viewButton) {
      menuHandler = viewInformation;
      menuHandler(0);
    } else if (buttons == selectButton) {
      menuHandler = viewMenu;
      menuHandler(0);
    }
  } else if (currentDisplayState == displayStateMenu) {
    if (menuHandler) {
      menuHandler(buttons);
    }
  } else if (currentDisplayState == displayStateEditor) {
    if (editorHandler) {
      editorHandler(buttons, 0, 0, NULL);
    }
  } 
  /*else if (currentDisplayState == displayStateStopwatch) {
    if (menuHandler) {
      menuHandler(buttons);
    }
  }*/
}

void viewInformation(uint8_t button) 
{
  currentDisplayState = displayStateMenu;
  display.clearWindow(0, 10, 96, 64);
  display.setCursor(0, 10);
  display.setFont(font10pt);
  display.print(getGuestName());
  display.setCursor(0, 22);
  display.setFont(font10pt);  
  display.print("Hotel WIFI");
  display.setCursor(0, 32);
  display.setFont(font10pt);  
  display.print("SSID: H_81_Public");
  display.setCursor(0, 42);
  display.setFont(font10pt);  
  display.print("Pswd: ");
  display.print(generateWIFI());

  /* Display Back Button */
  char backStr[] = ">";
  int Xpos = 95 - display.getPrintWidth(backStr);
  display.setCursor(Xpos, menuTextY[3]);
  display.print(backStr);

  /* go back to home screen */
  if (button == viewButton) 
  { 
    currentDisplayState = displayStateHome;
    initHomeScreen();
  }

}

void initHomeScreen() {
  display.clearWindow(0, 10, 96, 64);
  updateMainDisplay();
}

void updateMainDisplay() {
  if (lastSetBrightness != brightness) {
    display.setBrightness(brightness);
    lastSetBrightness = brightness;
  }
  displayBattery();
  if (currentDisplayState == displayStateHome) {
      display.setFont(font12pt);
      display.clearWindow(0, menuTextY[2], 96, 13);
      int printPos = 13;
      display.setCursor(printPos*0 + 2, 14);
      display.print("Hotel Verified");
      display.setCursor(printPos, 28);
      display.print("Room ");
      display.print(getRoomID());
      
      display.setFont(font10pt);
      display.setCursor(0, menuTextY[3]);
      display.print(F("< Menu          "));
      char viewStr[] = "Info >";
      int Xpos = 95 - display.getPrintWidth(viewStr);
      display.setCursor(Xpos, menuTextY[3]);
      display.print(viewStr);
      rewriteMenu = false;
  }
  lastMainDisplayUpdate = millisOffset();
}

void displayBattery() {
  int result = 0;
#if defined (ARDUINO_ARCH_AVR)
  //http://forum.arduino.cc/index.php?topic=133907.0
  const long InternalReferenceVoltage = 1100L;
  ADMUX = (0 << REFS1) | (1 << REFS0) | (0 << ADLAR) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (0 << MUX0);
  delay(10);
  ADCSRA |= _BV( ADSC );
  while ( ( (ADCSRA & (1 << ADSC)) != 0 ) );
  result = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L;
  //SerialMonitorInterface.println(result);
  //if(result>440){//probably charging
  uint8_t charging = false;
  if (result > 450) {
    charging = true;
  }
  result = constrain(result - 300, 0, 120);
  uint8_t x = 70;
  uint8_t y = 3;
  uint8_t height = 5;
  uint8_t length = 20;
  uint8_t amtActive = (result * length) / 120;
  uint8_t red, green, blue;
  display.drawLine(x - 1, y, x - 1, y + height, 0xFF); //left boarder
  display.drawLine(x - 1, y - 1, x + length, y - 1, 0xFF); //top border
  display.drawLine(x - 1, y + height + 1, x + length, y + height + 1, 0xFF); //bottom border
  display.drawLine(x + length, y - 1, x + length, y + height + 1, 0xFF); //right border
  display.drawLine(x + length + 1, y + 2, x + length + 1, y + height - 2, 0xFF); //right border
  for (uint8_t i = 0; i < length; i++) {
    if (i < amtActive) {
      red = 63 - ((63 / length) * i);
      green = ((63 / length) * i);
      blue = 0;
    } else {
      red = 32;
      green = 32;
      blue = 32;
    }
    display.drawLine(x + i, y, x + i, y + height, red, green, blue);
  }
#elif defined(ARDUINO_ARCH_SAMD)
  //http://atmel.force.com/support/articles/en_US/FAQ/ADC-example
  SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SAMPCTRL.bit.SAMPLEN = 0x1;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXPOS = 0x19;         // Internal bandgap input
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  // Start conversion
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;
  // Clear the Data Ready flag
  ADC->INTFLAG.bit.RESRDY = 1;
  // Start conversion again, since The first conversion after the reference is changed must not be used.
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;
  // Store the value
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete
  uint32_t valueRead = ADC->RESULT.reg;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
  result = (((1100L * 1024L) / valueRead) + 5L) / 10L;
  uint8_t x = 33;
  uint8_t y = 3;
  uint8_t height = 5;
  uint8_t length = 30;
  uint8_t red, green;
  if (result > 325) {
    red = 0;
    green = 63;
  } else {
    red = 63;
    green = 0;
  }
  display.drawLine(x - 1, y, x - 1, y + height, 0xFF); //left boarder
  display.drawLine(x - 1, y - 1, x + length, y - 1, 0xFF); //top border
  display.drawLine(x - 1, y + height + 1, x + length, y + height + 1, 0xFF); //bottom border
  display.drawLine(x + length, y - 1, x + length, y + height + 1, 0xFF); //right border
  display.drawLine(x + length + 1, y + 2, x + length + 1, y + height - 2, 0xFF); //right border
  for (uint8_t i = 0; i < length; i++) {
    display.drawLine(x + i, y, x + i, y + height, red, green, 0);
  }
#endif
}
