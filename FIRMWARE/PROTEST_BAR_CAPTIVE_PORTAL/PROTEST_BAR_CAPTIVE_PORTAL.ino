#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <DNSServer.h>

#define PIN 12
#define NUM_LEDS 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW+NEO_KHZ800);

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
WiFiServer server(80);

int counter = 0;
int interval = 10;
int pastTime = 0;
bool toggle = false;

String responseHTML = ""
  "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
  "<h1>Hello World!</h1><p>This is a captive portal example. All requests will "
  "be redirected here.</p></body></html>";

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("[[[ PROTEST_BAR ]]]");

  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  server.begin();

strip.setBrightness(255);
strip.begin();
strip.show(); // Initialize all pixels to 'off'

}

void loop() {

    if(millis()-pastTime>interval){
        updateLED();
        pastTime = millis();
    }

  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}

  void updateLED() {
      if(counter >= 255){
          toggle = true;
      }
      if(counter <= 0){
          toggle = false;
      }
        for(uint16_t i=0; i<strip.numPixels(); i++) {
            strip.setPixelColor(i, strip.Color(0,0,0,counter));
        }
        delay(2);
        strip.show();

        if(toggle){
            counter--;
        }

        if(!toggle){
            counter++;
        }

  }
