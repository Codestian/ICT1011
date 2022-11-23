//-------------------------------------------------------------------------------
// Hotel BLE Keyless System
// This is just the key for the hotel guests and employees.
// This code allows the watch to display time,date and day set by RTC.
// It also fetches the Guest Name and their respective room ID from the backend strapi server.
// Code is refenced from STBLE sample code and TinyCircuit Smart Watch.
// Only certain functions from TinyCiruict Smart Watch code are used.
// A password generator function is created which shows the password generated in the information menu.
//-------------------------------------------------------------------------------


#include <SPI.h>
#include <STBLE.h>
#include <Wire.h>
#include <TinyScreen.h>
#include <RTCZero.h>
#include <time.h>
RTCZero RTCZ;

#define WIFI 8
#define menu_debug_print true

TinyScreen display = TinyScreen(TinyScreenDefault);

//Debug output adds extra flash and memory requirements!
#ifndef BLE_DEBUG
#define BLE_DEBUG true
#endif

#define SerialMonitorInterface SerialUSB

uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

uint8_t menuTextY[4] = {12, 25, 38, 51};
uint8_t timeY = 14;

const FONT_INFO& font8pt = liberationSansNarrow_8ptFontInfo;
const FONT_INFO& font10pt = thinPixel7_10ptFontInfo;
const FONT_INFO& font12pt = liberationSans_12ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;

uint8_t defaultFontColor = TS_8b_White;
uint8_t defaultFontBG = TS_8b_Black;
uint8_t inactiveFontColor = TS_8b_Gray;
uint8_t inactiveFontBG = TS_8b_Black;

int brightness = 5;
uint8_t lastSetBrightness = 100;

unsigned long millisOffsetCount = 0;

unsigned long sleepTimer = 0;
int sleepTimeout = 60;

uint8_t rewriteTime = true;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
unsigned long lastMainDisplayUpdate = 0;
unsigned long mainDisplayUpdateInterval = 300;

// Pyserial Part
String incomingByte;
char* token;
char data[255];
char FullName[16];
char HotelNum[16];
// End of Pyserial

uint8_t TimeData[20];
uint8_t newtime= 0;



void setup() 
{
  RTCZ.begin();
  RTCZ.setTime(9, 50, 0);//h,m,s
  RTCZ.setDate(24, 11, 22);//d,m,y

  Wire.begin();
  SerialMonitorInterface.begin(9600);
  display.begin();
  display.setFlip(true);
  initHomeScreen();
  requestScreenOn();
  BLEsetup();
}

uint32_t millisOffset() 
{
  return (millisOffsetCount * 1000ul) + millis();
}

const char * getGuestName()
{
  /* to obtain guest name and return FIRST NAME ONLY */
   if (SerialMonitorInterface.available()) {
      incomingByte = SerialMonitorInterface.readStringUntil('\r');

      delay(100);

      int str_len = incomingByte.length() + 1;
      incomingByte.toCharArray(data, str_len);

      token = strtok(data,",");
      strcpy(FullName, token);

      token = strtok(NULL, ",");
      strcpy(HotelNum, token);

      delay(100);
   }
   return FullName;
}

const char * getRoomID()
{
  /* to obtain room ID from server and return ID */
  if (SerialMonitorInterface.available()) {
    incomingByte = SerialMonitorInterface.readStringUntil('\r');

    delay(100);

    int str_len = incomingByte.length() + 1;
    incomingByte.toCharArray(data, str_len);

    token = strtok(data,",");
    strcpy(FullName, token);

    token = strtok(NULL, ",");
    strcpy(HotelNum, token);

    delay(100);
    }
  return HotelNum;
}

/* WIFI password generator */
const char * generateWIFI(){
  /* Change to allowable characters */
  const char possible[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@!#$%&";
  static char wifiPass[WIFI + 1];
  for(int p = 0, i = 0; i < WIFI; i++){
    int r = random(0, strlen(possible));
    wifiPass[p++] = possible[r];
  }
  wifiPass[WIFI] = '\0';
  return wifiPass;
}



void loop() 
{
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.

  if (newtime) {
    newtime = 0;
    newTimeData();
  }

  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) { // for screen to remain on
    updateMainDisplay();
  }

  if (millisOffset() > sleepTimer + ((unsigned long)sleepTimeout * 1000ul)) { //go sleep
    if (displayOn) {
      displayOn = 0;
      display.off();
    }
    aci_loop();
  }
  checkButtons(); // so that the buttons work <- from smart watch base code
}

void newTimeData() {
  int y, M, d, k, m, s;
  y = (TimeData[1] << 8) | TimeData[0];
  M = TimeData[2];
  d = TimeData[3];
  k = TimeData[4];
  m = TimeData[5];
  s = TimeData[6];

  RTCZ.setTime(k, m, s);
  RTCZ.setDate(d, M, y - 2000);
}

void timeCharUpdate(uint8_t * newData, uint8_t length) {
  memcpy(TimeData, newData, length);
  newtime = millisOffset();
}

int requestScreenOn() {
  sleepTimer = millisOffset();
  if (!displayOn) {
    displayOn = 1;
    updateMainDisplay();
    display.on();
    return 1;
  }
  return 0;
}


void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn)
      buttonPress(buttons);
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x0F)) {
    buttonReleased = 1;
  }
}
