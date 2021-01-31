
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




void setupHTTP() {
  server.on("/", handleRoot);


  //server.on("/i2c", []() {
  //
  //  StaticJsonDocument<200> response;
  //  response["configVersion"] = "1.0";
  //  response["device"] = "";
  //  server.send(200, "application/json", response );
  //
  //});

  server.on("/status", []() {
    StaticJsonDocument<200> doc;
    doc["status"] = "1.0";
    doc["ch1"] = String( current_frame.ch1 );
    doc["ch2"] = String( current_frame.ch2 );
    doc["ch3"] = String( current_frame.ch3 );
    doc["ch4"] = String( current_frame.ch4 );

    server.send(200, "application/json", doc.as<String>() );
  });

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


  //  server.on("/test", []() {
  //    runTestProgram();
  //    server.send(200, "application/json", "{ \"status\":\"ok\"}" );
  //  });
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

  server.on("/frame", []() {
    String ch1 = getHTTPParam("ch1");
    if ( ch1.length() > 0 ) {
      Serial.println("Setting ch1: " + ch1 );  
      current_frame.ch1 = ch1.toInt();
    }
    String ch2 = getHTTPParam("ch2");
    if ( ch2.length() > 0 ) {
      Serial.println("Setting ch2: " + ch2 );  
      current_frame.ch2 = ch2.toInt();
    }
    String ch3 = getHTTPParam("ch3");
    if ( ch3.length() > 0 ) {
      Serial.println("Setting ch3: " + ch3 );  
      current_frame.ch3 = ch3.toInt();
    }
    String ch4 = getHTTPParam("ch4");
    if ( ch4.length() > 0 ) {
      Serial.println("Setting ch4: " + ch4 );  
      current_frame.ch4 = ch4.toInt();
    }

    StaticJsonDocument<200> doc;
    doc["status"] = "1.0";
    doc["ch1"] = String( current_frame.ch1 );
    doc["ch2"] = String( current_frame.ch2 );
    doc["ch3"] = String( current_frame.ch3 );
    doc["ch4"] = String( current_frame.ch4 );

    server.send(200, "application/json", doc.as<String>() );
  });

  server.on("/wipe", []() {
    strip.setBrightness(100);
    colorWipe(strip.Color(255, 255, 255), 50);
    strip.show();

    //SetAll( 120, 120, 120);

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
