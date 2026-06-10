/*
  created 02 June 2026
  by Sooraj D S

  SLAVE ESP32:
  The Idea here is that this code represents the signal for pedestrians
  i.e, if they can cross the road or not,

  MASTER ESP32:
  The Master Node controls the main traffic light of vehicles which are moving
  When the user wants to cross the road, he/she can press the button(saved in MASTER code) and wait
  until there signal becomes GREEN(done by slave) at same time the signal for the moving traffic
  becomes RED(done by master)

  ----------This below given is ESP 32 MASTER CODE----------------------
  This is a traffic lighting system that works independently and control
  the slave ESP32
*/

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 33;  // the number of the pushbutton pin
const int RED = 27;    // the number of the LED pin
const int YELLOW = 26;
const int GREEN = 25;
// Define TX and RX pins for UART (change if needed)
#define TXD1 19
#define RXD1 21

// Use Serial1 for UART communication, creating an instance mySerial();
HardwareSerial mySerial(1);

// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status
int ledState = HIGH;        // the current state of the output pin
int lastButtonState = LOW;  // the previous reading from the input pin
unsigned long startTime = 0;
unsigned long currentTime = 0;
int sequenceRunning = 0;  //true or false

// the following variables are unsigned longs because the time, measuRED in
// milliseconds, will quickly become a bigger number than can be stoRED in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
unsigned long lightDelay = 2000;    // Time between each light change 2 seconds

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, RXD1, TXD1);  // UART setup 
  Serial.println("ESP32 UART Transmitter (MASTER)");
  // initialize the LED pin as an output:
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
}

void buttonDebounce(int reading)
{
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
    }
  }
}
/*---------Available MESSAGES-----------------/
|       Master     | Slave  |
|------------------|--------|
| GREEN (No Timer) | RED    |
| YELLOW (t<2000)  | RED    |

| RED_1  (t<4000)  | YELLOW |
| RED_2  (t<6000)  | GREEN  |
| RED_1  (t<8000)  | YELLOW |
| RED_3  (t<10000) | RED    |

| YELLOW (t<12000) | RED    |
| GREEN  (No Timer)| RED    |
*/
void trafficLightMaster(String command)
{
  //First two condition checks if buttonState changed and sequenceRunning == 0 ensures the timer doesnt reset every time the button is pressed
  //command == "STOP" can be used to override the button press logic
  if((buttonState == HIGH && lastButtonState == LOW && sequenceRunning == 0) || command == "STOP") 
  {
      startTime = millis();
      sequenceRunning = 1;
  }
  // If the buttonState is changed sequenceRunning will be 1
  if (sequenceRunning == 0) {
    //GREEN ON
    digitalWrite(RED, LOW);
    digitalWrite(YELLOW, LOW);
    digitalWrite(GREEN, HIGH);
    mySerial.println(String("GREEN"));

  }
  else if (sequenceRunning == 1)
  {
    currentTime = millis();   //updating the current time
    if( currentTime-startTime < 2000){
      //YELLOW ON
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(GREEN, LOW);
      mySerial.println(String("YELLOW"));
    }
    else if( currentTime-startTime < 4000){
      //RED ON
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, LOW);
      mySerial.println(String("RED_1"));
    }
    else if( currentTime-startTime < 6000){
      //RED ON
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, LOW);
      mySerial.println(String("RED_2"));
    }
    else if( currentTime-startTime < 8000){
      //RED ON
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, LOW);
      mySerial.println(String("RED_1"));
    }
    else if( currentTime-startTime < 10000){
      //RED ON
      digitalWrite(RED, HIGH);
      digitalWrite(YELLOW, LOW);
      digitalWrite(GREEN, LOW);
      mySerial.println(String("RED_3"));
    }
    else if( currentTime-startTime < 12000){
      //YELLOW ON
      digitalWrite(RED, LOW);
      digitalWrite(YELLOW, HIGH);
      digitalWrite(GREEN, LOW);
      mySerial.println(String("YELLOW"));
    }
    else
    {
      //reset all the values to default value to show green signal
      startTime = currentTime;
      sequenceRunning = 0;
    }
  }
}

void loop() {
  // read the state of the pushbutton value:
  int reading = digitalRead(buttonPin);
  String command = "";
  //The second condition make sure the the serial reading doesnt interupt or restart the signal if it is already RED
  if (Serial.available() > 0 && sequenceRunning == 0) {
    command = Serial.readString(); 
    command.trim();
  }
  buttonDebounce(reading);
  trafficLightMaster(command);  
  lastButtonState = reading;
}