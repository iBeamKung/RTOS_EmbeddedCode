#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
//#include <Adafruit_SSD1306.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>
#include <ArduinoJson.h>

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST -1
#define OLED_ADDR 0x3C

// Wi-Fi Settings
const char* ssid = "iBeamHome_2.4G";
const char* password = "0877911507";
const char* mqtt_server = "netipat.dev";

// MQTT Settings
const char* client_id = "ESP32";

WiFiClient wifi_client;
PubSubClient mqtt_client(wifi_client);

// OLED Display Settings
//Adafruit_SSD1306 display(128, 64, &Wire, OLED_RST);
Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, &Wire, OLED_RST);

// Sensor and Actuator Pins
const int pir_pin = 16;
const int door_pin = 34;
const int buzzer_pin = 12;

// Lock Control Pins
const int locksw1_pin = 4;
const int lock1_pin = 25;
const int lock2_pin = 33;
bool statuslock[2] = { false, false };
bool temp_statuslock[2] = { false, false };

// Sensor and Actuator States
bool motion_detected = false;
bool temp_motion_detected = false;
bool door_open = false;

bool alert = false;

bool security = false;

// FreeRTOS Task Handles
TaskHandle_t motion_detection_task_handle = NULL;
TaskHandle_t door_status_task_handle = NULL;
TaskHandle_t lock_control_task_handle = NULL;
TaskHandle_t mqtt_task_handle = NULL;
TaskHandle_t oled_task_handle = NULL;
TaskHandle_t beep_task_handle = NULL;
TaskHandle_t alert_task_handle = NULL;
TaskHandle_t alert_control_task_handle = NULL;

// FreeRTOS Task Priorities
#define DOOR_STATUS_TASK_PRIORITY 1
#define LOCK_CONTROL_TASK_PRIORITY 2
#define MOTION_DETECTION_TASK_PRIORITY 3
#define MQTT_TASK_PRIORITY 4
#define ALERT_CONTROL_TASK_PRIORITY 5
#define OLED_TASK_PRIORITY 6
#define BEEP_TASK_PRIORITY 7
#define ALERT_TASK_PRIORITY 8

void setup() {
  pinMode(pir_pin, INPUT);
  pinMode(door_pin, INPUT);
  pinMode(buzzer_pin, OUTPUT);
  pinMode(lock1_pin, OUTPUT);
  pinMode(lock2_pin, OUTPUT);

  pinMode(4, INPUT_PULLUP);

  Serial.begin(115200);

  Serial.println("");
  Serial.println("==============================");
  Serial.println("WiFi - Setting");
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);
  Serial.println("==============================");
  Serial.print("Connecting Wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("==============================");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("==============================");

  Serial.println("");
  Serial.println("==============================");
  Serial.println("MQTT - Setting");
  Serial.print("Server: ");
  Serial.println(mqtt_server);
  Serial.println("==============================");
  mqtt_client.setServer(mqtt_server, 1883);
  mqtt_client.setCallback(mqtt_callback);
  while (!mqtt_client.connected()) {
    Serial.println("[MQTT] Not Connected");
    if (!mqtt_client.connected()) {
      Serial.println("[MQTT] Connecting...");
      if (mqtt_client.connect(client_id)) {
        Serial.println("[MQTT] Connected");
        
        mqtt_client.subscribe("RTOS/esp/check_status");
        mqtt_client.subscribe("RTOS/esp/lock_1");
        mqtt_client.subscribe("RTOS/esp/security");
      }
    }
  }
  mqtt_client.loop();

  Serial.println("[RTOS] Taskinit");

  xTaskCreate(mqtt_task, "mqtt_task", 4096, NULL, MQTT_TASK_PRIORITY, &mqtt_task_handle);
  xTaskCreate(motion_detection_task, "motion_detection_task", 4096, NULL, MOTION_DETECTION_TASK_PRIORITY, &motion_detection_task_handle);
  xTaskCreate(door_status_task, "door_status_task", 4096, NULL, DOOR_STATUS_TASK_PRIORITY, &door_status_task_handle);
  xTaskCreate(lock_control_task, "lock_control_task", 4096, NULL, LOCK_CONTROL_TASK_PRIORITY, &lock_control_task_handle);
  xTaskCreate(oled_task, "oled_task", 4096, NULL, OLED_TASK_PRIORITY, &oled_task_handle);
  xTaskCreate(beep_task, "beep_task", 4096, NULL, BEEP_TASK_PRIORITY, &beep_task_handle);
  xTaskCreate(alert_task, "alert_task", 4096, NULL, ALERT_TASK_PRIORITY, &alert_task_handle);
  xTaskCreate(alert_control_task, "alert_control_task", 4096, NULL, ALERT_CONTROL_TASK_PRIORITY, &alert_control_task_handle);

  Serial.println("[RTOS] TaskRunning...");
}

void loop() {
}
