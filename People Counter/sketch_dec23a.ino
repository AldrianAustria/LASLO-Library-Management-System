#include <LiquidCrystal_I2C.h>
#include <WiFi.h>

#define trig_left 32
#define echo_left 33

#define trig_right 25
#define echo_right 26

WiFiClient client;

const char* ssid = "";
const char* password = "";

const uint16_t port = 80;
const char* host = "";

int distance_left = 0;
long duration_left = 0;
int res_distance_left;

int trig_distance = 15;

int distance_right = 0;
long duration_right = 0;
int res_distance_right;

int lcdColumns = 16;
int lcdRows = 2;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

int people_count = 0;
int timeout_counter = 0;

String seq = "";

unsigned long current_time;
unsigned long prev_time_client = 0;
unsigned long prev_time_us_sensor = 0;
int us_sensor_delay = 50;

int us_sensor_left();
int us_sensor_right();

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();

  pinMode(echo_left, INPUT);
  pinMode(trig_left, OUTPUT);

  pinMode(echo_right, INPUT);
  pinMode(trig_right, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }

  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  res_distance_left = us_sensor_left();
  res_distance_right = us_sensor_right();
  current_time = millis();
  if (!client.connect(host, port)) {
    lcd.clear();
    Serial.println("Host is offline");
    lcd.setCursor(0, 0);
    lcd.print("Host Offline");
    delay(200);
    return;
  }

  lcd.setCursor(0, 0);
  lcd.print("People Inside:");

  res_distance_left = us_sensor_left();
  res_distance_right = us_sensor_right();
  if ((res_distance_right < trig_distance) && (seq.charAt(0) != '1')) {
    seq += "1";
  }
  if ((res_distance_left < trig_distance) && (seq.charAt(0) != '2')) {
    seq += "2";
  }

  if (seq.equals("12")) {
    if (current_time - prev_time_us_sensor >= us_sensor_delay) {
      people_count++;
      seq = "";
    }
  } else if (seq.equals("21") && people_count > 0) {
    if (current_time - prev_time_us_sensor >= us_sensor_delay) {
      people_count--;
      seq = "";
    }
  }

  if (seq.length() > 2 || seq.equals("11") || seq.equals("22") || timeout_counter > 5) {
    seq = "";
  }

  if (seq.length() == 1) {
    timeout_counter++;
  } else {
    timeout_counter = 0;
  }

  client.print(people_count);
  Serial.println(people_count);
  /*Serial.print(", ");
  Serial.println(res_distance_right);*/
  if (people_count < 10) {
    lcd.setCursor(0, 1);
    lcd.print(people_count);
    lcd.print(" ");
  } else if (people_count >= 10) {
    lcd.setCursor(0, 1);
    lcd.print(people_count);
  }
}
int us_sensor_left() {
  digitalWrite(trig_left, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_left, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_left, LOW);
  duration_left = pulseIn(echo_left, HIGH);
  distance_left = (duration_left * 0.0343 / 2);
  return distance_left;
}

int us_sensor_right() {
  digitalWrite(trig_right, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_right, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_right, LOW);
  duration_right = pulseIn(echo_right, HIGH);
  distance_right = (duration_right * 0.0343 / 2);
  return distance_right;
}
