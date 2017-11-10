#include <MeetAndroid.h>
MeetAndroid meetAndroid;

int sSensorAnalPin = A15; //porta analógica ligado ao sensor de som
int sSensorActualValue = 0;  //variável que guarda o valor que o sensor de som analógico esta captando
int sSensorLimit = 0; //valor que define o limite de barulho que pode ser feito no ambiente
int sSensorTimeMesuring = 0; //indica o tempo que o sensor de som irá ficar medindo o som ambiente
int androidValues[] = {0,0}; //o primeiro valor é o limite e o segundo o tempo de medição
//int sSensorCalibration = 0; //valor que define a calibração do microfone
const String HALF_VALUE = "HALF_VALUE";
const String EXCEEDED_VALUE = "EXCEEDED_VALUE";
const String FINISH = "FINISH";
const String STOP = "STOP";


void setup()
{
  //pinMode(ledPin,OUTPUT);
  
  Serial.begin(9600);
  //inicializando as variáveis. Importante para toda vez que começar uma nova medição.
  sSensorActualValue = 0;
  sSensorLimit = 0;
  sSensorTimeMesuring = 0;
  //sSensorCalibration = 0; 
  androidValues[0] = 0;
  androidValues[1] = 0;
  //Registrando a função que será chamada pelo Android para iniciar a medição.
  meetAndroid.registerFunction(iniciarMedicao, 'I');
  //Registrando a função que será chamada pelo Android para parar a medição antes do tempo acabar.
  meetAndroid.registerFunction(pararMedicao, 'P');
  inicializarLedsArduino();
  
}

void loop(){
  meetAndroid.receive();
}

void iniciarMedicao() {

  //Serial.println("Entrou em iniciarMedicao!!!");
  meetAndroid.send("LOG: Entrou em iniciarMedicao!!!");
  
  long tempoMiliSegundos = 60000;
  int segundos = 0;
  
  setup();

  meetAndroid.getIntValues(androidValues);
  sSensorLimit = androidValues[0];
  sSensorTimeMesuring = androidValues[1];

  meetAndroid.send("LOG: sSensorLimit:  ");
  meetAndroid.send(sSensorLimit);
  meetAndroid.send("LOG: sSensorTimeMesuring:  ");
  meetAndroid.send(sSensorTimeMesuring);
  
  tempoMiliSegundos = sSensorTimeMesuring * 60000; //o valor do tempo vem em minutos

  
  meetAndroid.send("LOG: Tempo em milisegundos:  ");
  meetAndroid.send(tempoMiliSegundos);
  
  for (long i = 0; i <= tempoMiliSegundos; i++) {
    segundos = i % 1000;
    
    if (segundos == 0) { //para ler apenas a cada segundo
      //Serial.print("SEGUNDO: ");
      //Serial.println(segundos);
      sSensorActualValue = analogRead(sSensorAnalPin);  
      verificaLimite();
    }
    
    //meetAndroid.send("LOG: Valor de i =  ");
    //meetAndroid.send(i);
  }
  
  //envia mensagem para o Android avisando que acabou a medição
  meetAndroid.send("FINISH");
  //Serial.print("TERMINOU!!!");
  
}


void verificaLimite() {

  int metade = sSensorLimit / 2;

  meetAndroid.send("LOG: Leitura atual do sensor: ");
  meetAndroid.send(sSensorActualValue); //envia para o android a leitura atual do sensor
  //Serial.print("VAlOR DO SENSOR: ");
  //Serial.println(sSensorActualValue);
  
  if(sSensorActualValue >= metade && sSensorActualValue < sSensorLimit) {
    //manda avisando que está próximo do valor  
    meetAndroid.send("LOG: Já está na metade do valor");
    meetAndroid.send("HALF_VALUE");
    //chama função que alerta no Arduino
    alterarLedsArduino(HALF_VALUE);
  } else if (sSensorActualValue >= sSensorLimit) {
    //manda avisando que o valor ultrapassou o limite
    meetAndroid.send("LOG: Ultrapassou o limite");
    meetAndroid.send("EXCEEDED_VALUE");
    //chama função que alerta no Arduino
    alterarLedsArduino(EXCEEDED_VALUE);
  }
}


void pararMedicao () {
  meetAndroid.send("STOP");
  //pensar se precisa deste setup aqui
  delay(3000);
  setup();
}

void inicializarLedsArduino () {
  meetAndroid.send("LOG: Inicializar Leds");
}

void alterarLedsArduino (String valor) {
  meetAndroid.send("LOG: Alterar Leds Arduino");
}


/*
 

int Led = 13;//define LED port
int buttonpin = 3; //define switch port
int  val;//define digital variable val

void  setup()
{
  pinMode(Led,OUTPUT);//define LED as a output port
  pinMode(buttonpin,INPUT);//define switch as a output port
}

void  loop()
{ 
  val = digitalRead(buttonpin);//read the value of the digital interface 3 assigned to val 
  
  if(val == HIGH) {  //when the switch sensor have signal, LED blink
    digitalWrite(Led,HIGH);
  } else {
    digitalWrite(Led,LOW);
  }
}

*/
