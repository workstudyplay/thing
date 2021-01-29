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
const char* ssid = "<Your SSID>";
const char* password = "<Your WiFi Password>!";
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

//bool saveConfig() {
//  StaticJsonDocument<200> doc;
//  doc["serverName"] = "api.example.com";
//  doc["accessToken"] = "128du9as8du12eoue8da98h123ueh9h98";
//
//  File configFile = LittleFS.open("/config.json", "w");
//  if (!configFile) {
//    Serial.println("Failed to open config file for writing");
//    return false;
//  }
//
//  serializeJson(doc, configFile);
//  return true;
//}


#include <Adafruit_PWMServoDriver.h>
// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  300 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  460 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

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

int pulseWidth(int angle)
{
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
#define NEOPIXEL_NUM_PIXELS 32
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

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");


  setupHTTP();

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  configureOTA();
  ArduinoOTA.begin();


}

void configureOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
}

int pos_max = 180;
int pos;


//void closeFlower() {
//  if (pos <= 0) {
//    Serial.println("flower already closed");
//    return;
//  }
//  for (pos = pos_max; pos >= 0; pos -= 1) {
//    // in steps of 1 degree
//    myservo.write(pos);
//
//    float b = 100 * pos / pos_max;
//    Serial.println( "At position: " + String(b) + ":" + String(pos) + "/" + String(pos_max) );
//
//    strip.setBrightness(int(b));
//    int i = 0;
//    if (b < 25 ) {
//      for (i = 0; i < strip.numPixels(); i++) {
//        strip.setPixelColor(i, strip.Color(255, 0, 00));
//      }
//    } else if (b < 50) {
//      for (i = 0; i < strip.numPixels(); i++) {
//        strip.setPixelColor(i, strip.Color(0, 255, 0));
//      }
//    } else if (b < 75) {
//      for (i = 0; i < strip.numPixels(); i++) {
//        strip.setPixelColor(i, strip.Color(255, 255, 0));
//      }
//    } else {
//      for (i = 0; i < strip.numPixels(); i++) {
//        strip.setPixelColor(i, strip.Color(0, 255, 255));
//      }
//    }
//    strip.show();
//    delay(15);
//  }
//  Serial.println("END closeFlower");
//}

void goHome() {
  server.send(200, "text/html", "<script>window.location='/'</script>");
}

void setColor() {

  int r = 255;
  int g = 255;
  int b = 255;

  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b ));
  }
}

void handleRainbow() {
  rainbow(250);

}

void handleSetColor() {


  goHome();

}

void handleOpenFlower() {
  openFlower(1);
  goHome();
}
void handleCloseFlower() {
  closeFlower(1);
  goHome();
}


