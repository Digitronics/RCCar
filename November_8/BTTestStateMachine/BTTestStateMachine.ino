#include <SoftwareSerial.h>//include library code
#define PAIR_BUTTON   3
#define STATE_IDLE    0
#define STATE_PAIR    1
#define BT_EN         4 //Bluetooth enable pin for AT access
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
  pinMode(BT_EN, OUTPUT);
  state =  STATE_IDLE;
  digitalWrite(BT_EN, HIGH);
  
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
          Serial.print("Entering pairing mode\n");
        }
      }
      else
      {
        buttonDebounce = 0; // reset the debounce counter if the button is not fully pressed
      }
      break;
    case STATE_PAIR:
      bool pairStatus = checkBTConnection(); // Check the BT connection (NEED TO IMPLEMENT THIS)
      //If the button is released and BT module is disconnected, disable bluetooth pairing and go back to IDLE state
      if(!pairStatus && digitalRead(PAIR_BUTTON))
      {
        //Return to the IDLE state
        disconnectBTModule();
        state = STATE_IDLE;
        Serial.write("Pairing disabled, entering Idle Mode\n");
      }
      //If the module has paired succesfully, continue to the next state
      if(pairStatus)
      {
        state = STATE_CONNECTED;
        Serial.write("Bluetooth Paired\n");
      }
      break;
    case STATE_CONNECTED:
      if(BTserial.available())
      {
        Serial.write(BTserial.read());
      }

}
