int Center_ldr = A0;  
int North_ldr = A1;   
int East_ldr = A2;    
int South_ldr = A3;   
int West_ldr = A4;    

int CenterLED = 2;    
int NorthBlue = 3;    
int NorthWhite = 4;   
int EastBlue = 5;     
int EastWhite = 6;    
int SouthBlue = 7;    
int SouthWhite = 8;   
int WestBlue = 9;     
int WestWhite = 10;   

int buttonPin = 11;   
int potPin = A5; 
int THRESHOLD = 50;

int mode = 0;          
int luxReference = 600; 
int lastButtonState = HIGH;
int avgLux = 0;

int calculateAverage(int ldrValues[]);

void intensityIndicator(int ldrValues[]);  // Declare the function here
void defaultMode(int ldrValues[]); 
void Alarm(int ldrValues[]); 
void blinkCenterLED();
void controlDirectionalLEDs(int ldrValues[]);
void controlCenterLED(int value);
void controlNorthLED(int value);
void controlEastLED(int value);
void controlSouthLED(int value);
void controlWestLED(int value);
void logLuxData(float luxValues[]);
void logData(int ldrValues[]);
void turnOffAllLEDs();
void turnOnAllLEDs();
float analogToLux(int analogValue);

void setup() {
  pinMode(CenterLED, OUTPUT);
  pinMode(NorthBlue, OUTPUT);
  pinMode(NorthWhite, OUTPUT);
  pinMode(EastBlue, OUTPUT);
  pinMode(EastWhite, OUTPUT);
  pinMode(SouthBlue, OUTPUT);
  pinMode(SouthWhite, OUTPUT);
  pinMode(WestBlue, OUTPUT);
  pinMode(WestWhite, OUTPUT);
  Serial.begin(9600); 
}

void loop() {
  int ldrValues[5] = {
    analogRead(Center_ldr), 
    analogRead(North_ldr), 
    analogRead(East_ldr), 
    analogRead(South_ldr), 
    analogRead(West_ldr)
  };

  luxReference = map(analogRead(potPin), 0, 1023, 300, 800);
  avgLux = calculateAverage(ldrValues);

  float luxValues[5];
  for (int i = 0; i < 5; i++) {
    luxValues[i] = analogToLux(ldrValues[i]);
  }

  logLuxData(luxValues);

  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW && lastButtonState == HIGH) {
    mode = (mode + 1) % 3; // Cycle through modes
    delay(200); // Debounce
  }
  lastButtonState = buttonState;

  switch (mode) {
    case 0:
      defaultMode(ldrValues);
      break;
    case 1:
      intensityIndicator(ldrValues);  
      break;
    case 2:
      Alarm(ldrValues);
      break;
  }
  logData(ldrValues);
  delay(500); 
}

float analogToLux(int analogValue) {
  // Map analog value (0–1023) to lux range (50–600)
  return map(analogValue, 0, 1023, 0, 600);
}

