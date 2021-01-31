#include <ArduinoJson.h>
#include "FS.h"
#include <LittleFS.h>

#include <Wire.h>

#include "I2CScanner.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#if defined(ESP8266)
// START ESP 8266 HEADERS  - - - - - - - - - - - - - - - -
#include <ESP8266mDNS.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include <ArduinoOTA.h>
const char* ssid = "gbsx";
const char* password = "OrlandoNakazawa!";
ESP8266WebServer server(80);

#elif defined(ESP32)
// START ESP 32 HEADERS  - - - - - - - - - - - - - - - -

//#include <WiFiClient.h>
//#include <ESP8266WebServer.h>

char* ssid     = "<Your SSID>";
char* password = "<Your WiFi Password>!";
WiFiServer server(80);

//TwoWire leftBus = TwoWire(0);
////TwoWire rightBus = TwoWire(1);
//
//
//void scanI2C_1() {
//  Serial.println("Scanning I2C Addresses Channel 1");
//  uint8_t cnt = 0;
//  for (uint8_t i = 0; i < 128; i++) {
//    leftBus.beginTransmission(i);
//    uint8_t ec = leftBus.endTransmission(true);
//    if (ec == 0) {
//      if (i < 16)Serial.print('0');
//      Serial.print(i, HEX);
//      cnt++;
//    }
//    else Serial.print("..");
//    Serial.print(' ');
//    if ((i & 0x0f) == 0x0f)Serial.println();
//  }
//  Serial.print("Scan Completed, ");
//  Serial.print(cnt);
//  Serial.println(" I2C Devices found.");
//}

#else

#endif


#include <Adafruit_PWMServoDriver.h>
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  140 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  450 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

I2CScanner scanner;

#include <WiFiClient.h>
#include <WiFiUdp.h>

uint64_t chipid;
String _hostname = "boop";

bool loadConfig() {
  File configFile = LittleFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return false;
  }
  size_t size = configFile.size();
  if (size > 1024) {
    Serial.println("Config file size is too large");
    return false;
  }
  // Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  // We don't use String here because ArduinoJson library requires the input
  // buffer to be mutable. If you don't use ArduinoJson, you may as well
  // use configFile.readString instead.
  configFile.readBytes(buf.get(), size);

  StaticJsonDocument<200> doc;
  auto error = deserializeJson(doc, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }
  const char* configVersion = doc["configVersion"];
  const char* hostName = doc["hostname"];
  Serial.print("Loaded configVersion: ");
  Serial.println(configVersion);
  Serial.print("Loaded hostname: ");
  Serial.println(hostName);
  _hostname = hostName;
  return true;
}

#define _TASK_SLEEP_ON_IDLE_RUN
#define _TASK_STATUS_REQUEST
#include <TaskScheduler.h>

Scheduler ticker;
int _now = 0;
unsigned long _tickCount = 0;
unsigned long _frameCount = 0;
int FPS = 10;

#define PROGRAM_BUFFER_LENGTH 1200
struct sequence_frame_t {
  bool has_data;
  int sequence_index;

  int ch1;
  int ch2;
  int ch3;
  int ch4;

} sequence_steps[PROGRAM_BUFFER_LENGTH];

sequence_frame_t current_frame;

bool tickerRunning = false;
bool ch1_increment = true;
void TickCallback() {
  if (tickerRunning == false ) {
    return;
  }
  _frameCount++;
  //  Serial.println("Frame: " + String(_frameCount) + "/" + String(FPS) + "" );
  if ( _frameCount == 0 || _frameCount % FPS == 0 ) {
    //    if (current_frame.ch1 >= 255) {
    //      ch1_increment = false;
    //    } else if (current_frame.ch1 <= 0 ) {
    //      ch1_increment = true;
    //    }
    //
    //    if (ch1_increment) {
    //      current_frame.ch1++;
    //    } else {
    //      current_frame.ch1--;
    //    }
    Serial.printf("frame ticker: %03d %03d %03d %03d \n", current_frame.ch1, current_frame.ch2, current_frame.ch3, current_frame.ch4 );
    _frameCount = 0;
    _tickCount++;
  }
  //programFrameCallback();
  
  pwm.setPWM(0, 0, current_frame.ch1);
  pwm.setPWM(1, 0, current_frame.ch2);
  pwm.setPWM(2, 0, current_frame.ch3);

  _now++;
}

