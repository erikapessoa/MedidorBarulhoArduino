#include <LedControl.h>
#include <MeetAndroid.h>


MeetAndroid meetAndroid;


LedControl ledControl = LedControl(12,10,11,1);

int sSensorAnalPin = A15; //porta analógica ligado ao sensor de som
int sSensorDigitalPin = 3; //porta analógica ligado ao sensor de som
int sSensorActualValue = 0;  //variável que guarda o valor que o sensor de som analógico esta captando
int sSensorActualValueAndroid = 0;  //variável que guarda o valor que o sensor de som analógico esta captando convertido para Android
int sSensorLimit = 0; //valor que define o limite de barulho que pode ser feito no ambiente
//int sSensorTimeMesuring = 0; //indica o tempo que o sensor de som irá ficar medindo o som ambiente
//int androidValues[] = {0,0}; //o primeiro valor é o limite e o segundo o tempo de medição
//int sSensorCalibration = 0; //valor que define a calibração do microfone

const int VALOR_MIN_ARDUINO = 530;
const int VALOR_MAX_ARDUINO = 570;
const int VALOR_MIN_ANDROID = 0;
const int VALOR_MAX_ANDROID = 100;


// carinha feliz
byte happySmile[8]= {B00111100,B01000010,B10100101,B10000001,B10100101,B10011001,B01000010,B00111100};
// carinha neutra
byte neutralSmile[8]={B00111100, B01000010,B10100101,B10000001,B10111101,B10000001,B01000010,B00111100};
// carinha triste
byte sadSmile[8]= {B00111100,B01000010,B10100101,B10000001,B10011001,B10100101,B01000010,B00111100};



bool MODE_MESURING = false;

const int BELLOW_VALUE = 0;
const int HALF_VALUE = 1;
const int EXCEEDED_VALUE = 2;

void setup() {
  
  Serial.begin(9600);

  /*
   The MAX7219 is in power-saving mode on startup, we have to do a wakeup call
   */
  ledControl.shutdown(0,false);
  /* Set the brightness to a medium values */
  ledControl.setIntensity(0,8);
  /* and clear the display */
  ledControl.clearDisplay(0);

  //Registrando a função que será chamada pelo Android para iniciar a medição.
  meetAndroid.registerFunction(iniciarMedicao, 'I');
  //Registrando a função que será chamada pelo Android para parar a medição antes do tempo acabar.
  meetAndroid.registerFunction(pararMedicao, 'P');

  pinMode(sSensorDigitalPin,INPUT);
  
}

void loop(){
  meetAndroid.receive();

  if(MODE_MESURING) {
    mesuringAmbientSound();
  }
  
}

void iniciarMedicao() {

  /*
   The MAX7219 is in power-saving mode on startup, we have to do a wakeup call
   */
  ledControl.shutdown(0,false);
  /* Set the brightness to a medium values */
  ledControl.setIntensity(0,8);
  /* and clear the display */
  ledControl.clearDisplay(0);

  //inicializando as variáveis. Importante para toda vez que começar uma nova medição.
  sSensorActualValue = 0;
  //sSensorCalibration = 0; 

  inicializarLedsArduino();

  sSensorLimit = meetAndroid.getInt();  //valor que vem do app android

  MODE_MESURING = true;

}


void mesuringAmbientSound() {

  sSensorActualValue = analogRead(sSensorAnalPin);  

  if (sSensorActualValue < VALOR_MIN_ARDUINO)
      sSensorActualValue = VALOR_MIN_ANDROID;
  else if (sSensorActualValue >= VALOR_MIN_ARDUINO) // && sSensorActualValue <= VALOR_MAX_ARDUINO)
      sSensorActualValue = sSensorActualValue - VALOR_MIN_ARDUINO;
  //else
    //  sSensorActualValue = VALOR_MAX_ANDROID;
  
  verificaLimite();
  delay(2000); //verificar o som a cada 5 segundos

}

void verificaLimite() {

  int metade = sSensorLimit / 2;

  sSensorActualValueAndroid = (VALOR_MAX_ANDROID * sSensorActualValue) / (VALOR_MAX_ARDUINO - VALOR_MIN_ARDUINO) ;
  
  if(sSensorActualValueAndroid < metade) {
    //chama função que alerta no Arduino
    alterarLedsArduino(BELLOW_VALUE);
  } else if(sSensorActualValueAndroid >= metade && sSensorActualValueAndroid < sSensorLimit) {
    //chama função que alerta no Arduino
    alterarLedsArduino(HALF_VALUE);
  } else if (sSensorActualValueAndroid >= sSensorLimit) {
    //chama função que alerta no Arduino
    alterarLedsArduino(EXCEEDED_VALUE);
  }

  meetAndroid.send(sSensorActualValueAndroid); //envia para o android a leitura atual do sensor

}


void pararMedicao () {

  MODE_MESURING = false;

  /* limpa o display */
  ledControl.clearDisplay(0);
  ledControl.shutdown(0,true);
  
}

void inicializarLedsArduino () {
  desenharSorrisoFeliz();
  
}

void alterarLedsArduino (int valor) {
  if (valor == BELLOW_VALUE) {//abaixo da metade do valor
    desenharSorrisoFeliz();
  } if(valor == HALF_VALUE) { //chegou na metade do valor
    desenharSorrisoNeutro();
  } else if(valor == EXCEEDED_VALUE) { //passou do valor
    desenharSorrisoTriste();
  }
}


void desenharSorrisoFeliz() {
  ledControl.setRow(0,0,happySmile[0]);
  ledControl.setRow(0,1,happySmile[1]);
  ledControl.setRow(0,2,happySmile[2]);
  ledControl.setRow(0,3,happySmile[3]);
  ledControl.setRow(0,4,happySmile[4]);
  ledControl.setRow(0,5,happySmile[5]);
  ledControl.setRow(0,6,happySmile[6]);
  ledControl.setRow(0,7,happySmile[7]);
}

void desenharSorrisoNeutro() {
  ledControl.setRow(0,0,neutralSmile[0]);
  ledControl.setRow(0,1,neutralSmile[1]);
  ledControl.setRow(0,2,neutralSmile[2]);
  ledControl.setRow(0,3,neutralSmile[3]);
  ledControl.setRow(0,4,neutralSmile[4]);
  ledControl.setRow(0,5,neutralSmile[5]);
  ledControl.setRow(0,6,neutralSmile[6]);
  ledControl.setRow(0,7,neutralSmile[7]);
}

void desenharSorrisoTriste() {
  ledControl.setRow(0,0,sadSmile[0]);
  ledControl.setRow(0,1,sadSmile[1]);
  ledControl.setRow(0,2,sadSmile[2]);
  ledControl.setRow(0,3,sadSmile[3]);
  ledControl.setRow(0,4,sadSmile[4]);
  ledControl.setRow(0,5,sadSmile[5]);
  ledControl.setRow(0,6,sadSmile[6]);
  ledControl.setRow(0,7,sadSmile[7]);
}


