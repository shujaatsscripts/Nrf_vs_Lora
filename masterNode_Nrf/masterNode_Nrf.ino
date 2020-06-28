
/*
This code is programmed in Nrf Master Node
*/
#include <SPI.h>
#include "RF24.h"

/****************** Data variables ***************************/
struct nrfData{
    int averageRoundTripdelay = 0;
    int totalPackets = 0;
    int packetloss = 0;
    int delayTime = 0;
};
nrfData packetData;
/**********************************************************/

/****************** User Config ***************************/
#define timeoutDelay 200000 //Recieve back timeout delay in milliseconds
bool radioNumber = 1; // Set this radio as radio number 0 or 1
int packetNo = 0;
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(9,10);
/**********************************************************/

byte addresses[][6] = {"1Node","2Node"};

// Used to control whether this node is sending or receiving
bool role = 1;

void setup() {
  Serial.begin(115200);
  Serial.println(F("RF24/examples/GettingStarted"));
  Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
  
  radio.begin();

  // Set the PA Level low to prevent power supply related issues since this is a
 // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  radio.setPALevel(RF24_PA_MAX);

  // Open a writing and reading pipe on each radio, with opposite addresses
  if(radioNumber){
    radio.openWritingPipe(addresses[1]);
    radio.openReadingPipe(1,addresses[0]);
  }else{
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
  }
}

void loop() {

/*************** Initializing Test Parameters****************/ 
    packetData.totalPackets = 500;
    packetData.delayTime = 0;
    packetData.averageRoundTripdelay = 0;
    packetData.totalPackets = 0;

    if ( Serial.available() ){
        char c = toupper(Serial.read());
        if ( c == 'S' && role == 0 ){      
            Serial.print(F("Staring Test in"));
            delay(1000);
            Serial.print(F(" 3 "));
            delay(1000);
            Serial.print(F(" 2 "));
            delay(1000);
            Serial.println(F(" 1 "));
            delay(1000);
            Serial.print(F("Start"));
            delay(1000);
            }
/****************** Ping Out Role ***************************/    
    
    for (int n = 0 ; n < packetData.totalPackets ; n++){
        unsigned long start_time = micros();                       // Take the time, and send it.  This will block until complete
        if (!radio.write( &start_time, sizeof(unsigned long) )){
            Serial.println(F("failed"));
            }
        
        radio.startListening();                                    // Now, continue listening
    
        unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
        boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not
    
        while ( !radio.available() ){                             // While nothing is received
            if (micros() - started_waiting_at > timeoutDelay ){            // If waited longer than 200ms, indicate timeout and exit while loop
            timeout = true;
            break;
            }      
        }
        
        if ( timeout ){                                             // Describe the results
            Serial.println(F("Failed, response timed out."));
        }else{
            unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
            radio.read( &got_time, sizeof(unsigned long) );
            unsigned long end_time = micros();
        
        // Spew it
            Serial.print(F("Sent "));
            Serial.print(start_time);
            Serial.print(F(", Got response "));
            Serial.print(got_time);
            Serial.print(F(", Round-trip delay "));
            Serial.print(end_time-start_time);
            Serial.println(F(" microseconds"));
            }

    // Try again 1s later
        delay(5000);
    }

} // Loop