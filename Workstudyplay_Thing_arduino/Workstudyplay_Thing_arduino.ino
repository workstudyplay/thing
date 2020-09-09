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

#include <BleKeyboard.h>
BleKeyboard bleKeyboard("Workstudyplay Thing", "Workstudyplay.org", 100);
void setupBLE() {
  Serial.println("Starting BLE keyboard");
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

const char* ssid = "<Your SSID>>";
const char* password = "<WiFi password>";
String _hostname = "orlando";

WebServer server(80);

void toggleZoomAudio() {
  Serial.println("Sending Toggle Audio.");
  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.print("a");
  bleKeyboard.releaseAll();
}
void toggleZoomVideo() {
  Serial.println("Sending Toggle Video.");
  bleKeyboard.press(KEY_LEFT_GUI);
  bleKeyboard.press(KEY_LEFT_SHIFT);
  bleKeyboard.print("v");
  bleKeyboard.releaseAll();
}

const int led = 13;
const int audioLed = 27;
const int videoLed = 12;

void handleRoot() {

  server.send(200, "text/html", "<script>function sendCommand(cmd){ var l=document.getElementsByTagName('button'); for(var i=0; i<l.length;i++){ var e=l[i]; e.disabled=true;e.innerHTML='loading...';};window.location=cmd;}</script><button onclick=\"sendCommand('/toggle-audio')\">Toggle Audio</button><button onclick=\"sendCommand('/toggle-video')\">Toggle Video</button>");

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

void setup(void) {
  Wire.begin();
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);

  i2cdetect();

  setupBLE();

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

  //  if (MDNS.begin("esp32")) {
  //    Serial.println("MDNS responder started");
  //  }

  server.on("/", handleRoot);

  server.on("/toggle-audio", toggleAudio);
  server.on("/toggle-video", toggleVideo);

  server.on("/scan", []() {
    Serial.println("Scanning I2C...");
    server.send(200, "text/plain", "scanning");
    i2cdetect();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  //  handleBLE();
  server.handleClient();
}
