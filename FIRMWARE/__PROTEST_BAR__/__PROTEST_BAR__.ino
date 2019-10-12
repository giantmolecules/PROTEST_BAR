#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"
#include <NeoPixelBus.h>
#include <SPIFFS.h>

DNSServer dnsServer;
AsyncWebServer server(80);

//--------------------------------------------------------------------------------
//--------/ DEFINE /--------------------------------------------------------------
//--------------------------------------------------------------------------------

#define PIN 21
#define NUM_LEDS 8
#define STATUS_LED 13

//--------------------------------------------------------------------------------
//--------/ DECLARE /-------------------------------------------------------------
//--------------------------------------------------------------------------------

NeoPixelBus<NeoGrbwFeature, Neo800KbpsMethod> strip(NUM_LEDS, PIN);

String ssids[] = {
  "       ABUSE",
  "      OF",
  "     POWER",
  "    COMES",
  "   AS",
  "  NO",
  " SURPRISE"
};

const char* PARAM_INPUT_1 = "red";
const char* PARAM_INPUT_2 = "green";
const char* PARAM_INPUT_3 = "blue";
const char* PARAM_INPUT_4 = "white";
const char* PARAM_INPUT_5 = "message";

char bssid[32];

int values[] = {16, 32, 64, 127, 255, 127, 64, 32};

int pastTime1 = 0;
int interval1 = 30;

int pastTime2 = 0;
int interval2 = 30000;

int counter = 0;

bool toggle = false;
int mode = 0;
int previousMode = 0;
bool flip = 0;

int red = 0;
int green = 0;
int blue = 0;
int white = 0;

String mac;
String ssid;
String message;

const char config_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
    <title>ESP Input Form</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
    </style>
</head>
<body>

    <p>
        [ PROTEST_BAR ] CONFIGURATION
    </p>
    
    <br>
    
    <form action="/get">
        <input type="range" min="0" max="100" value="10" class="slider" name="red">
        <br>
        <input type="range" min="0" max="100" value="5" class="slider" name="green">
        <br>
        <input type="range" min="0" max="100" value="0" class="slider" name="blue">
        <br>
        <input type="range" min="0" max="100" value="0" class="slider" name="white">
        <br>
        <input type="submit" value="Submit">
    </form><br>
    
    <form action="/get">
        <textarea name="message" rows="5" cols="33">
            It was a dark and stormy night...
        </textarea>
        <input type="submit" value="Submit">
    </form>

    <form action="/mode1">
        <input type="submit" value="SSID">
    </form>

    <form action="/mode2">
        <input type="submit" value="WEBSERVER">
    </form>

</body>
</html>

)rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
   public:
    CaptiveRequestHandler(){}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request) {
      //request->addInterestingHeader("ANY");
      return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
      /*
      //List all parameters
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isFile()) { //p->isPost() is also true
          Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if (p->isPost()) {
          Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
          Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
          if (p->name().equals("red")) {
            red = p->value().toInt();
          }
          if (p->name().equals("green")) {
            green = p->value().toInt();
          }
          if (p->name().equals("blue")) {
            blue = p->value().toInt();
          }
          if (p->name().equals("white")) {
            white = p->value().toInt();
          }
          if (p->name().equals("input5")) {
            message = p->value().c_str();
          }
          if (p->name().equals("input6")) {
            flip = 1;
            //switchMode(0);
          }
          if (p->name().equals("input7")) {
            flip = 2;
            //switchMode(1);
          }
        }
        Serial.print("RED: ");
        Serial.println(red);
        Serial.print("GREEN: ");
        Serial.println(green);
        Serial.print("BLUE: ");
        Serial.println(blue);
        Serial.print("WHITE: ");
        Serial.println(white);
      }
      */
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print(config_html);
        request->send(response);
    }
    
};

//--------------------------------------------------------------------------------
//--------/ SETUP /---------------------------------------------------------------
//--------------------------------------------------------------------------------

void setup() {
  
  Serial.begin(115200);
  pinMode(STATUS_LED, OUTPUT);
  
  if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
  }
  
  red = readFile(SPIFFS, "/red.txt").toInt();
  green = readFile(SPIFFS, "/green.txt").toInt();
  blue = readFile(SPIFFS, "/blue.txt").toInt();
  white = readFile(SPIFFS, "/white.txt").toInt();
  message = readFile(SPIFFS, "/message.txt").toInt();
  
  switchMode(0);
  
  dnsServer.start(53, "*", WiFi.softAPIP());