Task tTicker( 1000 / FPS, TASK_FOREVER, &TickCallback, &ticker, true);
void startTickers() {
  tickerRunning = true;
  Serial.println("Starting tickers");
  current_frame.ch1 = 0;
  current_frame.ch2 = 0;
  current_frame.ch3 = 0;
  current_frame.ch4 = 0;
}

int dimmer_program_length = 0;
int dimmer_program_position = 0;
#define NO_VALUE -999

void clearDimmerProgram() {
  dimmer_program_length = 0;
  dimmer_program_position = 0;
  return;

  for ( int n = 0; n < PROGRAM_BUFFER_LENGTH; n++) {
    sequence_frame_t frame = sequence_steps[n];
    frame.has_data = false;
    frame.sequence_index = NULL;

    frame.ch1 = NO_VALUE;
    frame.ch2 = NO_VALUE;
    frame.ch3 = NO_VALUE;
    frame.ch4 = NO_VALUE;
  }
}

float calculateSlope(int x1, int y1, int x2, int y2) {
  // rise over run
  float s = ((float)y1 - (float)y2) / ((float)x1 - (float)x2);
  //Serial.println( String(s) + " = (" + String(y1) + "-"+ String(y2) + ") / (" + String(x1) + "-"+ String(x2) + ")" );
  return s;
}
void addProgramSequence( int channel, int start_value, int end_value, int time_in_ms ) {
  // since we know the fps, we can calculate the values as a function of time
  int totalFrames = time_in_ms / FPS;

  Serial.printf("Generated %d frames", totalFrames);
  // here we need to calculate the value as a function of start_value end_value n
  float slope = calculateSlope(0, start_value, totalFrames, end_value);

  for ( int x = 0; x < totalFrames; x++) {
    float float_value =  ( (float)slope * (float)x) + (float)start_value;
    //Serial.println( "Float_value= " + String(float_value) );

    int value = int( float_value );
    //Serial.println( "x=" + String(x) + ", y="+String( value) );

    sequence_frame_t frame;

    frame.has_data = true;
    frame.sequence_index = true;

    frame.ch1 = NO_VALUE;
    frame.ch2 = NO_VALUE;
    frame.ch3 = NO_VALUE;
    frame.ch4 = NO_VALUE;

    if ( channel == 1 || channel == 0) frame.ch1 = value;
    if ( channel == 2 || channel == 0) frame.ch2 = value;
    if ( channel == 3 || channel == 0) frame.ch3 = value;
    if ( channel == 4 || channel == 0) frame.ch4 = value;

    // add the frame to the sequences
    sequence_steps[dimmer_program_length] = frame;

    dimmer_program_length++;
  }
  Serial.println("Adding " + String(totalFrames) + " frames to program, Program length is now: " + String( dimmer_program_length ) );
}

void setupServos() {
  Serial.println("Setting up servo controller");
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(10);
}

// You can use this function if you'd like to set the pulse length in seconds
// e.g. setServoPulse(0, 0.001) is a ~1 millisecond pulse width. It's not precise!
void setServoPulse(uint8_t n, double pulse) {
  double pulselength;

  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period");
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit");
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}
bool program_play_forward = true;

void programFrameCallback() {
  //  Serial.println("programFrameCallback");
  //  if ( dimmer_program_position + 1 >= dimmer_program_length ) {
  //    Serial.println("Program Complete");
  //    // if we are configured to loop, then restart the program
  //    dimmer_program_position = 0;
  //    return;
  //  }

  // go to the next frame
  //dimmer_program_position++;

  if ( program_play_forward ) {
    if (dimmer_program_position >= dimmer_program_length) {
      Serial.println("Program Complete forward");
      // complete
      program_play_forward = false;
      dimmer_program_position++;
    } else {
      dimmer_program_position++;
    }

  } else {
    if (dimmer_program_position <= 0) {
      Serial.println("Program Complete backward");
      program_play_forward = true;
      dimmer_program_position--;
    } else {
      dimmer_program_position--;
    }
  }
  sequence_frame_t frame = sequence_steps[dimmer_program_position];

  // run the current frame
  if ( frame.ch1 != NO_VALUE ) {
    current_frame.ch1 = frame.ch1;
  }
  if ( frame.ch2 != NO_VALUE ) {
    current_frame.ch2 = frame.ch2;
  }
  if ( frame.ch3 != NO_VALUE ) {
    current_frame.ch3 = frame.ch3;
  }
  if ( frame.ch4 != NO_VALUE ) {
    current_frame.ch4 = frame.ch4;
  }
}

