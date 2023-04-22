void oled_task(void* pvParameters) {
  //display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.begin(OLED_ADDR, true);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Smart Home Security");
  display.display();
  delay(2000);

  bool motion = false;
  bool door = false;

  for(;;)
  {
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Smart Home Security");
    
    display.println("");

    display.print("Motion: ");
    if(motion_detected) { display.println("Detected"); }
    else { display.println("None"); }

    display.print("Door: ");
    if(door_open) { display.println("Open"); }
    else { display.println("Close"); }

    display.print("Lock: ");
    if(statuslock[0]) { display.println("Locked"); }
    else { display.println("Unlocked"); }

    display.print("Security: ");
    if(security) { display.println("On"); }
    else { display.println("Off"); }

    display.println("");
    if(alert) { display.println("!!!!! ALERT !!!!!"); }

    display.display();
    vTaskDelay(pdMS_TO_TICKS(100)); //delay(100);
  }
}