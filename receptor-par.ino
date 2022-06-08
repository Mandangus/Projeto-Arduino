
#define PINO_RTS 11
#define PINO_CTS 12
#define PINO_TX 13
#define BAUD_RATE 1
#define HALF_BAUD 1000/(2*BAUD_RATE)

#define SET_CTS_BIT(b) CTS_BIT = b
#define SET_RTS_BIT(b) RTS_BIT = b

#define SEND_CTS_BIT() digitalWrite(PINO_CTS,CTS_BIT) 
#define RECV_RTS_BIT() (RTS_BIT = digitalRead(PINO_RTS))
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
  frame[frame_index]=digitalRead(PINO_TX);

  Serial.print(frame[frame_index]);

  ++frame_index;

  if (frame_index == FRAME_LENGTH){
    nextCharOnMsg();
  }
}

char buildChar(){
  // ler do 1 ao 8
  int data_frame[8];
  for (int i = 0; i < 8; i++){
    data_frame[i] = frame[i+1];
  }
  // converter os bits de dados para char
  char tmp = 0;
  for (int i = 0; i < 8; i++){
    tmp |= data_frame[i] << (8-1-i); //slot each bit in where it belongs 
  }

  return tmp;
}

void nextCharOnMsg(){
  // builda o char a partir do frame
  char c = buildChar();
  // seta o frame index pra 0
  frame_index = 0;
  //checa paridade
  int parity = bitParidade(c);
  if(parity){
    message[message_index++] = c;
  }
}

// Calcula bit de paridade - Par ou impar
bool bitParidade(char data){
  int parity = 0;

  for (int i = 0; i < 8; i++) {
    parity = parity ^ bitRead(data, i);
  }

  return parity;
}
void showMsg(){
  if (message_index < MESSAGE_MAX_LENGTH - 1 ){
    message[++message_index] = '\0';
  }
  Serial.println(message);
  message_index = 0;
}
// Rotina de interrupcao do timer1
// O que fazer toda vez que 1s passou?
ISR(TIMER1_COMPA_vect){

  SEND_CTS_BIT();
  
  if (!RECV_RTS_BIT()){

    // if acabou a transmissão
    if (message_index != 0){
      showMsg();
      SET_CTS_BIT(false);
    }
    return;
  }
  SET_CTS_BIT(true);


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

}
