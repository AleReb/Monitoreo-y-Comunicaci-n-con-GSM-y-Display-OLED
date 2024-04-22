
/**************************************************************
 *
 * This sketch connects to a website and downloads a page.
 * It can be used to perform HTTP/RESTful API calls.
 *
 * For this example, you need to install ArduinoHttpClient library:
 *   https://github.com/arduino-libraries/ArduinoHttpClient
 *   or from http://librarymanager/all#ArduinoHttpClient
 *
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 * For more HTTP API examples, see ArduinoHttpClient library
 *
 **************************************************************/
//////////
#include <SoftwareSerial.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// SCL GPIO5
// SDA GPIO4
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 48)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//////////////////////
String  test =""; // cadena para mandar datos

//////////////////////////////////////////
#include "MPU9250.h"

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
  MPU9250 IMU(Wire,0x68);

////////////////////////////////////


// The serial connection to the GPS device
SoftwareSerial ss(D5, D6,false, 256);

///////////
// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_UBLOX
// #define TINY_GSM_MODEM_BG96
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266

// Increase RX buffer if needed
#define TINY_GSM_RX_BUFFER 512

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

// Uncomment this if you want to see all AT commands
//#define DUMP_AT_COMMANDS

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Use Hardware Serial on Mega, Leonardo, Micro
//#define SerialAT Serial1

// or Software Serial on Uno, Nano


SoftwareSerial SerialAT(D4, D3); // RX, TX //d7 y d8 

// Your GPRS credentials
// Leave empty, if missing user or pass

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "wap.tmovil.cl";
const char user[] = "wap";
const char pass[] = "wap";
// Server details
const char server[] = "api.pushingbox.com";
//char resource[]=""; 
const int  port = 80;
String value1 = "00.7";
String value2= "-2";
String value3= "0.1";

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

TinyGsmClient client(modem);
HttpClient http(client, server, port);

void setup() {
IMU.begin();

display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
    // init done
    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    display.display();
    delay(3000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
display.println("Hello, world!");
 display.display();
  
  // Set console baud rate
  SerialMon.begin(115200);
  delay(10);

  // Set GSM module baud rate
  SerialAT.begin(115200);
  delay(3000);
display.clearDisplay();
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  SerialMon.println(F("Initializing modem..."));
  modem.restart();

  String modemInfo = modem.getModemInfo();
  SerialMon.print(F("Modem: "));
  SerialMon.println(modemInfo);

  // Unlock your SIM card with a PIN
  //modem.simUnlock("1234");
}

void loop() {
///////
 //IMU.readSensor();
  String tag("mi_disposivo");

  
 // value1 += String(IMU.getAccelX_mss(),3);

  
  test = "/pushingbox?devid=vCC8791B4F809B06&tag2="+tag+"&value="+value1+"&value2="+value2+"&value3="+value3;


  
// char resource[] = "";

 display.println("Waiting for network..."); //
  display.println(apn);//  
  display.display();
  
  
  SerialMon.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    SerialMon.println(" fail");
    delay(10000);
    
    return;
  }
  SerialMon.println(" OK");

  SerialMon.print(F("Connecting to "));
  SerialMon.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    SerialMon.println(" fail");
    delay(10000);
    return;
  }

  SerialMon.print(F("Performing HTTP GET request... "));
  display.clearDisplay(); //
  int err = http.get(test);//se puede usar un string
  if (err != 0) {
    SerialMon.println(F("failed to connect"));
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  SerialMon.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    //SerialMon.println(headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    SerialMon.print(F("Content length is: "));
    SerialMon.println(length);
  }
  if (http.isResponseChunked()) {
    SerialMon.println(F("The response is chunked"));
  }
 
  String body = http.responseBody();
  SerialMon.println(F("Response:"));
  SerialMon.println(body);

  SerialMon.print(F("Body length is: "));
  SerialMon.println(body.length());

  // Shutdown

  http.stop();
  SerialMon.println(F("Server disconnected"));

  modem.gprsDisconnect();
  SerialMon.println(F("GPRS disconnected"));

  // Do nothing forevermore
  while (true) {
    delay(1000);
  }
}
