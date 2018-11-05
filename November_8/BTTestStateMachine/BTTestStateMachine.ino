#include <SoftwareSerial.h>//include library code
#define PAIR_BUTTON   7
#define STATE_IDLE    0
#define STATE_PAIR    1
#define STATE_CONNECTED    2
#define BT_ST         5 //Bluetooth State pin for connection state
SoftwareSerial BTserial(2, 3); // ArduinoRX | ArduinoTX
// Connect the AT-09 BLE TX to the Arduino RX on pin 2. 
// Connect the AT-09 BLE RX to the Arduino TX on pin 3
//*********************************************************************************

unsigned int state;
unsigned int buttonDebounce = 0;
unsigned int buttonDebounceLimit = 10;
void setup() {
  Serial.begin(9600); // Initializes USB Serial
  BTserial.begin(9600); //Initailizes Bluetooth UART
  pinMode(PAIR_BUTTON, INPUT_PULLUP);
  pinMode(BT_ST, INPUT);
  state =  STATE_IDLE;
  //Change the role and put the module into sleep mode.
  disconnectBTModule();
}


void loop() {
  //State machine
  switch(state)
  {
    case STATE_IDLE: 
      //If the pair button is pressed, enable the bluetooth module
      if(!digitalRead(PAIR_BUTTON))
      {
        buttonDebounce++;
        if(buttonDebounce >= buttonDebounceLimit)
        {
          state = STATE_PAIR; //Once the button has been debounced, move to pairing mode
          bTModulePair(); // Set the BT module to advertise / discovery mode 
          buttonDebounce = 0;
          Serial.println("Entering pairing mode");
        }
      }
      else
      {
        buttonDebounce = 0; // reset the debounce counter if the button is not fully pressed
      }
      break;
      
    case STATE_PAIR:
      //If the button is released and BT module is disconnected, disable bluetooth pairing and go back to IDLE state
      if(!digitalRead(BT_ST) && digitalRead(PAIR_BUTTON)) // if state is low and button is high
      {
        //Return to the IDLE state
        disconnectBTModule();
        state = STATE_IDLE;
        Serial.write("Pairing disabled, entering Idle Mode\n");
      }
      //If the module has paired succesfully, continue to the next state
      if(digitalRead(BT_ST))
      {
        state = STATE_CONNECTED;
        Serial.write("Bluetooth Paired\n");
        //Flush the Bluetooth serial so that it doesn't print all the accumulated wake+sleep commands
        while(BTserial.available()){
          BTserial.read();
        }
      }
      break;
      
    case STATE_CONNECTED:
      if(BTserial.available())
      {
        Serial.write(BTserial.read());
      }
      if (Serial.available())
      {    
        BTserial.write(Serial.read());    
      }

  }
}



void bTModulePair() {
  //send a string of 80+ characters to wake up the module
  BTserial.println("I am Ironman I am Ironman I am Ironman I am Ironman I am Ironman I am Ironman I am Ironman");// Set the BT module to advertise / discovery mode 
  delay(100);
  BTserial.println("AT+ROLE0");
  delay(800);//give time for the module to switch roles
}

void disconnectBTModule(){
   BTserial.println("AT+ROLE1");
    delay(800);// Allows time for the module to switch roles so that it can register the next command.
    BTserial.println("AT+SLEEP");
}
