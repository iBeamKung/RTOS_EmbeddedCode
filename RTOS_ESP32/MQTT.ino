void mqtt_task(void* pvParameters) {
  for (;;) {
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
    

    uint32_t notification;
    if (xTaskNotifyWait(0, ULONG_MAX, &notification, 100) == pdTRUE) {
      if (notification == 1) {
        Serial.println("[MQTT] Motion Sent Payload");
        if(security == true)
        {
          StaticJsonDocument<200> jsonDocument;

          jsonDocument["clientId"] = "ALL";
          jsonDocument["alert"] = "The system detects abnormal movements.";
          char Jsonout[256];
          serializeJson(jsonDocument, Jsonout);        

          mqtt_client.publish("RTOS/esp/alert", Jsonout);
          
        }
      } else if (notification == 2) {

        Serial.println("[MQTT] StatusDoor");

        StaticJsonDocument<200> jsonDocument;
        jsonDocument["status_door"] = door_open;
        char Jsonout[256];
        serializeJson(jsonDocument, Jsonout);        

        mqtt_client.publish("RTOS/esp/status_door", Jsonout);

      } else if (notification == 3) {

        Serial.println("[MQTT] Lock Update");

        StaticJsonDocument<200> jsonDocument;

        jsonDocument["status_lock_1"] = statuslock[0];
        char Jsonout[256];
        serializeJson(jsonDocument, Jsonout);        

        mqtt_client.publish("RTOS/esp/status_lock_1", Jsonout);
      }
    }
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] Message arrived on topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  String topic_str = topic, payload_str = (char*)payload;

  if(topic_str == "RTOS/esp/check_status")
  {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload, length);

    StaticJsonDocument<300> jsonDocument;
    jsonDocument["req_id"] = doc["clientId"];
    jsonDocument["status_lock_1"] = statuslock[0];
    jsonDocument["status_door"] = door_open;
    jsonDocument["security"] = security;

    char Jsonout[256];
    serializeJson(jsonDocument, Jsonout);

    mqtt_client.publish("RTOS/esp/status", Jsonout);
  }
  else if(topic_str == "RTOS/esp/lock_1")
  {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload, length);
    if(doc["lock_1"] == true)
    { 
      if(!door_open) { statuslock[0] = doc["lock_1"]; }
      else {
        StaticJsonDocument<500> jsonDocument;
        jsonDocument["clientId"] = doc["clientId"];
        jsonDocument["alert"] = "The door cannot be locked because the door is open.";
        char Jsonout[256];
        serializeJson(jsonDocument, Jsonout);        

        mqtt_client.publish("RTOS/esp/alert", Jsonout);
      }
    }
    else
    { statuslock[0] = doc["lock_1"]; }

    Serial.print("Status Lock: ");
    Serial.println(statuslock[0]);

    StaticJsonDocument<300> jsonDocument3;
    jsonDocument3["req_id"] = "None";
    jsonDocument3["status_lock_1"] = statuslock[0];
    jsonDocument3["status_door"] = door_open;
    jsonDocument3["security"] = security;
    char Jsonout3[256];
    serializeJson(jsonDocument3, Jsonout3);
    mqtt_client.publish("RTOS/esp/status", Jsonout3);
  }
  else if(topic_str == "RTOS/esp/security")
  {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload, length);
    security = doc["security"];

    if(security==false) {alert=false;}

    StaticJsonDocument<500> jsonDocument;

    jsonDocument["status_security"] = security;
    char Jsonout[256];
    serializeJson(jsonDocument, Jsonout);        

    mqtt_client.publish("RTOS/esp/status_security", Jsonout);

    Serial.print("Security Status: ");
    Serial.println(security);

    StaticJsonDocument<300> jsonDocument2;
    jsonDocument2["req_id"] = "None";
    jsonDocument2["status_lock_1"] = statuslock[0];
    jsonDocument2["status_door"] = door_open;
    jsonDocument2["security"] = security;
    char Jsonout2[256];
    serializeJson(jsonDocument2, Jsonout2);
    mqtt_client.publish("RTOS/esp/status", Jsonout2);
  }
}