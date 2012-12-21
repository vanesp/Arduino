// Receiver for all RF12 data... the received data is not interpreted, just sent out
// all interpretation is done in the code on the Linux box...
// 2012-07-18 <vanes.peter@gmail.com>
// http://opensource.org/licenses/mit-license.php


#include <JeeLib.h>
#include <RF12.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 7                 // Data wire is plugged into port 7 on the Arduino
OneWire oneWire(ONE_WIRE_BUS);         // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire);   // Pass our oneWire reference to Dallas Temperature.

// By using direct addressing its possible to make sure that as you add temperature sensors
// the temperature sensor to variable mapping will not change.

int idSensor[] = { 2, 3 };              // map id of temp sensor in database
#define NRSENSORS  2                    // number of sensors
DeviceAddress Thermometer[NRSENSORS];   // array to hold device addresses

// number of sensors and lights... make 2 if gas sensor there
#define NSIGS 1
// sensors are on D3 & D4
#define SENSOR1 3  
// activity lights are on pins 8..9
#define LIGHT1  8

// payload data, as sent over the serial port

typedef struct {
    uint16_t count;  // counter, wraps after reaching 4095
    uint8_t value;  // current sensor value (only 0 or 1 used)
    uint8_t wrap;   // set if the counter has wrapped at least once
    uint8_t fresh; // set if this count is a new value
    uint16_t rate;   // number of milliseconds since last change
} TxItem;

struct {
    uint8_t seqnum8; // increased by one for each new packet
    uint8_t origin8; // can deal with up to 4 units
    TxItem data[NSIGS]; // sensor data, 7 bytes for each sensor
} payload;

static byte myNodeID;       // node ID used for this unit

MilliTimer milliTimer; // poll sensor every millisecond
MilliTimer sendTimer;  // retransmit the payload buffer every 3s
#define TIMER 60000      // Nr of milliseconds in main timing loop = 60 secs
#define TEMP  5          // send temperature every 5 minute (5 loops)

// Pulse counting settings
long startTime, endTime;  // counters for millis
int power, elapsedWh;     // power and energy
int ppwh = 1;             // 1000 pulses/kwh = 1 pulse per wh - Number of pulses per wh - found or set on the meter.


byte    j;
byte    ActualSensors;      // Actual number of sensors
uint8_t txrequest;          // set whenever there is new data to send
int     tempLoop;

void setup () {
    Serial.begin(57600);
    Serial.println("\n[GNR 01]");
    rf12_initialize(1, RF12_868MHZ, 5); // 868 Mhz, net group 5, node 1
    // keep as set up
    myNodeID = rf12_config();

    for (uint8_t i = 0; i < NSIGS; ++i) {
        pinMode(SENSOR1 + i, INPUT);
        digitalWrite(SENSOR1 + i, 1);
        pinMode(LIGHT1 + i, OUTPUT);
        digitalWrite(LIGHT1 + i, 0);
    }

    sensors.begin();
    ActualSensors = sensors.getDeviceCount();
    Serial.print(" T Sensors found: ");
    Serial.println (ActualSensors);
    if (ActualSensors > NRSENSORS) {
      ActualSensors = NRSENSORS;
    }
    for (j = 0; j<ActualSensors; j++) {
      if (!sensors.getAddress(Thermometer[j], j)) { 
        Serial.print("[GRN 01] Cannnot find Device ");
        Serial.println(j);
      } 
      // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(Thermometer[j], 9);
    }

    sensors.requestTemperatures();      // Throw away the first temperatures
    tempLoop = 0;
    startTime = millis();
    Serial.flush();
}


// should be called at least each millisec to accurately track sensor changes
void pollSensors() {
    // count milliseconds to de-bounce sensors, will trigger on 10th "1" value
    // use long ints in case a sensor remains "1" for over 65s
    static long triggers[NSIGS];
    
    // track last millisecond to be able to report millis since last
    static long mlast[NSIGS];

    if (milliTimer.poll(1)) {
        long ms = millis();
        for (uint8_t i = 0; i < NSIGS; ++i) {
            TxItem *ip = payload.data + i;
            ip->value = ! digitalRead(SENSOR1 + i);
            digitalWrite(LIGHT1 + i, ip->value);
            if (ip->value == 0)
                triggers[i] = -10;
            else if (++triggers[i] == 0) {
                // can represent 2^27 millisecs, i.e. about 1.5 days
                ip->rate = ms - mlast[i];
                mlast[i] = ms;
                // txrequest = ip->fresh = 1;
                if (++ip->count == 0) {
                    ip->wrap = 1;
                    txrequest = 1;
                }
            }
        }
    }
}


void loop () {
    int i;
    int Temp;

    pollSensors();
    
    if (rf12_recvDone() && rf12_crc == 0) {
        byte n = rf12_len;

        Serial.print("GNR ");
        Serial.print((int) rf12_hdr);  // whole header... &0x1F is node ID... header has info on PIR

        for (i=0; i<n; i++) {
          Serial.print (' ');
          Serial.print ((int) rf12_data[i]);        
        }
        Serial.println();
        Serial.flush();

        if (RF12_WANTS_ACK) {
          // send an ack
          rf12_sendStart(RF12_ACK_REPLY, 0, 0);
          rf12_sendWait(1); // don't power down too soon
        }
    }
    
    if (sendTimer.poll(TIMER)) {
        txrequest = 1;
        tempLoop++;
    }
    
    if (txrequest) {
        // send electricity values
        txrequest = 0;
        for (uint8_t i = 0; i < NSIGS; ++i) {
          Serial.print("ELEC ");
          endTime = millis();
          // rate is in millisecs, calculate watts
          power = int((TIMER / (endTime - startTime) * payload.data[i].count )/ppwh);  
          startTime = endTime;
          Serial.print(power);  
          Serial.print (' ');
          Serial.println (payload.data[i].count);        
          Serial.flush();
          payload.data[i].count = 0;    // reset count
          payload.data[i].fresh = 0;
        }
        sendTimer.poll(TIMER); // reset
    }
    
    if (tempLoop >= TEMP) {
        tempLoop = 0;
        sensors.requestTemperatures();       // Send the command to get temperatures
        for (j = 0; j < ActualSensors; j++) {
           Temp = sensors.getTempC(Thermometer[j]) * 100;
           Serial.print("TMP ");
           Serial.print(idSensor[j]);  
           Serial.print (' ');
           Serial.println (Temp);        
           Serial.flush();
        }
    }
      
}
