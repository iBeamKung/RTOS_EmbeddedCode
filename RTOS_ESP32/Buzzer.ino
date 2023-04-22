void alert_task(void* pvParameters) {
  for (;;) {
    if (alert) {
      analogWrite(buzzer_pin, 150);
      vTaskDelay(pdMS_TO_TICKS(500));
      analogWrite(buzzer_pin, 100);
      vTaskDelay(pdMS_TO_TICKS(500));
    } else {
      analogWrite(buzzer_pin, 0);
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}

void beep_task(void* pvParameters) {
  for (;;) {
    uint32_t notification;
    if (xTaskNotifyWait(0, ULONG_MAX, &notification, portMAX_DELAY) == pdTRUE) {
      analogWrite(buzzer_pin, 100);
      vTaskDelay(pdMS_TO_TICKS(500));
      analogWrite(buzzer_pin, 0);
    }
  }
}