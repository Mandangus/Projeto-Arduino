
#define PINO_RTS 11
#define PINO_CTS 12
#define PINO_TX 13
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

#define SET_CTS_BIT(b) CTS_BIT = b
#define SET_RTS_BIT(b) RTS_BIT = b

#define SEND_CTS_BIT() digitalWrite(PINO_CTS,CTS_BIT) 
#define RECV_RTS_BIT() (CTS_BIT = digitalRead(PINO_RTS))
#include "Temporizador.h"

#define FRAME_LENGTH 11
int frame[FRAME_LENGTH];
int frame_index = 0;

#define MESSAGE_MAX_LENGTH 128
char message[MESSAGE_MAX_LENGTH];
int message_index = 0;

bool RTS_BIT = false; /* Ready for transmission */
bool CTS_BIT = false; /* Confirm transmission */

void reciveNextFrameBit() {
  digitalRead(PINO_TX, frame[frame_index]);

  Serial.print(frame[frame_index]);

  ++frame_index;

}

// Calcula bit de paridade - Par ou impar
bool bitParidade(char data){
  int parity = 0;

  for (int i = 0; i < 8; i++) {
    parity = parity ^ bitRead(data, i);
  }

  return parity;
}

// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){

  if (!RECV_RTS_BIT()){
    if(CTS_BIT){
      SET_CTS_BIT(false)
      // construir mansagem do frame
      char msg = buildChar()
      //checar paridade
      int parity = bitParidade(msg)
      if(parity){
        Serial.print(msg)
      }
    }
    return;
  }
  SET_CTS_BIT(true);
  SEND_CTS_BIT();


  reciveNextFrameBit();
}

// Executada uma vez quando o Arduino reseta
void setup(){
  //desabilita interrupcoes
  noInterrupts();
  // Configura porta serial (Serial Monitor - Ctrl + Shift + M)
  Serial.begin(9600);

  pinMode(PINO_TX, OUTPUT);
  pinMode(PINO_RTS, INPUT);
  pinMode(PINO_CTS, OUTPUT);

  
  configuraTemporizazdor(BAUD_RATE);

  interrupts();

  iniciaTemporizador();
}

// O loop() eh executado continuamente (como um while(true))
void loop ( ) {
  if(CTS_BIT){}
}