/*
    // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", config_html);
  });
*/
  server.on("/mode1", HTTP_GET, [](AsyncWebServerRequest *request){
    switchMode(1);
    request->send(200, "text/plain", "Switching mode, please wait...");
    
  });
    server.on("/mode2", HTTP_GET, [](AsyncWebServerRequest *request){
      switchMode(2);
      AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>[ PROTEST_BAR ]</title></head><body>");
        response->print("<h1>");
        response->print(message);
        response->print("</h1>");
        response->print("</body></html>");
        request->send(response);
});
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
      Serial.println("HELLO!");
    String inputMessage;
    String inputParam;

    /*
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      red = request->getParam(PARAM_INPUT_1)->value().toInt();
      Serial.println("RED: " + red);
    }
    // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_2)) {
      green = request->getParam(PARAM_INPUT_2)->value().toInt();
      Serial.println("GREEN: " + green);
    }
    // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_3)) {
      blue = request->getParam(PARAM_INPUT_3)->value().toInt();
      Serial.println("BLUE: " + blue);
    }
    // GET input4 value on <ESP_IP>/get?input4=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_4)) {
      white = request->getParam(PARAM_INPUT_4)->value().toInt();
      Serial.println("WHITE: " + white);
    }
    // GET input5 value on <ESP_IP>/get?input5=<inputMessage>
    else if (request->hasParam(PARAM_INPUT_5)) {
      message = request->getParam(PARAM_INPUT_5)->value();
      Serial.println("MESSAGE: " + message);
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    */

    //List all parameters
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isFile()) { //p->isPost() is also true
          Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if (p->isPost()) {
          Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
          Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
          if (p->name().equals("red")) {
            red = p->value().toInt();
            writeFile(SPIFFS, "/red.txt", p->value().c_str());
          }
          if (p->name().equals("green")) {
            green = p->value().toInt();
            writeFile(SPIFFS, "/green.txt", p->value().c_str());
          }
          if (p->name().equals("blue")) {
            blue = p->value().toInt();
            writeFile(SPIFFS, "/blue.txt", p->value().c_str());
          }
          if (p->name().equals("white")) {
            white = p->value().toInt();
            writeFile(SPIFFS, "/white.txt", p->value().c_str());
          }
          if (p->name().equals("message")) {
            message = p->value().c_str();
            writeFile(SPIFFS, "/message.txt", p->value().c_str());
          }
         
        }
      }
    viewColor();
    request->send_P(200, "text/html", config_html);
  });
  
  server.onNotFound(notFound);  
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
  server.begin();
  
  strip.Begin();
  strip.Show();
  
}

//--------------------------------------------------------------------------------
//--------/ MAIN /----------------------------------------------------------------
//--------------------------------------------------------------------------------

void loop() {
  if(mode == 1){
    if (millis() - pastTime1 > interval1) {
    if (toggle) {
      String ssidString;
      int index = counter % 7;
      ssidString += ssids[index];
      ssidString.toCharArray(bssid, 32);
      WiFi.softAP(bssid, NULL);
      blink();
      counter++;
    }
    if (!toggle) {
      updateLED();
    }
    toggle = !toggle;
    pastTime1 = millis();
  }
  }
  else{
    dnsServer.processNextRequest();
  }
}

//--------------------------------------------------------------------------------
//--------/ FUNCTIONS /-----------------------------------------------------------
//--------------------------------------------------------------------------------

//--------/ msg2array /-----------------------------------------------------------

String msg2array(String s){
  
}

//--------/ notFound /---------------------------------------------------------------

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

//--------/ readFile /---------------------------------------------------------------

String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  Serial.println(fileContent);
  return fileContent;
}

//--------/ writeFile /---------------------------------------------------------------

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}

//--------/ processor /---------------------------------------------------------------

// Replaces placeholder with stored values
String processor(const String& var){
  //Serial.println(var);
  if(var == "inputString"){
    return readFile(SPIFFS, "/inputString.txt");
  }
  else if(var == "inputInt"){
    return readFile(SPIFFS, "/inputInt.txt");
  }
  else if(var == "inputFloat"){
    return readFile(SPIFFS, "/inputFloat.txt");
  }
  return String();
}


//--------/ blink /---------------------------------------------------------------

void blink() {
  digitalWrite(STATUS_LED, HIGH);
  delay(20);
  digitalWrite(STATUS_LED, LOW);
}

//--------/ updateLED /-----------------------------------------------------------

void updateLED() {

  int temp = values[7];

  for (int i = NUM_LEDS - 1; i >= 1; i--) {
    values[i] = values[i - 1];
  }

  values[0] = temp;

  for (int i = 0; i < NUM_LEDS; i++) {
    strip.SetPixelColor(i, RgbwColor(red, green, blue, values[i] / 16));
  }

  delay(1);
  strip.Show();

}

//--------/ viewColor /-----------------------------------------------------------

void viewColor(){
  for(int i = 0; i < NUM_LEDS; i++){
    strip.SetPixelColor(i, RgbwColor(red, green, blue, 1));
  }
  strip.Show();
}

//--------/ switchMode /-----------------------------------------------------------

void switchMode(int m){
  previousMode = mode;
  if(m==0){
  WiFi.mode(WIFI_AP);
  mac = String(WiFi.macAddress());
  ssid = "[ CONFIG_" + mac.substring(12,14)+mac.substring(15)+" ]";
  WiFi.softAP(ssid.c_str());
  Serial.println();
  Serial.println("[ PROTEST_BAR ] @ "+mac);
  Serial.println();
  Serial.println("Running in CONFIG (0) Mode...");
  Serial.println();
  mode = 0;
  }
  
  if(m==1){
  WiFi.mode(WIFI_AP);
  mac = String(WiFi.macAddress());
  Serial.println();
  Serial.println("[ PROTEST_BAR ] @ "+mac);
  Serial.println();
  Serial.println("Running in SSID (1) Mode...");
  Serial.println();
  mode = 1;
  }
   if(m==2){
  WiFi.mode(WIFI_AP);
  mac = String(WiFi.macAddress());
  WiFi.softAP("[ PROTEST_BAR ]");
  Serial.println();
  Serial.println("[ PROTEST_BAR ] @ "+mac);
  Serial.println();
  Serial.println("Running in WEBSERVER (2) Mode...");
  Serial.println();
  mode = 2;
  }
}
//--------------------------------------------------------------------------------
//--------/ EOF /-----------------------------------------------------------------
//--------------------------------------------------------------------------------

