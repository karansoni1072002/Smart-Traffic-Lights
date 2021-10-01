//SOURCE CODE FOR MASTER - ARDUINO UNO 
 
 
#include <SPI.h>
#include <nRF24l01.h>
#include <printf.h>
// set Chip-Enable (CE) and Chip-Select-Not (CSN) radio setup pins
#define CE_PIN 9
#define CSN_PIN 10
// set transmission cycle send rate - in milliseconds
#define SEND_RATE 1000
// create RF24 radio object using selected CE and CSN pins
RF24 radio(CE_PIN,CSN_PIN);
const byte nodeAddresses[4][5] = {
    {'S','L','V','E','1'},
    {'S','L','V','E','2'},
    {'S','L','V','E','3'},
    {'S','L','V','E','4'}
};
// integer to store count of successful transmissions
int prevG1=15;
int prevG2=30;
int prevG3=30;
int prevG4=45;
 
int masterSendCount = 0;
int carData[4]={1,2,2,3};
unsigned long currentTime;
unsigned long lastSentTime;
const int askCarCount[3]={1,0,0};
int sendTimeData[3]={2,-1,-1};
int garbage=0;
int dly=0;
void setup()
{
    Serial.begin(9600);
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setDataRate(RF24_250KBPS);// setted for max range with compensation for min send rate(250 KBPS)
    radio.setChannel(0x76);
    radio.setRetries(4, 10);
    radio.enableAckPayload();
}
void loop()
{
    dly=(prevG2-0.00018)*1000;
    delay(dly);//delaying for a fixed time
    receiveCarData(0,0); //collecting sensor data
    prevG1=assingTime(carData[0]);// updating received data locally
    sendTimeData[1]=prevG1+prevG3+prevG4;
    sendTimeData[2]=prevG3;
    giveTimeData(1);// sending the red light time to slave
    dly=(prevG3-0.00018)*1000;
    delay(dly);
    receiveCarData(1,1);
    prevG2=assingTime(carData[1]);
    sendTimeData[1]=prevG4+prevG1+prevG2;
    sendTimeData[2]=prevG4;
    giveTimeData(2);
    dly=(prevG4-0.00018)*1000;
    delay(dly);
    receiveCarData(2,2);
    prevG3=assingTime(carData[2]);
    sendTimeData[1]=prevG3+prevG1+prevG2;
    sendTimeData[2]=prevG1;
    giveTimeData(3);
    dly=(prevG1-0.00018)*1000;
    delay(dly);
    receiveCarData(3,3);
    prevG4=assingTime(carData[3]);
    sendTimeData[1]=prevG3+prevG4+prevG2;
    sendTimeData[2]=prevG2;
    giveTimeData(0);
}
void receiveCarData(byte node,int i)
{
    Serial.print("[*] Master unit has successfully sent and received data ");
    Serial.print(masterSendCount);
    Serial.println(" times.");
        // setup a write pipe to current sensor node - must match the remote node listening pipe
        radio.openWritingPipe(nodeAddresses[node]);
        Serial.print("[*] Attempting to transmit car data to node ");
        Serial.println(node + 1);
        Serial.print("[*] The master unit count being sent is: ");
        Serial.println(masterSendCount);
        // boolean to indicate if radio.write() tx was successful
        bool tx_sent;
        tx_sent = radio.write( &askCarCount,sizeof(askCarCount));
        // if tx success - receive and read slave node ack reply
        if (tx_sent) {
            if (radio.isAckPayloadAvailable()) {
                radio.read(&carData[i], sizeof(carData[i]));
                Serial.print("[+] Successfully received data from node: ");
                Serial.println(node);
                Serial.print("  ---- The node count received was: ");
                Serial.println(carData[i]);
                // iterate master device count - keeps data changing between transmissions
                if (masterSendCount < 500) {
                    masterSendCount++;
                } else {
                    masterSendCount = 1;
                }
            }
        }
        else {
            Serial.print("[-] The transmission to the selected node failed.");
        }
    Serial.println("--------------------------------------------------------");
}
void giveTimeData(byte node)
{
    Serial.print("[*] Master unit has successfully sent and received data ");
    Serial.print(masterSendCount);
    Serial.println(" times.");
        // setup a write pipe to current sensor node - must match the remote node listening pipe
        radio.openWritingPipe(nodeAddresses[node]);
        Serial.print("[*] Attempting to transmit car data to node ");
        Serial.println(node + 1);
        Serial.print("[*] The master unit count being sent is: ");
        Serial.println(masterSendCount);
        // boolean to indicate if radio.write() tx was successful
        bool tx_sent;
        tx_sent = radio.write( &sendTimeData,sizeof(sendTimeData));
        // if tx success - receive and read slave node ack reply
        if (tx_sent) {
            if (radio.isAckPayloadAvailable()) {
                radio.read(&garbage, sizeof(garbage));
                Serial.print("[+] Successfully received data from node: ");
                Serial.println(node);
                Serial.print("  ---- The node count received was: ");
                Serial.println(garbage);
                // iterate master device count - keeps data changing between transmissions
                if (masterSendCount < 500) {
                    masterSendCount++;
                } else {
                    masterSendCount = 1;
                }
            }
        }
        else {
            Serial.print("[-] The transmission to the selected node failed.");
        }
    Serial.println("--------------------------------------------------------");
}
int assingTime(int val){
  if(val==1)return 15;
  if(val==2)return 30;
  return 45;
}