void runTestProgram() {
  int high = SERVOMAX;
  int low = SERVOMIN;
  addProgramSequence( 0, low, high, 2 * 1000 );
  // addProgramSequence( 1, high, low, .2 * 1000 );
  //  addProgramSequence( 1, 100, 0, 2*1000 );
  //  addProgramSequence( 1, 0, 50, 2*1000 );
  //  addProgramSequence( 1, 50, 0, 2*1000 );
  //  addProgramSequence( 1, 0, 100, 1*1000 );
  //  addProgramSequence( 1, 100, 0, .5*1000 );
  //
  //  addProgramSequence( 1, 0, 0, 5*1000 );
}

int pulseWidth(int angle) {
  int pulse_wide, analog_value;
  pulse_wide   = map(angle, 0, 180, SERVOMIN, SERVOMAX);
  analog_value = int(float(pulse_wide) / 1000000 * SERVO_FREQ * 4096);
  Serial.println(analog_value);
  return analog_value;
}

void setServo( int servonum, int angle ) {
  pwm.setPWM(servonum, 0, pulseWidth(angle));
  //  Serial.println(servonum);
  //  for (uint16_t pulselen = SERVOMIN; pulselen < pos; pulselen++) {
  //    pwm.setPWM(servonum, 0, pulselen);
  //  }
}

void closeServo( int servonum ) {
  Serial.println("Close Servo: " + String(servonum) );
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen += 10) {
    Serial.println("Pulse: " + String( pulselen ) );
    pwm.setPWM(servonum, 0, pulselen);
    delay(250);
  }
}

void openServo( int servonum ) {
  Serial.println("Open Servo: " + String(servonum) );
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen -= 10) {
    Serial.println("Pulse: " + String( pulselen ) );
    pwm.setPWM(servonum, 0, pulselen);
    delay(100);
  }
}

void openFlower( int servonum ) {
  Serial.println("Close Servo: " + String(servonum) );
  for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen += 1) {
    Serial.println("Pulse: " + String( pulselen ) );
    pwm.setPWM(servonum, 0, pulselen);
    delay(100);
  }
}
void closeFlower( int servonum ) {
  Serial.println("Open Servo: " + String(servonum) );
  for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen -= 1) {
    Serial.println("Pulse: " + String( pulselen ) );
    pwm.setPWM(servonum, 0, pulselen);

    //strip.setPixelColor(i, strip.Color(0, 255, 255));
    delay(50);
  }
}

#define NEOPIXEL_PIN 2
#define NEOPIXEL_NUM_PIXELS 16
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED
  display.clearDisplay();
  display.display();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);    // Start at top-left corner
  //  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println("Workstudyplay Thing");
  display.display();
}

void setupNeoPixels() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  strip.begin();
  colorWipe(strip.Color(255, 255, 255), 50);

  strip.setBrightness(20);
  strip.show(); // Initialize all pixels to 'off'
}

//#include <WebSocketClient.h>
//using namespace net;
//WebSocketClient wsClient;
//
//void setupWS() {
//  // Ethernet/WiFi initialization goes here ...
//  // ...
//
//  client.onOpen([](WebSocket &ws) {
//    const char message[]{ "Hello from Arduino client!" };
//    ws.send(message, strlen(message));
//  });
//  client.onClose([](WebSocket &ws, const WebSocket::CloseCode &code,
//                   const char *reason, uint16_t length) {
//    // ...
//  });
//  client.onMessage([](WebSocket &ws, const WebSocket::DataType &dataType,
//                     const char *message, uint16_t length) {
//    // ...
//  });
//
//  client.open("echo.websocket.org", 80);
//}

#include "http-server.h"
#include "ota.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Mounting FS...");
  if (!LittleFS.begin()) {
    Serial.println("Failed to mount file system");
    return;
  }
  if (!loadConfig()) {
    Serial.println("Failed to load config");
  } else {
    Serial.println("Config loaded");
  }
  scanner.Init();
  scanner.Scan();

  setupServos();
  //setupDisplay();

  setupNeoPixels();

  Serial.println("connecting WiFi: " + String(ssid) );
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Set up mDNS responder:
  // - first argument is the domain name, in this example
  //   the fully-qualified domain name is "esp8266.local"
  // - second argument is the IP address to advertise
  //   we send our IP address on the WiFi network
  if (!MDNS.begin(_hostname.c_str())) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
 // runTestProgram();
  startTickers();

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");
  setupHTTP();

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  configureOTA();
  ArduinoOTA.begin();
}

int pos_max = 180;
int pos;

void loop()
{
  if (tickerRunning) {
    ticker.execute();
  }
  server.handleClient();
  ArduinoOTA.handle();
}
