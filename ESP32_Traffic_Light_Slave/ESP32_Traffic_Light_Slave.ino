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

  ----------This below given is ESP 32 SLAVE CODE----------------------
  This is a traffic lighting system that works based on the instruction
  of the master ESP32
*/

// constants won't change. They're used here to set pin numbers:
const int RED = 27;    // the number of the LED pin
const int YELLOW = 26;
const int GREEN = 25;

// Define TX and RX pins for UART (change if needed)
#define TXD1 19
#define RXD1 21

// Use Serial1 for UART communication
HardwareSerial mySerial(2);

// variables will change:
unsigned long startTime = 0;
unsigned long currentTime = 0;
String prevState = "";

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, RXD1, TXD1);  // UART setup
  
  Serial.println("ESP32 UART Receiver (SLAVE)");

  // initialize the LED pin as an output:
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
}
//RED YELLOW GREEN
void light(bool r = 0, bool y = 0, bool g =0)
{
  digitalWrite(RED, r);
  digitalWrite(YELLOW, y);
  digitalWrite(GREEN, g);
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
void trafficLightSlave(String message)
{
  
    if(message == "RED_1")
    {
      //YELLOW ON
      light(0,1,0); //R Y G
    }
    else if(message == "RED_2")
    {
      //GREEN ON
      light(0,0,1);
    }
    else if(message == "RED_3" || message == "GREEN" || message == "YELLOW")
    {
      //RED ON
      light(1,0,0);
    }
    else    //If some issue in traffic signal, default to YELLOW SIGNAL
    {
      //YELLOW ON
      light(0,1,0); //R Y G
    }
}
//Log Printer to Serial Monitor
void log(String message)
{
  if(message == "RED_2" && message != prevState)
  {
    Serial.println("...Pedestrian Crossing...");
  }
  else if(message == "RED_1" && prevState == "YELLOW")
  {
    Serial.println("...Please Wait...");
  }
  else if((message == "GREEN" && message != prevState) || (message == "RED_1" && prevState == "RED_2"))
  {
    Serial.println("...Do Not Cross...");
  }
  prevState = message;
}

void loop() {
  String message = "";
  if (mySerial.available()) {
    // Read data
    message = mySerial.readStringUntil('\n');
  }
  trafficLightSlave(message);
  log(message);
}