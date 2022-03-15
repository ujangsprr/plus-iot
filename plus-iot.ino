#include "Header.h"

void SetID(String ID)
{
  if(WiFi.status()== WL_CONNECTED){
    Serial.println("Set ID Device");
    HTTPClient http;

    String response;

    StaticJsonDocument<200> buff;
    String jsonParams;

    buff["deviceID"] = ID;

    serializeJson(buff, jsonParams);
    Serial.println(jsonParams);

    http.begin(apidev);
    Serial.println(apidev);

    http.addHeader("Content-Type", "application/json");
    
    int statusCode = http.POST(jsonParams);
    Serial.print("Status: ");
    Serial.println(statusCode);

    response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
  }
  else {
    Serial.println("WiFi Disconnected");
  } 
}

void GetData()
{
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    String response;

    http.begin(apiget);
    
    int statusCode = http.GET();
    response = http.getString();

    if(response == "true" && WaterLevel < 100){
      Serial.println("Pompa Nyala");
      digitalWrite(WaterPump, LOW);
      PumpON = 1;
    }
    else if(response == "false"){
      Serial.println("Pompa Mati");
      digitalWrite(WaterPump, HIGH);
      PumpON = 0;
    }

    if(WaterLevel >= 100){
      Serial.println("Pompa Mati");
      digitalWrite(WaterPump, HIGH);
      PumpON = 0;
    }
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void SendData(float level)
{
  if(WiFi.status()== WL_CONNECTED){
    Serial.println("Send to API");
    HTTPClient http;

    String response;

    StaticJsonDocument<200> buff;
    String jsonParams;

    buff["waterLevel"] = level;

    serializeJson(buff, jsonParams);
    Serial.println(jsonParams);

    http.begin(apidata);
    Serial.println(apidata);

    http.addHeader("Content-Type", "application/json");
    
    int statusCode = http.POST(jsonParams);
    Serial.print("Status: ");
    Serial.println(statusCode);

    response = http.getString();
    Serial.print("Response: ");
    Serial.println(response);
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}

void ReadSensor(int pin)
{
  SensorValue = analogRead(pin);
  WaterLevel = map(SensorValue, 0, 3800, 0, 100);
}

void LCDScreen(float level, int condition)
{
  char buffSensor[20];
  char buffPump[20];

  sprintf(buffSensor, "Water : %d%%", int(level));

  if(condition == 1){
    sprintf(buffPump, "Pump  : ON");
  }
  else{
    sprintf(buffPump, "Pump  : OFF");
  }

  if(count == 0){
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print("  PLUS IOT NET  ");
    lcd.setCursor(0, 1); lcd.print("   MONITORING   ");
  }
  else if(count == 1){
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(buffSensor);
    lcd.setCursor(0, 1); lcd.print(buffPump);
  }
  else if(count == 2){
    lcd.clear();
    lcd.setCursor(0, 0); lcd.print(WiFi.SSID());
    lcd.setCursor(0, 1); lcd.print(WiFi.localIP());
  }

  if(count == 2){
    count = 0;
  }
  else{
    count++;
  }
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  
  strftime(secTime,10, "%S", &timeinfo); 
}

void setup()
{
  Serial.begin(115200);

  pinMode(Button, INPUT_PULLUP);
  pinMode(WaterSensor, INPUT); 
  pinMode(WaterPump, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  lcd.init();                 
  lcd.backlight();

  lcd.setCursor(0, 0); lcd.print("== WELCOME TO ==");
  lcd.setCursor(0, 1); lcd.print("==  PLUS IOT  ==");
  delay(1000);
  lcd.clear();

  lcd.setCursor(0, 0); lcd.print("CONNECTING");
  lcd.setCursor(0, 1); lcd.print("DEVICE TO WIFI");
  digitalWrite(LED_BLUE, HIGH);

  WiFi.mode(WIFI_STA);
  delay(1000);

  bool res;
  res = wm.autoConnect("PLUSNET","12345678");
  
  if(!res) {
       Serial.println("Failed to connect");
  } 
  else {  
      Serial.print("Connected");
  }

  delay(500);
  SetID(ID);

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();
  
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("DEVICE CONNECTED");
  lcd.setCursor(0, 1); lcd.print("SSID: "); lcd.print(WiFi.SSID());
  
  digitalWrite(LED_BLUE, LOW);
  delay(500);
  digitalWrite(LED_BLUE, HIGH);
  delay(200);
  digitalWrite(LED_BLUE, LOW);
  delay(200);
  digitalWrite(LED_BLUE, HIGH);
  delay(200);
  digitalWrite(LED_BLUE, LOW);

  delay(2000);
}

void loop()
{
  printLocalTime();
  ReadSensor(WaterSensor);

  if(strcmp(secTime, "00") == 0){
    digitalWrite(LED_BLUE, HIGH);
    SendData(WaterLevel);
    digitalWrite(LED_BLUE, LOW);
  }

  currentMillis = millis();
  if (currentMillis - previousMillisLCD >= 2000) {
    previousMillisLCD = currentMillis;
    LCDScreen(WaterLevel, PumpON);
  }

  currentMillis = millis();
  if (currentMillis - previousMillisGET >= 200) {
    previousMillisGET = currentMillis;
    GetData();
  }

  int reading = digitalRead(Button);
  if(reading == LOW){digitalWrite(LED_BLUE, HIGH);}
  else{digitalWrite(LED_BLUE, LOW);}
  
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        lcd.clear();
        lcd.setCursor(0, 0); lcd.print("RESTARTING");
        digitalWrite(LED_BLUE, LOW);
        for(int i=0; i<3; i++){
          delay(1000);
          lcd.print(" .");
        }
        wm.resetSettings();
        ESP.restart();
      }
    }
  }
  lastButtonState = reading;
}
