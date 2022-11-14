//-------------------------------------------------------------------------------
//  TinyCircuits ST BLE TinyShield UART Example Sketch
//  Last Updated 2 March 2016
//
//  This demo sets up the BlueNRG-MS chipset of the ST BLE module for compatiblity 
//  with Nordic's virtual UART connection, and can pass data between the Arduino
//  serial monitor and Nordic nRF UART V2.0 app or another compatible BLE
//  terminal. This example is written specifically to be fairly code compatible
//  with the Nordic NRF8001 example, with a replacement UART.ino file with
//  'aci_loop' and 'BLEsetup' functions to allow easy replacement. 
//
//  Written by Ben Rose, TinyCircuits http://tinycircuits.com
//
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

#if defined (ARDUINO_ARCH_AVR)
#define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
#define SerialMonitorInterface SerialUSB
#endif


uint8_t ble_rx_buffer[21];
uint8_t ble_rx_buffer_len = 0;
uint8_t ble_connection_state = false;
#define PIPE_UART_OVER_BTLE_UART_TX_TX 0

uint8_t menuTextY[4] = {12, 25, 38, 51};

uint8_t rewriteMenu = false;

const FONT_INFO& font8pt = liberationSansNarrow_8ptFontInfo;
const FONT_INFO& font10pt = thinPixel7_10ptFontInfo;
const FONT_INFO& font22pt = liberationSansNarrow_22ptFontInfo;

uint8_t defaultFontColor = TS_8b_White;
uint8_t defaultFontBG = TS_8b_Black;
uint8_t inactiveFontColor = TS_8b_Gray;
uint8_t inactiveFontBG = TS_8b_Black;

int brightness = 3;
uint8_t lastSetBrightness = 100;
unsigned long millisOffsetCount = 0;
unsigned long lastMainDisplayUpdate = 0;
int sleepTimeout = 60;
uint8_t displayOn = 0;
unsigned long sleepTimer = 0;
uint8_t buttonReleased = 1;

// Pyserial Part
String incomingByte;
char* token;
char data[255];
char FullName[16];
char HotelNum[16];
// End of Pyserial

int hh, mm, ss;
int timerMM = 50, timerSS = 0;

void setup() {
  SerialMonitorInterface.begin(9600);
  BLEsetup();
  Wire.begin();
  display.begin();
  display.setFlip(true);
  initHomeScreen();
  requestScreenOn();
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

uint32_t millisOffset() {
#if defined (ARDUINO_ARCH_AVR)
  return millis();
#elif defined(ARDUINO_ARCH_SAMD)
  return (millisOffsetCount * 1000ul) + millis();
#endif
}

void loop() {
  aci_loop();//Process any ACI commands or events from the NRF8001- main BLE handler, must run often. Keep main loop short.
  if (ble_rx_buffer_len) {//Check if data is available
    SerialMonitorInterface.print(ble_rx_buffer_len);
    SerialMonitorInterface.print(" : ");
    SerialMonitorInterface.println((char*)ble_rx_buffer);
    ble_rx_buffer_len = 0;//clear afer reading
  }
  if (SerialMonitorInterface.available()) {//Check if serial input is available to send
    delay(10);//should catch input
    uint8_t sendBuffer[21];
    uint8_t sendLength = 0;
    while (SerialMonitorInterface.available() && sendLength < 19) {
      sendBuffer[sendLength] = SerialMonitorInterface.read();
      sendLength++;
    }
    if (SerialMonitorInterface.available()) {
      SerialMonitorInterface.print(F("Input truncated, dropped: "));
      if (SerialMonitorInterface.available()) {
        SerialMonitorInterface.write(SerialMonitorInterface.read());
      }
    }
    sendBuffer[sendLength] = '\0'; //Terminate string
    sendLength++;
    if (!lib_aci_send_data(PIPE_UART_OVER_BTLE_UART_TX_TX, (uint8_t*)sendBuffer, sendLength))
    {
      SerialMonitorInterface.println(F("TX dropped!"));
    }
  }  
checkButtons();

}

void sWatchFunction() {
  for (;;) {
    hh = RTCZ.getHours();
    mm = RTCZ.getMinutes();
    ss = RTCZ.getSeconds();
    int softReset = 0;  // flag for resume button but not hardreset
    if (softReset == 0) {
      if (display.getButtons(TSButtonLowerLeft))  // start
      {
        RTCZ.setTime(0, 0, 0);
        RTCZ.begin();
      }
      if (display.getButtons(TSButtonLowerRight)) {  // init soft reset
        softReset == 1;
        display.setFont(font10pt);
        display.setCursor(0, menuTextY[3]);
        display.print("Resume");
        hh = RTCZ.getHours();
        mm = RTCZ.getMinutes();
        ss = RTCZ.getSeconds();
        SerialMonitorInterface.println(mm);
        SerialMonitorInterface.println(hh);
        SerialMonitorInterface.println(ss);
        break;
      }
    } 

    else if (softReset == 1) {
      if (display.getButtons(TSButtonLowerLeft))  // resume from softreset
      {
        softReset = 0;
        display.setFont(font10pt);
        display.setCursor(0, menuTextY[3]);
        display.print("Start");
        //hh = RTCZ.getHours();
        //mm = RTCZ.getMinutes();
        //ss = RTCZ.getSeconds();
        //SerialMonitorInterface.println(mm);
        //SerialMonitorInterface.println(hh);
        //SerialMonitorInterface.println(ss);
        RTCZ.setTime(hh, mm, ss);
        RTCZ.begin();
      }
      if (display.getButtons(TSButtonLowerRight)) {  //hardreset
        softReset = 0;
        hh = 0;
        mm, ss = 0;
        break;
      }
    }

  display.setFont(font22pt);
  display.setCursor(10, 26);
  display.print((hh / 10) % 10);
  display.print(hh % 10);
  display.print(":");
  display.print((mm / 10) % 10);
  display.print(mm % 10);
  display.print(":");
  display.print((ss / 10) % 10);
  display.print(ss % 10);
  display.setFont(font10pt);
  display.setCursor(70, menuTextY[3]);
  display.print("Stop");
  display.setCursor(0, menuTextY[3]);
  display.print("Start");
  }
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
