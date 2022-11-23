//-------------------------------------------------------------------------------
// This File configures the display of the Tinycircuit.
//-------------------------------------------------------------------------------


const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;

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
}

void viewInformation(uint8_t button) 
{
  currentDisplayState = displayStateMenu;
  display.clearWindow(0, 10, 96, 64);
  display.setCursor(0, 10);
  display.setFont(font10pt);
  display.print(getGuestName());
  display.setCursor(0, 22); 
  display.print("Room ");
  display.print(getRoomID());
  display.setCursor(0, 32); 
  display.print("Hotel WIFI");
  display.setCursor(0, 42); 
  display.print("SSID: H_81_Public");
  display.setCursor(0, 52); 
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
  rewriteTime = true;
  rewriteMenu = true;
  updateMainDisplay();
}

uint8_t lastDisplayedDay = -1;
uint8_t lastDisplayedMonth = -1;
uint8_t lastDisplayedYear = -1;

void updateDateDisplay() 
{
  int currentDay = RTCZ.getDay();
  int currentMonth = RTCZ.getMonth();
  int currentYear = RTCZ.getYear();

  if ((lastDisplayedDay == currentDay) && (lastDisplayedMonth == currentMonth) &&(lastDisplayedYear == currentYear))
    return;
 
  lastDisplayedDay = currentDay;
  lastDisplayedMonth = currentMonth;
  lastDisplayedYear = currentYear;
  
  display.setFont(font10pt);
  display.fontColor(defaultFontColor, defaultFontBG);
  display.setCursor(2, 2);

  const char * wkday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  time_t currentTime = RTCZ.getEpoch();
  struct tm* wkdaycalc = gmtime(&currentTime);
  display.print(wkday[wkdaycalc->tm_wday]);
  display.print(' ');
  display.print(RTCZ.getMonth());
  display.print('/');
  display.print(RTCZ.getDay());
  display.print(F("  "));
}

void updateMainDisplay() {
  if (lastSetBrightness != brightness) {
    display.setBrightness(brightness);
    lastSetBrightness = brightness;
  }
  displayBattery();
  updateDateDisplay();
  if (currentDisplayState == displayStateHome) {
      updateTimeDisplay();
      if (rewriteMenu)
      {
        display.setFont(font10pt);
        display.clearWindow(0, menuTextY[2], 96, 13);
        int printPos = 13;
        display.setCursor(printPos, menuTextY[2]);
        display.print("Hotel Verified");
      }
      display.setCursor(0, menuTextY[3] + 2);
      display.print(F("< Menu          "));
      char viewStr[] = "Info >";
      int Xpos = 95 - display.getPrintWidth(viewStr);
      display.setCursor(Xpos, menuTextY[3]);
      display.print(viewStr);
      rewriteMenu = false;
  }
  lastMainDisplayUpdate = millisOffset();
}

uint8_t lastAMPMDisplayed = 0;
uint8_t lastHourDisplayed = -1;
uint8_t lastMinuteDisplayed = -1;
uint8_t lastSecondDisplayed = -1;

void updateTimeDisplay() {
  int currentHour, currentMinute, currentSecond;

  currentHour = RTCZ.getHours();
  currentMinute = RTCZ.getMinutes();
  currentSecond = RTCZ.getSeconds();

  if (currentDisplayState != displayStateHome)
    return;

  char displayX;
  int hour12 = currentHour;
  int AMPM = 1;
  if (hour12 > 12) {
    AMPM = 2;
    hour12 -= 12;
  }
  display.fontColor(defaultFontColor, defaultFontBG);
  if (rewriteTime || lastHourDisplayed != hour12) {
    display.setFont(font22pt);
    lastHourDisplayed = hour12;
    displayX = 0;
    display.setCursor(displayX, timeY);
    if (lastHourDisplayed < 10)display.print('0');
    display.print(lastHourDisplayed);
    display.write(':');
    if (lastAMPMDisplayed != AMPM) {
      if (AMPM == 2)
        display.fontColor(inactiveFontColor, inactiveFontBG);
      display.setFont(font10pt);
      display.setCursor(displayX + 80, timeY - 0);
      display.print(F("AM"));
      if (AMPM == 2) {
        display.fontColor(defaultFontColor, defaultFontBG);
      } else {
        display.fontColor(inactiveFontColor, inactiveFontBG);
      }
      display.setCursor(displayX + 80, timeY + 11);
      display.print(F("PM"));
      display.fontColor(defaultFontColor, defaultFontBG);
    }
  }

  if (rewriteTime || lastMinuteDisplayed != currentMinute) {
    display.setFont(font22pt);
    lastMinuteDisplayed = currentMinute;
    displayX = 14 + 14 - 1;
    display.setCursor(displayX, timeY);
    if (lastMinuteDisplayed < 10)display.print('0');
    display.print(lastMinuteDisplayed);
    display.write(':');
  }

  if (rewriteTime || lastSecondDisplayed != currentSecond) {
    display.setFont(font22pt);
    lastSecondDisplayed = currentSecond;
    displayX = 14 + 14 + 14 + 14 - 2;
    display.setCursor(displayX, timeY);
    if (lastSecondDisplayed < 10)display.print('0');
    display.print(lastSecondDisplayed);
  }
  rewriteTime = false;
}

void displayBattery() {
  int result = 0;

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
  uint8_t x = 70;
  uint8_t y = 3;
  uint8_t height = 5;
  uint8_t length = 20;
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
}
