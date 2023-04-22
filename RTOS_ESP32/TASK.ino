void motion_detection_task(void* pvParameters) {
  delay(2000);
  for (;;) {
    motion_detected = digitalRead(pir_pin);
    if (motion_detected != temp_motion_detected)
    {
      if (motion_detected) {
        Serial.println("[Motion] Detected");
        xTaskNotify(mqtt_task_handle, 1, eSetValueWithOverwrite);
      }
      temp_motion_detected = motion_detected;
    }
    //Serial.println(digitalRead(pir_pin));
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void door_status_task(void* pvParameters) {
  delay(2000);
  for (;;) {
    bool read_door = !digitalRead(door_pin);
    if(read_door != door_open){
      Serial.println("[DoorSensor] Detected");
      door_open = read_door;
      xTaskNotify(mqtt_task_handle, 2, eSetValueWithOverwrite);
    } 
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void lock_control_task(void* pvParameters) {
  for (;;) {
    Serial.println("[LockSensor] Lock");
    if (digitalRead(locksw1_pin) == false) {
      while(!digitalRead(locksw1_pin)) {}
      if(!door_open) {statuslock[0] = !statuslock[0];}
      else {
        xTaskNotify(beep_task_handle, 1, eSetValueWithOverwrite);
      }
    }

    if(statuslock[0] != temp_statuslock[0]){
      temp_statuslock[0] = statuslock[0];
      xTaskNotify(mqtt_task_handle, 3, eSetValueWithOverwrite);
    } 

    if (statuslock[0] == true) {
      digitalWrite(lock1_pin, HIGH);
    } else {
      digitalWrite(lock1_pin, LOW);
    }
    if (statuslock[1] == true) {
      digitalWrite(lock2_pin, HIGH);
    } else {
      digitalWrite(lock2_pin, LOW);
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //delay(100);
  }
}

void alert_control_task(void* pvParameters) {
  for (;;) {
    if ((statuslock[0] && door_open) || security && motion_detected) {
      alert = true;
    }
    else{
      alert = false;
    }
    vTaskDelay(pdMS_TO_TICKS(100)); //delay(100);
  }
}