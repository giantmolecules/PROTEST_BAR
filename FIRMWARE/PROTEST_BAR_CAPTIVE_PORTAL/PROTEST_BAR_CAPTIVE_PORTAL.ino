//--------------------------------------------------------------------------------
//--------/ HEADER /---------------------------------------------------------------
//--------------------------------------------------------------------------------


//  August 31, 2019
//  PROTEST_BAR_CAPTIVE_PORTAL
//  Brett Ian Balogh
//  brettbalogh@gmail.com

//  This sketch creates an AP with a DNS server that resolves all requests to the
//  internal server. It therefore functions as a captive portal and should pop up
//  a connect page on a computer or mobile device. The user can determine what 
//  content is served. A bonus feature is a strip of addressable LEDs that can
//  be animated in many different ways.


//--------------------------------------------------------------------------------
//--------/ INCLUDE /-------------------------------------------------------------
//--------------------------------------------------------------------------------

#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <DNSServer.h>

//--------------------------------------------------------------------------------
//--------/ DEFINE /--------------------------------------------------------------
//--------------------------------------------------------------------------------

#define PIN 12
#define NUM_LEDS 8

//--------------------------------------------------------------------------------
//--------/ DECLARE /-------------------------------------------------------------
//--------------------------------------------------------------------------------

int values[] = {16,32,64,127,255,127,64,32};
int counter = 0;
int interval = 100;
int pastTime = 0;
bool toggle = false;
const byte DNS_PORT = 53;

IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
WiFiServer server(80);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);

//--------------------------------------------------------------------------------
//--------/ HTML RESPONSE /-------------------------------------------------------
//--------------------------------------------------------------------------------

String responseHTML = ""
                      "<!DOCTYPE html>"
                      "<html>"
                          "<head>"
                              "<title>[ PROTEST_BAR ]</title>"
                              "<style>"
                                  "html{font-size:72px;font-family:monospace;}"
                              "</style>"
                          "</head>"
                          "<body>"
                              "<h1>Hey Fuckers</h1>"
                              "<p>Time to Sin</p>"
                          "</body>"
                      "</html>";

//--------------------------------------------------------------------------------
//--------/ SETUP /---------------------------------------------------------------
//--------------------------------------------------------------------------------

void setup() {
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("[ PROTEST_BAR ]");
  dnsServer.start(DNS_PORT, "*", apIP);
  server.begin();
  strip.setBrightness(50);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

//--------------------------------------------------------------------------------
//--------/ LOOP /----------------------------------------------------------------
//--------------------------------------------------------------------------------

void loop() {

  if (millis() - pastTime > interval) {
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

//--------------------------------------------------------------------------------
//--------/ FUNCTIONS /-----------------------------------------------------------
//--------------------------------------------------------------------------------

void updateLED() {

  int temp = values[7];
  for(int i = strip.numPixels()-1; i >= 1; i--){
      values[i]=values[i-1];
  }
  values[0]=temp;

  for (int i = 0; i < strip.numPixels(); i++) {
        uint32_t rgbcolor = strip.gamma32(strip.ColorHSV(10922, 1, values[i]));
    strip.setPixelColor(i, rgbcolor);
  }
  delay(2);
  strip.show();
}

//--------------------------------------------------------------------------------
//--------/ EOF /-----------------------------------------------------------------
//--------------------------------------------------------------------------------
