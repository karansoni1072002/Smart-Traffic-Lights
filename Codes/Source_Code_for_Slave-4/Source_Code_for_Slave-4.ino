//SOURCE CODE FOR SLAVE 4
 
 
#include <TM1637.h>
#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <printf.h>
#define CE_PIN 9
#define CSN_PIN 8
RF24 radio(CE_PIN,CSN_PIN);
#define NODE_ID 3
const byte nodeAddresses[4][5] = {
    {'S','L','V','E','1'},
    {'S','L','V','E','2'},
    {'S','L','V','E','3'},
    {'S','L','V','E','4'}
};
int dataReceived[3]={2,105,45};
int dataToSend=-1;
int CLK=4;
int DIO=6;
int s1;
int s2;
int s3;
int grnTime;
int sensorAna1=A1;
int sensorAna2=A2;
boolean flg=false;
boolean flg1=false;
int sensorAna3=A3;
int redSwitch=2;
int t1;
int k,tk1,tk2,tk3,tk4;
int t2;
int yellowSwitch=3;
int greenSwitch=7;
TM1637 tm(CLK,DIO);
void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(0x66);
  radio.openReadingPipe(1, nodeAddresses[NODE_ID]);     
  radio.enableAckPayload();
  printf_begin();
  radio.printDetails();
  grnTime=30;
  radio.startListening();
  tm.set(4);// setting brightness
}
void loop() {
  t1=millis();
  digitalWrite(yellowSwitch,LOW);
  digitalWrite(redSwitch,LOW);
  digitalWrite(greenSwitch,HIGH);
  startTimer(grnTime,0);
  while(1){
  if(flg1)break;
  radioCheckAndReply();
  }
  flg1=false;
  digitalWrite(redSwitch,HIGH);
  digitalWrite(greenSwitch,LOW);
  startTimer(dataReceived[2],1);
  //one sec  interval
  collectCarDensity();
  radio.writeAckPayload(1, &dataToSend, sizeof(dataToSend));
  while(1){
    if(flg)break;
    radioCheckAndReply();
  }
  flg=false;
  tm.display(0,0);
    tm.display(1,tk2-1);
    tm.display(2,tk3-1);
    tm.display(3,tk4-1);
    delay(999);
  t2=millis();
  startTimer((int)(dataRecieved[1]-(t2-t1)),2);
  digitalWrite(redSwitch,LOW);
  digitalWrite(yellowSwitch,HIGH);
  startTimer(2,0);
}
void radioCheckAndReply(void)
{
    if ( radio.available() ) {
          radio.read( dataReceived, sizeof(dataReceived) );
          Serial.println("Received request from master - sending preloaded data.");
          Serial.print("The received count from the master was: ");
          Serial.println(dataReceived[0]);
          Serial.println("--------------------------------------------------------");
          if(dataReceived[0]==1){
            flg=true;
            grnTime=dataToSend*15; 
            dataToSend=-1;
            radio.writeAckPayload(1, &dataToSend, sizeof(dataToSend));
          }
          else{
              flg1=true;
          }
    }
}
void getCarDensity(void){
    if(s3==99)
    dataToSend=3;
    else if(s2==99)
    dataToSend=2;
    else if(s1==99)
    dataToSend=1;
    else{
        // this condition is very unlikely to happen but i have added it for sake of continuity of the code
        dataToSend=0;
    }
}
void collectCarDensity (void){
    s3=analogRead(sensorAna3);
    if(s3>=300)s3=99;
    else s3=0;
    s2=analogRead(sensorAna2);
    if(s2>=300)s2=99;
    else s2=0;
    s1=analogRead(sensorAna1);
    if(s1>=300)s1=99;
    else s1=0;
    getCarDensity();
}
void startTimer(int initial,int final){
    for(int i=(int)t2;i>=2;i--){
      k=i;
      tk1=0;
      tk4=k%10;
      k/=10;
      tk3=k%10;
      k/=10;
      tk2=k%10;
      tm.display(0,0);
      tm.display(1,tk2);
      tm.display(2,tk3);
      tm.display(3,tk4);
      delay(999);
  }
 
}
