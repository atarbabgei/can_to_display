#include <Arduino.h>
/*
 * VCU Dashboard Data Logger
 * v1.0.0
 * 
 */

#include <SPI.h>
#include "mcp2515_can.h"

// Choose SERIAL PORT MONITOR (Internal USB Serial) or Serial1 (Out)
#define OUTSERIAL SERIAL_PORT_MONITOR 
//#define OUTSERIAL Serial1

struct Dash_data{
  int v_speed;
  int v_battery;
  int v_temperature;
};

const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin

#define MAX_DATA_SIZE 8
#define CAN_SPEED CAN_250KBPS
#define SERIAL_BAUD 115200
const uint32_t VCU_ADDRESS_1 = 0x0CF00400;
const uint32_t VCU_ADDRESS_2 = 0x0CF00500;

int map_one_byte(int x, int in_min, int in_max, int out_min, int out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup() {
    OUTSERIAL.begin(SERIAL_BAUD);
    Dash_data m_dash_data;
    while (!SERIAL_PORT_MONITOR) {}
    
    // init can bus : baudrate = 250k, MCP XTAL 8 MHz (Mini Module) 
    while (CAN_OK != CAN.begin(CAN_SPEED, MCP_8MHz)) {             
        //SERIAL_PORT_MONITOR.println(F("CAN init fail, retry..."));
        OUTSERIAL.println("CAN init fail, retry...");
        delay(100);
    }
   OUTSERIAL.println("CAN init ok!");

   // Set CAN Mask
   CAN.init_Mask(0, 1, 0x0CFFFFFF);                         
   CAN.init_Mask(1, 1, 0x0CFFFFFF);

  // Set CAN Filter to get specified ID
   CAN.init_Filt(0, 1, 0x0CF00400);                          
   CAN.init_Filt(1, 1, 0x0CF00500);                          
}

uint32_t id;
uint8_t  len;
byte raw_data[MAX_DATA_SIZE] = {0};


void loop() {
    // check if data coming
    if (CAN_MSGAVAIL != CAN.checkReceive()) {
        return;
    }

    // read data, len: data length, buf: data buf
    CAN.readMsgBuf(&len, raw_data);

    if(CAN.getCanId()==VCU_ADDRESS_1){
      OUTSERIAL.println("Address 1");
      
      for (int i = 0; i < len; i++) { // print the data
            OUTSERIAL.print(raw_data[i]);
            OUTSERIAL.print("\t");
        }
      OUTSERIAL.println(" ");
     }
    else if(CAN.getCanId()==VCU_ADDRESS_2){
      OUTSERIAL.println("Address 2");
      
      for (int i = 0; i < len; i++) { // print the data
            OUTSERIAL.print(raw_data[i]);
            OUTSERIAL.print("\t");
        }
      OUTSERIAL.println(" ");
     }
   else{
      OUTSERIAL.println("Others");
    
      for (int i = 0; i < len; i++) { // print the data
            OUTSERIAL.print(raw_data[i]);
            OUTSERIAL.print("\t");
        }
      OUTSERIAL.println(" ");
     }
}