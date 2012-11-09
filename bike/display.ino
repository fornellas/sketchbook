extern Adafruit_PCD8544 display;

void show(float speed, float distance, float temperature, float humidity){
  byte d;
  display.clearDisplay();

  // battery
  display.drawRect(70, 0, 84-70, 6, BLACK);
  display.drawFastVLine(69, 2, 2, BLACK);
  display.drawFastVLine(68, 2, 2, BLACK);
  display.drawRect(72, 2, 84-70-4, 2, BLACK);

  // speed
  display.setTextSize(3);
  if(speed<10.0)
    display.setCursor(18,0);
  else
    display.setCursor(3,0);
  display.println(int(speed));
  display.setTextSize(2);
  display.setCursor(35,7);
  display.println(".");
  display.setCursor(45,7);
  d=floor(10.0*(speed-floor(speed)));
  display.println(d);
  display.setTextSize(1);
  display.setCursor(57,14);
  display.println("Km/h");

  display.drawFastHLine(0, 22, 84, BLACK);

  // distance
  display.setTextSize(2);
  if(distance<10){
    display.setCursor(12,24);
    display.println(distance, 2);
    display.setTextSize(1);
    display.setCursor(61, 31);
  }
  else if(distance<100){
    display.setCursor(6,24);
    display.println(distance, 2);
    display.setTextSize(1);
    display.setCursor(67, 31);
  }
  else{
    display.setCursor(7,24);
    display.println(distance, 1);
    display.setTextSize(1);
    display.setCursor(67, 31);
  }
  display.println("km");

  display.drawFastHLine(0, 39, 84, BLACK);
  /*
  display.setTextSize(1);
   display.setCursor(4,41);
   display.println(temperature, 1);
   display.setCursor(26,36);
   display.println(".");
   display.setCursor(31,41);
   display.println("C");
   
   display.drawFastVLine(42, 40, 8, BLACK);
   
   display.setCursor(54,41);
   display.println(humidity, 0);
   display.setCursor(66,41);
   display.println("%");
   */
   
  // Temperature
  display.setTextSize(1);
  display.setCursor(0,41);
  display.println(temperature, 0);
  display.setCursor(12,36);
  display.println(".");
  display.setCursor(17,41);
  display.println("C");

  display.drawFastVLine(27, 40, 8, BLACK);

  // Humidity
  display.setCursor(32,41);
  display.println(humidity, 0);
  display.setCursor(44,41);
  display.println("%");

  display.drawFastVLine(53, 40, 8, BLACK);

  // time
  display.setCursor(58,41);
  display.println("13");
  display.setCursor(68,41);
  display.println(":");
  display.setCursor(72,41);
  display.println("33");

  display.display();
}