String getHTTPParam( String paramName ) {

  String message = "Number of args received: ";
  message += server.args();            //Get number of parameters
  message += "\n";                            //Add a new line

  String txt_value = "";
  for (int i = 0; i < server.args(); i++) {

    message += "Arg no" + (String)i + "– > ";  //Include the current iteration value
    message += server.argName(i) + ": ";     //Get the name of the parameter
    message += server.arg(i) + "\n";              //Get the value of the parameter
    String arg = server.argName(i);
    if ( arg == paramName ) {
      txt_value = String( server.arg(i) );
      Serial.println( "Found " + paramName + " = " + txt_value );
    }
  }
  return txt_value;
}
void setupHTTP() {
  server.on("/", handleRoot);



  server.on("/config", []() {
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile) {
      Serial.println("Failed to open config file");
      return false;
    }

    size_t size = configFile.size();
    if (size > 1024) {
      Serial.println("Config file size is too large");
      server.send(200, "application/json", "{ \"status\":\"fail\" }" );
      return false;
    }

    // Allocate a buffer to store contents of the file.
    //  std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    //    configFile.readBytes(buf.get(), size);

    server.send(200, "application/json", configFile.readString() );
  });

  server.on("/setup", []() {
    String incoming_hostname = getHTTPParam("hostname");
    if ( incoming_hostname.length() > 0 ) {
      StaticJsonDocument<200> doc;
      doc["configVersion"] = "1.0";
      doc["hostname"] = incoming_hostname;
      File configFile = LittleFS.open("/config.json", "w");
      if (!configFile) {
        Serial.println("Failed to open config file for writing");
        return false;
      }
      serializeJson(doc, configFile);
      Serial.println( "Wrote config.json" );

      int ok = loadConfig();

      if ( ok == false ) {
        server.send(200, "application/json", "{ \"status\":\"fail\", \"hostname\": \"" + String(incoming_hostname) + "\" }" );
      } else {
        server.send(200, "application/json", "{ \"status\":\"ok\", \"hostname\": \"" + String(incoming_hostname) + "\" }" );
      }
    }
  });

  server.on("/open-flower", handleOpenFlower);
  server.on("/close-flower", handleCloseFlower);
  server.on("/rainbow", handleRainbow);

  server.on("/open-claw", []() {
    int servonum = 1;
    openServo( servonum );
    server.send(200, "application/json", "{ \"status\":\"ok\"}" );
  });
  server.on("/close", []() {

    for (int n = 90; n < 1000; n += 10 ) {
      setServo(1, n);
      delay(200);
    }

  });
  server.on("/close-claw", []() {
    int servonum = 1;
    closeServo( servonum );
    server.send(200, "application/json", "{ \"status\":\"ok\"}" );
  });
  server.on("/claw", []() {
    String p = getHTTPParam("p");
    if ( p.length() > 0 ) {
      Serial.println("Setting position: " + p );
      setServo(1, p.toInt());
      server.send(200, "application/json", "{ \"status\":\"ok\"}" );
    }
  });

  server.on("/control", []() {
    String brightness = getHTTPParam("brightness");
    if ( brightness.length() > 0 ) {
      Serial.println("Setting brightness: " + brightness );
      strip.setBrightness(brightness.toInt());
      strip.show();
      server.send(200, "application/json", "{ \"status\":\"ok\"}" );
    }
  });

  server.on("/wipe", []() {
    strip.setBrightness(100);
    colorWipe(strip.Color(255, 255, 255), 50);
    strip.show();

    SetAll( 120, 120, 120);

    server.send(200, "application/json", "{ \"status\":\"ok\"}" );

    Serial.println("done with request");

  });
  server.on("/light", []() {
    String r = getHTTPParam("r");
    String g = getHTTPParam("g");
    String b = getHTTPParam("b");
    String brightness = getHTTPParam("brightness");
    if ( brightness.length() > 0 ) {
      int v = brightness.toInt();
      if (v == 0) {
        r = "0";
        g = "0";
        b = "0";
        strip.setBrightness(brightness.toInt());
      } else {
        Serial.println("Setting brightness: " + brightness );
        strip.setBrightness(brightness.toInt());
        strip.show();
      }
    }

    if ( r.length() > 0 || g.length() > 0 || b.length() > 0 ) {
      for (int i = 0; i < strip.numPixels(); i++) {
        //Serial.println("setting pixel " + String(i) + " to " + r + "," + g + "," + b );
        strip.setPixelColor(i, strip.Color(r.toInt(), g.toInt(), b.toInt()));
      }
      strip.show();
    }

    //    String pos = getHTTPParam("pos");
    //
    //    if ( pos.length() > 0 ) {
    //      Serial.println("Set servo position: " + String(pos.toInt()) );
    //      myservo.write(pos.toInt());
    //    }

    // server.send(200, "application/json", "{ \"pos\":\"" + String(pos) + "\", \"r\":\"" + String(r) + "\", \"g\":\"" + String(g) + "\", \"b\":\"" + String(b) + "\" }" );
    server.send(200, "application/json", "{ \"status\":\"ok\"}" );
    Serial.println("done with request");
    //goHome();
  });
  //  server.on(" / scan", []() {
  //    Serial.println("Scanning I2C...");
  //    server.send(200, "text / plain", "scanning");
  //    i2cdetect();
  //  });

  server.onNotFound(handleNotFound);

  server.begin();
}



void SetAll( int r, int g, int b ) {

  for (int i = 0; i < strip.numPixels(); i++) {
    //Serial.println("setting pixel " + String(i) + " to " + r + "," + g + "," + b );
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

String scriptLibrary() {
  return "<script>function sendText(str){ sendCommand('/send-text?str=' + str ) };function sendCommand(cmd){ var l=document.getElementsByTagName('button'); for(var i=0; i<l.length;i++){ var e=l[i]; e.disabled=true;e.innerHTML='loading...';};window.location=cmd;}</script>";

}

String Button( String url, String label ) {
  return "<button onclick = \"sendCommand('" + url + "')\">" + label + "</button>";
}

void handleNotFound() {

  Serial.println("not found");

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
void handleRoot() {
  String html = "<script>function sendText(str){ sendCommand('/send-text?str=' + str ) };function sendCommand(cmd){ var l=document.getElementsByTagName('button'); for(var i=0; i<l.length;i++){ var e=l[i]; e.disabled=true;e.innerHTML='loading...';};window.location=cmd;}</script>";
  html = html + Button("/open-flower", "Open Flower");
  html = html + Button("/close-flower", "Close Flower");
  server.send(200, "text/html", html );
}

void loop(void)
{
  server.handleClient();
  ArduinoOTA.handle();
}
