#include <Wire.h>

#include <i2cdetect.h>
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
const char* password = "<Your WiFi Password>";
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

#include "fauxmoESP.h"

#include <WiFiClient.h>
#include <WiFiUdp.h>
//#include <ArduinoOTA.h>


#include <Servo.h>
Servo myservo;

uint64_t chipid;

String _hostname = "boop";


Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, 2, NEO_GRB + NEO_KHZ800);

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
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     10 // Reset pin # (or -1 if sharing Arduino reset pin)


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setupDisplay() {

  //  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
  //    Serial.println(F("SSD1306 allocation failed"));
  //    for (;;); // Don't proceed, loop forever
  //  }

  display.begin(SSD1306_SWITCHCAPVCC);  // Switch OLED
  display.clearDisplay();


  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(WHITE); // Draw white text
  display.setCursor(0, 0);    // Start at top-left corner
  //  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println("Workstudyplay Thing");
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


int servoPin = 14;

void setup(void)
{
  Serial.begin(115200);
  //i2cdetect();
  //setupDisplay();

  setupNeoPixels();

  Serial.println("attaching servo to pin: " + String( servoPin ) );
  myservo.attach(servoPin);


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

void openFlower() {
  if (pos >= 180) {
    Serial.println("flower already open");
    return;
  }
  Serial.println("START openFlower");
  for (pos = 0; pos <= pos_max; pos += 1) {
    // in steps of 1 degree
    myservo.write(pos);
    float b = 100 * pos / pos_max;
    //Serial.println( "At position: " + String(b) + ":" + String(pos) + "/" + String(pos_max) );

    strip.setBrightness(int(b));
    int i = 0;
    if (b < 25 ) {
      Serial.println("0-25");
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 00));
      }
    } else if (b < 50) {
      Serial.println("50-75");
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 0));
      }
    } else if (b < 75) {
      Serial.println("50-75");
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 0));
      }
    } else {
      Serial.println("75-100");
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 255));
      }
    }
    strip.show();
    delay(15);
  }
  Serial.println("END openFlower");
}

void closeFlower() {
  if (pos <= 0) {
    Serial.println("flower already closed");
    return;
  }
  for (pos = pos_max; pos >= 0; pos -= 1) {
    // in steps of 1 degree
    myservo.write(pos);

    float b = 100 * pos / pos_max;
    Serial.println( "At position: " + String(b) + ":" + String(pos) + "/" + String(pos_max) );

    strip.setBrightness(int(b));
    int i = 0;
    if (b < 25 ) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 0, 00));
      }
    } else if (b < 50) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 0));
      }
    } else if (b < 75) {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(255, 255, 0));
      }
    } else {
      for (i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, strip.Color(0, 255, 255));
      }
    }
    strip.show();
    delay(15);
  }
  Serial.println("END closeFlower");
}

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
  openFlower();
  goHome();
}
void handleCloseFlower() {
  closeFlower();
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

  server.on("/open-flower", handleOpenFlower);
  server.on("/close-flower", handleCloseFlower);
  server.on("/rainbow", handleRainbow);

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

    String pos = getHTTPParam("pos");

    if ( pos.length() > 0 ) {
      Serial.println("Set servo position: " + String(pos.toInt()) );
      myservo.write(pos.toInt());
    }

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
