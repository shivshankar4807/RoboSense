#define BLYNK_TEMPLATE_ID "TMPL3ehHcWdqn"
#define BLYNK_TEMPLATE_NAME "smart all monitoring"
#define BLYNK_AUTH_TOKEN "************************"   // token hidden in report
 
#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
 
char ssid[] = "Shivshankar";
char pass[] = "********";
 
#define MQ2_PIN 32
#define MQ3_PIN 33
#define MQ4_PIN 34
#define MQ7_PIN 35
#define MQ8_PIN 36
#define MQ9_PIN 39
#define LED_PIN 22
 
const int GAS_THRESHOLD = 2800;   // 0-4095, ESP32 12-bit ADC
 
BlynkTimer timer;
unsigned long lastAlertTime = 0;
const unsigned long alertInterval = 60000; // 60 s between alerts
 
void checkDangerAndAlert(bool isDanger, String alertMsg) {
  if (isDanger && (millis() - lastAlertTime > alertInterval)) {
    if (Blynk.connected()) {
      Blynk.logEvent("danger_alert", alertMsg);
    }
    lastAlertTime = millis();
  }
}
 
void processAndSendSensorData() {
  int v_mq2 = analogRead(MQ2_PIN); delay(5);
  int v_mq3 = analogRead(MQ3_PIN); delay(5);
  int v_mq4 = analogRead(MQ4_PIN); delay(5);
  int v_mq7 = analogRead(MQ7_PIN); delay(5);
  int v_mq8 = analogRead(MQ8_PIN); delay(5);
  int v_mq9 = analogRead(MQ9_PIN); delay(5);
 
  bool gasDanger = (v_mq2 > GAS_THRESHOLD || v_mq3 > GAS_THRESHOLD ||
                    v_mq4 > GAS_THRESHOLD || v_mq7 > GAS_THRESHOLD ||
                    v_mq8 > GAS_THRESHOLD || v_mq9 > GAS_THRESHOLD);
 
  if (gasDanger) {
    digitalWrite(LED_PIN, LOW);
    checkDangerAndAlert(true, "Alert: High Gas Level Detected across MQ Sensors!");
  } else {
    digitalWrite(LED_PIN, HIGH);
  }
 
  if (Blynk.connected()) {
    Blynk.virtualWrite(V2, v_mq2);
    Blynk.virtualWrite(V3, v_mq3);
    Blynk.virtualWrite(V4, v_mq4);
    Blynk.virtualWrite(V5, v_mq7);
    Blynk.virtualWrite(V6, v_mq8);
    Blynk.virtualWrite(V7, v_mq9);
  }
}
 
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
 
  WiFi.begin(ssid, pass);
  int wifiAttempt = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempt < 20) {
    delay(500);
    wifiAttempt++;
  }
 
  Blynk.config(BLYNK_AUTH_TOKEN);
  timer.setInterval(4000L, processAndSendSensorData); // every 4 s
}
 
void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    if (!Blynk.connected()) Blynk.connect(3000);
    Blynk.run();
  }
  timer.run();
}
