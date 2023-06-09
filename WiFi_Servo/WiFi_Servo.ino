#include "WifiCam.hpp"
#include <WiFi.h>
#include <ESP32Servo.h>

Servo myservo;

static const char* WIFI_SSID = "";
static const char* WIFI_PASS = "";

int pos = 35;
int servoPin = 12;
int rotation = 0;

int servo_stay_delay = 20000;
int servo_angle_delay = 5;

unsigned long current_time;
unsigned long prev_time = 0;
unsigned long prev_time_stay = 0;

esp32cam::Resolution initialResolution;

WebServer server(80);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  delay(2000);

  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi failure");
    delay(5000);
    ESP.restart();
  }
  Serial.println("WiFi connected");

  {
    using namespace esp32cam;

    initialResolution = Resolution::find(1024, 768);

    Config cfg;
    cfg.setPins(pins::AiThinker);
    cfg.setResolution(initialResolution);
    cfg.setJpeg(80);

    bool ok = Camera.begin(cfg);
    if (!ok) {
      Serial.println("camera initialize failure");
      delay(5000);
      ESP.restart();
    }
    Serial.println("camera initialize success");
  }

  Serial.println("camera starting");
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  addRequestHandlers();
  server.begin();
  
  myservo.setPeriodHertz(50);
	myservo.attach(servoPin);
}

void loop(){
  server.handleClient();
  current_time = millis();
  if(current_time - prev_time >= servo_angle_delay){
    prev_time = current_time;
    if(rotation == 0){
      myservo.write(pos);
      Serial.println(pos);
      if(pos == 90){
        if(current_time - prev_time_stay >= servo_stay_delay){
          prev_time_stay = current_time;
          rotation = 1;
        }
      }
      else{
        pos++;
      }
    }
    else if(rotation == 1){
      myservo.write(pos);
      Serial.println(pos);
      if(pos == 130){
        if(current_time - prev_time_stay >= servo_stay_delay){
          prev_time_stay = current_time;
          rotation = 2;
        }
      }
      else{
        pos++;
      }
    }
    else if(rotation == 2){
      myservo.write(pos);
      Serial.println(pos);
      if(pos == 90){
        if(current_time - prev_time_stay >= servo_stay_delay){
          prev_time_stay = current_time;
          rotation = 3;
        }
      }
      else{
        pos--;
      }
    }
    else{
      myservo.write(pos);
      Serial.println(pos);
      if(pos == 35){
        if(current_time - prev_time_stay >= servo_stay_delay){
          prev_time_stay = current_time;
          rotation = 0;
        }
      }
      else{
        pos--;
      }
    }
  }
}