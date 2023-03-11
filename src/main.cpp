// Platform Libs
#include <WiFiClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Arduino.h>
#include <LittleFS.h>

// general purpose Libs
#include <SchmittTrigger.h>
#include <VirtualButton.h>
#include <ESPTelnet.h>
#include <CircularBuffer.h>

// ESP8266 specific Libs
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WiFiMulti.h>



class SchmittButton : public VirtualButton {
  public:
    SchmittButton(SchmittTrigger<int> trigger, bool invert);

  private:
    SchmittTrigger<int> trigger_;

    bool read(void) override;
};

SchmittButton::SchmittButton(SchmittTrigger<int> trigger, bool invert): VirtualButton(invert), trigger_(trigger){

}

bool SchmittButton::read(void){
  return trigger_.output();
}


#define abs(x) ((x)>0?(x):-(x))

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);
ESPTelnet telnet;


const int hall_port = D4;
const int led = LED_BUILTIN;
uint32 inpult_num=0;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
unsigned long long boot_time;
SchmittTrigger<int> trigger(520, 550);
SchmittButton schmittButton(trigger, false);

struct tim_and_val {
 uint32 time;
 uint32 value;
} ;

void handleNotFound(){
  String message = "";
  message += "# HELP gas gasmeter inpulses\n";
  message += "# TYPE gas counter\n";
  message += "gas_total ";
  message += inpult_num;
  message += "\n";
  message += "gas_created ";
  message += boot_time;
  message += "\n";

  server.send(200, "text/plain", message);
}

void handleReboot(){
  server.send(200, "text/plain", "Restart_Ok");
  delay(500);
  ESP.restart();
}


void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("gas");
  LittleFS.begin();

  File f = LittleFS.open("/wifi.txt","r");
  if(f){
    while(f.available()){
      String user = f.readStringUntil('\n');
      String pass = f.readStringUntil('\n');
      wifiMulti.addAP(user.c_str(), pass.c_str());
      Serial.println(user+":"+pass);
    }
  }

  f.close();
  LittleFS.end();

  Serial.println("");

  pinMode(hall_port, INPUT_PULLUP);
  

  // Wait for connection
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();

  if (MDNS.begin("gas")) {
    Serial.println("MDNS responder started");
  }
  timeClient.begin();
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  boot_time = timeClient.getEpochTime();
  Serial.print("Boot TimeDate: ");
  Serial.println(boot_time);

  server.on("/metrics", handleNotFound);
  server.on("/reboot", handleReboot);
  //server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  telnet.begin();
  Serial.println("Telnet server started");

}

void loop(void){
  wifiMulti.run();
  telnet.loop();
  timeClient.update();
  server.handleClient();
  ArduinoOTA.handle();
  
  int read = analogRead(A0);
  
  trigger.input(read);
  schmittButton.update(trigger.output());

  telnet.println(String(read)+","+trigger.output()+","+inpult_num);

  if(schmittButton.just_pressed()){
    inpult_num++;
  }

  // Kleine Pause für wifi Kommunikation
  delay(100);
}