void logLuxData(float luxValues[]) {
  Serial.println("LUX VALUES:");
  Serial.print("Center: "); Serial.print(luxValues[0], 2);
  Serial.print(" | North: "); Serial.print(luxValues[1], 2);
  Serial.print(" | East: "); Serial.print(luxValues[2], 2);
  Serial.print(" | South: "); Serial.print(luxValues[3], 2);
  Serial.print(" | West: "); Serial.println(luxValues[4], 2);
  Serial.print("LUX REF: ");
  Serial.println(luxReference);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void defaultMode(int ldrValues[]) {
  bool allSimilar = true;
  int referenceValue = ldrValues[0];
  
  for (int i = 1; i < 5; i++) {
    if (abs(ldrValues[i] - referenceValue) > THRESHOLD) {
      allSimilar = false;
      break;
    }
  }

  if (allSimilar) {   
    blinkCenterLED(); 
    turnOffAllLEDs();
  } else {
    controlDirectionalLEDs(ldrValues); 
  }
  delay(1000);
}

void blinkCenterLED() {
  for (int i = 0; i < 3; i++) { 
    digitalWrite(CenterLED, HIGH);
    delay(500); 
    digitalWrite(CenterLED, LOW);
    delay(500); 
  }
}

void controlDirectionalLEDs(int ldrValues[]) {
  // Turn off all LEDs initially
  turnOffAllLEDs();

  controlCenterLED(ldrValues[0]);
  controlNorthLED(ldrValues[1]);
  controlEastLED(ldrValues[2]);
  controlSouthLED(ldrValues[3]);
  controlWestLED(ldrValues[4]);
}

void controlCenterLED(int value) {
  digitalWrite(CenterLED, value >= 950 ? HIGH : LOW); 
  if(value>=850){
    blinkCenterLED();
  }
}

void controlNorthLED(int value) {
  digitalWrite(NorthBlue, value >= 850 ? HIGH : LOW);
  digitalWrite(NorthWhite, value >= 900 ? HIGH : LOW);
}

void controlEastLED(int value) {
  digitalWrite(EastBlue, value >= 850 ? HIGH : LOW);
  digitalWrite(EastWhite, value >= 900 ? HIGH : LOW);
}

void controlSouthLED(int value) {
  digitalWrite(SouthBlue, value >= 850 ? HIGH : LOW);
  digitalWrite(SouthWhite, value >= 900 ? HIGH : LOW);
}

void controlWestLED(int value) {
  digitalWrite(WestBlue, value >= 790 ? HIGH : LOW);
  digitalWrite(WestWhite, value >= 750 ? HIGH : LOW);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void intensityIndicator(int ldrValues[]) {
  turnOffAllLEDs(); // Ensure all LEDs are off at the start
  
  int ledPins[5] = {EastWhite, EastBlue, CenterLED, WestBlue, WestWhite};
  int level = 0;

  if (avgLux > 550 && avgLux <= 650) level = 1;
  else if (avgLux > 650 && avgLux <= 750) level = 2;
  else if (avgLux > 750 && avgLux <= 850) level = 3;
  else if (avgLux > 850 && avgLux <= 950) level = 4;
  else if (avgLux > 950) level = 5;

  for (int i = 0; i < level; i++) {
    digitalWrite(ledPins[i], HIGH);
  }
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void Alarm(int ldrValues[]) {
  turnOffAllLEDs();
  
  if (avgLux > luxReference) {
    Serial.println("ALARM ACTIVATED");
    for (int i = 0; i < 3; i++) { // Blink all LEDs
      turnOnAllLEDs();
      delay(200);
      turnOffAllLEDs();
      delay(200);
    }
  }
}

void turnOffAllLEDs() {
  digitalWrite(CenterLED, LOW);
  digitalWrite(NorthBlue, LOW);
  digitalWrite(NorthWhite, LOW);
  digitalWrite(EastBlue, LOW);
  digitalWrite(EastWhite, LOW);
  digitalWrite(SouthBlue, LOW);
  digitalWrite(SouthWhite, LOW);
  digitalWrite(WestBlue, LOW);
  digitalWrite(WestWhite, LOW);
}

void turnOnAllLEDs() {
  digitalWrite(CenterLED, HIGH);
  digitalWrite(NorthBlue, HIGH);
  digitalWrite(NorthWhite, HIGH);
  digitalWrite(EastBlue, HIGH);
  digitalWrite(EastWhite, HIGH);
  digitalWrite(SouthBlue, HIGH);
  digitalWrite(SouthWhite, HIGH);
  digitalWrite(WestBlue, HIGH);
  digitalWrite(WestWhite, HIGH);
}

int calculateAverage(int ldrValues[]) {
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += ldrValues[i];
  }
  return sum / 5;
}

void logData(int ldrValues[]) {
  Serial.print("MODE: ");
  Serial.print(mode == 0 ? "Default" : mode == 1 ? "Intensity Indicator" : "Alarm");
  Serial.println(" | LDR Values:");
  Serial.print("Center: "); Serial.print(ldrValues[0]);
  Serial.print(" | North: "); Serial.print(ldrValues[1]);
  Serial.print(" | East: "); Serial.print(ldrValues[2]);
  Serial.print(" | South: "); Serial.print(ldrValues[3]);
  Serial.print(" | West: "); Serial.println(ldrValues[4]);
  Serial.print("LUX_REF: ");
  Serial.print(luxReference);
  Serial.print(" | AVG_LUX: ");
  Serial.println(avgLux);
}
