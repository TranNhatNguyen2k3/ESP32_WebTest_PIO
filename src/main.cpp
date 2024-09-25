#include <WiFi.h>
#include <SPIFFS.h>
#include <WebServer.h>

 
const char* ssid = "HUUPHAP";
const char* password = "2024@018";
const int ledPin = 2;
 
WebServer server(80);

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

void setup() {
  Serial.begin(115200);

  
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

   
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

 
  server.on("/", HTTP_GET, []() {
    File file = SPIFFS.open("/index.html", "r");
    if (!file) {
      server.send(404, "text/plain", "File not found");
      return;
    }
    server.streamFile(file, "text/html");
    file.close();
  });
  server.on("/led/on", HTTP_GET, []() {
    digitalWrite(ledPin, HIGH);  
    server.send(200, "text/plain", "LED is ON");
  });

  server.on("/led/off", HTTP_GET, []() {
    digitalWrite(ledPin, LOW);  
    server.send(200, "text/plain", "LED is OFF");
  });

   
  server.onNotFound([]() {
    String path = server.uri();
    if (SPIFFS.exists(path)) {
      File file = SPIFFS.open(path, "r");
      server.streamFile(file, getContentType(path));
      file.close();
    } else {
      server.send(404, "text/plain", "File not found");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

 
void loop() {
  server.handleClient();
}