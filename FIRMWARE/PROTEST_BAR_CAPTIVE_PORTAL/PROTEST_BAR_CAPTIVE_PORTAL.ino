
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

//#include <Adafruit_NeoPixel.h>
//#include <NeoPixelBus.h>
#include <WiFi.h>
#include <DNSServer.h>

//--------------------------------------------------------------------------------
//--------/ DEFINE /--------------------------------------------------------------]
//--------------------------------------------------------------------------------

#define PIN 12
#define NUM_LEDS 8
#define STATUS_LED 13

//--------------------------------------------------------------------------------
//--------/ DECLARE /-------------------------------------------------------------
//--------------------------------------------------------------------------------

int values[] = {16, 32, 64, 127, 255, 127, 64, 32};
int counter = 0;
int interval = 200;
int pastTime = 0;
bool toggle = false;
const byte DNS_PORT = 53;

IPAddress apIP(192,168,1,1);
DNSServer dnsServer;
WiFiServer server(80);
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRBW + NEO_KHZ800);
//NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(NUM_LEDS, PIN);


//--------------------------------------------------------------------------------
//--------/ HTML RESPONSE /-------------------------------------------------------
//--------------------------------------------------------------------------------

String responseHTML = ""
                      "<!DOCTYPE html>"
                      "<html>"
                      "<head>"
                      "<title>[ PROTEST_BAR ]</title>"
                      "<style>"
                      "html{font-size:60px;font-family:monospace;}"
                      "</style>"
                      "</head>"
                      "<body>"
                      "<h1>[ PROTEST_BAR ]</h1>"
                      "<p>Abuse Of Power Comes As No Surprise</p>"
                      "</body>"
                      "</html>";

//--------------------------------------------------------------------------------
//--------/ SETUP /---------------------------------------------------------------
//--------------------------------------------------------------------------------

void setup() {
  pinMode(STATUS_LED, OUTPUT);
  //strip.Begin();
  //strip.Show(); // Initialize all pixels to 'off'
  //updateLED();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("[ PROTEST_BAR ]");
  dnsServer.start(DNS_PORT, "*", apIP);
  server.begin();
}

//--------------------------------------------------------------------------------
//--------/ LOOP /----------------------------------------------------------------
//--------------------------------------------------------------------------------

void loop() {

  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients
  
  if (client) {
    //blink();
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

void blink() {
  digitalWrite(STATUS_LED, HIGH);
  delay(20);
  digitalWrite(STATUS_LED, LOW);
}

void updateLED() {

  int temp = values[7];

  for (int i = NUM_LEDS - 1; i >= 1; i--) {
    values[i] = values[i - 1];
  }

  values[0] = temp;

  for (int i = 0; i < NUM_LEDS; i++) {
    //strip.SetPixelColor(i, RgbwColor(0, 1, 0, values[i]/16));
  }

  delay(1);
  //strip.Show();

}

//--------------------------------------------------------------------------------
//--------/ EOF /-----------------------------------------------------------------
//--------------------------------------------------------------------------------
