#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
//#include <ESPmDNS.h>
#include <Wire.h>

#include <i2cdetect.h>
/* I2C slave Address Scanner
  for 5V bus
   Connect a 4.7k resistor between SDA and Vcc
   Connect a 4.7k resistor between SCL and Vcc
  for 3.3V bus
   Connect a 2.4k resistor between SDA and Vcc
   Connect a 2.4k resistor between SCL and Vcc

*/

uint64_t chipid;


#include <BleKeyboard.h>
BleKeyboard bleKeyboard;

void setupBLE() {
  Serial.println("Starting BLE keyboard");
  bleKeyboard = BleKeyboard("Workstudyplay Keyboard", "workstudyplay.org", 100);
  printf("ESP32 Chip ID = %04X",(uint16_t)(chipid>>32));
  bleKeyboard.begin();
}
void handleBLE() {
  if (bleKeyboard.isConnected()) {
    toggleZoomAudio();
    delay(500);
    toggleZoomVideo();
    delay(5 * 1000);
  } else {
    Serial.println("Nothing connected via BLE");
  }
}

const char* ssid = "<Your SSID>";
const char* password = "<WiFi password>";
String _hostname = "<your hostname>";

WebServer server(80);

void toggleZoomAudio() {
  if (!bleKeyboard.isConnected()) {
    Serial.println("Can't Toggle Audio, no BLE connection");
    return;
  }
  Serial.println("Sending Toggle Audio.");
  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.print("a");
  bleKeyboard.releaseAll();
}
void toggleZoomVideo() {
  if (!bleKeyboard.isConnected()) {
    Serial.println("Can't Toggle Video, no BLE connection");
    return;
  }
  Serial.println("Sending Toggle Video.");
  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.print("v");
  bleKeyboard.releaseAll();
}

const int led = 13;
const int audioLed = 27;
const int videoLed = 12;

const int buttonPinAudio = 17;
const int buttonPinVideo = 16;

boolean oldPinStateAudio = LOW;
boolean oldPinStateVideo = LOW;

String scriptLibrary() {
  return "<script>function sendText(str){ sendCommand('/send-text?str=' + str ) };function sendCommand(cmd){ var l=document.getElementsByTagName('button'); for(var i=0; i<l.length;i++){ var e=l[i]; e.disabled=true;e.innerHTML='loading...';};window.location=cmd;}</script>";
}

String Button( String url, String label ) {
  return "<button onclick=\"sendCommand('" + url + "')\">" + label + "</button>";
}
void handleRoot() {
  String html = "<script>function sendText(str){ sendCommand('/send-text?str=' + str ) };function sendCommand(cmd){ var l=document.getElementsByTagName('button'); for(var i=0; i<l.length;i++){ var e=l[i]; e.disabled=true;e.innerHTML='loading...';};window.location=cmd;}</script>";
  html = html + Button("/toggle-audio", "Toggle Audio");
  html = html + Button("/toggle-video", "Toggle Video");
  html = html + "<input type=\"text\" id=\"str\" /><button onclick=\"var e=document.getElementById('str');sendText(e.value);e.value='';\">&gt;</button>";

  server.send(200, "text/html", html );
}

void toggleAudio() {
  //  digitalWrite(audioLed, 1);
  server.send(200, "text/html", "<script>window.location='/'</script>");
  toggleZoomAudio();
}

void toggleVideo() {
  //  digitalWrite(videoLed, 1);

  server.send(200, "text/html", "<script>window.location='/'</script>");
  toggleZoomVideo();
}

void sendTextResponse() {
  if (!bleKeyboard.isConnected()) {
    Serial.println("Can't Send text, no BLE connection");
    return;
  }
  String txt = getHTTPParam("str");

  Serial.println("Sending text to keyboard interface: " + txt );
  bleKeyboard.print(txt);
  bleKeyboard.releaseAll();
  server.send(200, "text/html", "<script>window.location='/'</script>");
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
      Serial.println( "Found " + paramName + "=" + txt_value );
    }
  }
  return txt_value;
}

void sendText() {

  // iterate the string and send them as keys
  bleKeyboard.print("923 4030 4708");
  bleKeyboard.print(KEY_RETURN);
  bleKeyboard.releaseAll();

}
void sendMeetingPassword() {

  // iterate the string and send them as keys
  bleKeyboard.print("rSBc7x");
  bleKeyboard.print("\n");
  bleKeyboard.releaseAll();
}

void sendEnter() {
  bleKeyboard.print("\n");
  bleKeyboard.releaseAll();
}

void handleNotFound() {
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

TwoWire leftBus = TwoWire(0);
//TwoWire rightBus = TwoWire(1);


void scanI2C_1() {
  Serial.println("Scanning I2C Addresses Channel 1");
  uint8_t cnt = 0;
  for (uint8_t i = 0; i < 128; i++) {
    leftBus.beginTransmission(i);
    uint8_t ec = leftBus.endTransmission(true);
    if (ec == 0) {
      if (i < 16)Serial.print('0');
      Serial.print(i, HEX);
      cnt++;
    }
    else Serial.print("..");
    Serial.print(' ');
    if ((i & 0x0f) == 0x0f)Serial.println();
  }
  Serial.print("Scan Completed, ");
  Serial.print(cnt);
  Serial.println(" I2C Devices found.");
}
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &leftBus, OLED_RESET);
void setupDisplay() {

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  // Clear the buffer
  display.clearDisplay();


  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(5, 5);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println(F("Workstydyplay Thing"));
}

void setupWiFi() {
  WiFi.mode(WIFI_STA);
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
  digitalWrite(led, HIGH);
}

void setupHTTP() {
  server.on("/", handleRoot);

  server.on("/toggle-audio", toggleAudio);
  server.on("/toggle-video", toggleVideo);

  server.on("/send-text", sendTextResponse);
  server.on("/send-enter", sendEnter);

  server.on("/send-meeting-id", sendTextResponse);
  server.on("/send-meeting-password", sendMeetingPassword);

  server.on("/scan", []() {
    Serial.println("Scanning I2C...");
    server.send(200, "text/plain", "scanning");
    i2cdetect();
  });

  server.onNotFound(handleNotFound);

  server
  .begin();
}
void setup(void) {
  chipid = ESP.getEfuseMac();
  
  leftBus.begin(23, 22, 100000);
  setupDisplay();
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);

  Serial.begin(115200);

  //  i2cdetect();
  //scanI2C_1();

  pinMode(buttonPinAudio, INPUT_PULLDOWN);
  pinMode(buttonPinVideo, INPUT_PULLDOWN);
  setupBLE();

  setupWiFi();

  //  if (MDNS.begin("esp32")) {
  //    Serial.println("MDNS responder started");
  //  }

  setupHTTP();

  Serial.println("HTTP server started");
}

void readButtonStates() {

  // Audio
  if (digitalRead(buttonPinAudio) == HIGH) {
    if (oldPinStateAudio == LOW) {
      toggleZoomAudio();
    }
    oldPinStateAudio = HIGH;
  } else {
    oldPinStateAudio = LOW;
  }
  // Video
  if (digitalRead(buttonPinVideo) == HIGH) {
    if (oldPinStateVideo == LOW) {
      toggleZoomVideo();
    }
    oldPinStateVideo = HIGH;
  } else {
    oldPinStateVideo = LOW;
  }
}

void loop(void) {
  server.handleClient();
  readButtonStates();
  //  handleBLE();
}
