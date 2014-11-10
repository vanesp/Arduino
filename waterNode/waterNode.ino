// A waterNode, based on the roomNodePvE


// Water-Detector, Detect water with a little circuit
  
//  2N390 NPN transistor
//  E (pin 2) to ground
//  C (pin 3) is output to waterPin, and 10KOhm to +5V
//  B (pin 2) through 10K Ohm to sensor 1
//  Sensor 2 to +5V
//  When water = conductive = +ve voltage to base = collector goes to ground = LOW on waterPin

//  modified 7 november 2014
//  by Peter van Es

// The complexity in the code below comes from the fact that newly detected water alarm (similar to PIR motion)
// needs to be reported as soon as possible, but only once, while all the
// other sensor values (temperature) are being collected and averaged in a more regular cycle.

// PvE: changes made to allow for non-struct data sending to allow processing in the php code

// The Studeerkamer Jeenode has the Roomboard connected to Port 2 and 3 and uses the SHT11, Roomnode id = 2
// The Woonkamer Jeenode has the Roomboard connected to Port 1 (HYT131) and 4, Roomnode id = 3
// The waterNode has Roomnode id = 6, the circuitboard is connected to Port 1 (which equates to PD4 = Arduino 4)
// The DS18S20 is connected to Port 1 AIO1 which is Arduino Digital 14

#include <JeeLib.h>
#include <PortsSHT11.h>
#include <avr/sleep.h>
#include <util/atomic.h>
#include <OneWire.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

#include <DallasTemperature.h>

// Data wire is plugged into port 14 on the Arduino
#define ONE_WIRE_BUS 14
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;
int  resolution = 12;
int  delayInMillis = 0;
float temperature = 0.0;


#define SERIAL  0   // set to 1 to also report readings on the serial port
#define DEBUG   0   // set to 1 to display each loop() run and PIR trigger

#define PIR_PORT    1   // defined if PIR is connected to a port's DIO pin, but here it is a water detector
#define NODE        6

#define MEASURE_PERIOD  600 // how often to measure, in tenths of seconds
#define RETRY_PERIOD    10  // how soon to retry if ACK didn't come in
#define RETRY_LIMIT     5   // maximum number of times to retry
#define ACK_TIME        10  // number of milliseconds to wait for an ack
#define REPORT_EVERY    5   // report every N measurement cycles
#define SMOOTH          3   // smoothing factor used for running averages

// set the sync mode to 2 if the fuses are still the Arduino default
// mode 3 (full powerdown) can only be used with 258 CK startup fuses
#define RADIO_SYNC_MODE 2

// External definitions from RF12.h to allow IT+ communications
extern boolean ITPlusFrame;
extern void rf12_initialize_overide_ITP ();

// The scheduler makes it easy to perform various tasks at various times:

enum { MEASURE, REPORT, TASK_END };

static word schedbuf[TASK_END];
Scheduler scheduler (schedbuf, TASK_END);

// Other variables used in various places in the code:

static byte reportCount;    // count up until next report, i.e. packet send
static byte myNodeID;       // node ID used for this unit

// This defines the structure of the packets which get sent out by wireless:

struct {
    byte light;     // light sensor: 0..255
    byte moved;     // motion detector: 0..1
    byte humi;      // humidity: 0..100
    int  temp;      // temperature: -500..+500 (tenths)
    byte lobat;     // supply voltage dropped under 3.1V: 0..1
} payload;

// Conditional code, depending on which sensors are connected and how:

#if SHT11_PORT
    SHT11 sht11 (SHT11_PORT);
#endif

#if HYT131_PORT
    PortI2C hyti2cport (HYT131_PORT);
    HYT131 hyt131 (hyti2cport);
#endif

#if LDR_PORT
    Port ldr (LDR_PORT);
#endif

#if PIR_PORT
    #define PIR_HOLD_TIME   30  // hold PIR value this many seconds after change
    #define PIR_PULLUP      0   // set to one to pull-up the PIR input pin
    #define PIR_FLIP        1   // 0 or 1, to match PIR reporting high or low
    
    class PIR : public Port {
        volatile byte value, changed;
        volatile uint32_t lastOn;
    public:
        PIR (byte portnum)
            : Port (portnum), value (0), changed (0), lastOn (0) {}

        // this code is called from the pin-change interrupt handler
        void poll() {
            // see http://talk.jeelabs.net/topic/811#post-4734 for PIR_FLIP
            byte pin = digiRead() ^ PIR_FLIP;
            // if the pin just went on, then set the changed flag to report it
            if (pin) {
                if (!state())
                    changed = 1;
                lastOn = millis();
            }
            value = pin;
        }

        // state is true if curr value is still on or if it was on recently
        byte state() const {
            byte f = value;
            if (lastOn > 0)
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    if (millis() - lastOn < 1000 * PIR_HOLD_TIME)
                        f = 1 ^ PIR_FLIP;
                }
            return f;
        }

        // return true if there is new motion to report
        byte triggered() {
            byte f = changed;
            changed = 0;
            return f;
        }
    };

    PIR pir (PIR_PORT);

    // the PIR signal comes in via a pin-change interrupt
    ISR(PCINT2_vect) { pir.poll(); }
#endif

// has to be defined because we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// utility code to perform simple smoothing as a running average
static int smoothedAverage(int prev, int next, byte firstTime =0) {
    if (firstTime)
        return next;
    return ((SMOOTH - 1) * prev + next + SMOOTH / 2) / SMOOTH;
}

// spend a little time in power down mode while the SHT11 does a measurement
static void shtDelay () {
    Sleepy::loseSomeTime(32); // must wait at least 20 ms
}

// wait a few milliseconds for proper ACK to me, return true if indeed received
static byte waitForAck() {
    MilliTimer ackTimer;
    while (!ackTimer.poll(ACK_TIME)) {
        if (rf12_recvDone() && rf12_crc == 0 &&
                // see http://talk.jeelabs.net/topic/811#post-4712
                rf12_hdr == (RF12_HDR_DST | RF12_HDR_CTL | myNodeID))
            return 1;
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }
    return 0;
}

// readout all the sensors and other values
static void doMeasure() {
    byte firstTime = payload.humi == 0; // special case to init running avg
    
    payload.lobat = rf12_lowbat();

    #if SHT11_PORT
#ifndef __AVR_ATtiny84__
        sht11.measure(SHT11::HUMI, shtDelay);        
        sht11.measure(SHT11::TEMP, shtDelay);
        float h, t;
        sht11.calculate(h, t);
        int humi = h + 0.5, temp = 10 * t + 0.5;
#else
        //XXX TINY!
        int humi = 50, temp = 25;
#endif
        payload.humi = smoothedAverage(payload.humi, humi, firstTime);
        payload.temp = smoothedAverage(payload.temp, temp, firstTime);
    #endif
    #if HYT131_PORT
        int humi, temp;
        hyt131.reading(temp, humi);
        payload.humi = smoothedAverage(payload.humi, humi/10, firstTime);
        payload.temp = smoothedAverage(payload.temp, temp, firstTime);
    #endif
    #if LDR_PORT
        ldr.digiWrite2(1);  // enable AIO pull-up
        byte light = ~ ldr.anaRead() >> 2;
        ldr.digiWrite2(0);  // disable pull-up to reduce current draw
        payload.light = smoothedAverage(payload.light, light, firstTime);
    #endif
    #if PIR_PORT
        payload.moved = pir.state();
    #endif
    
    sensors.requestTemperatures();
    Sleepy::loseSomeTime(delayInMillis);
    temperature = sensors.getTempCByIndex(0);
    payload.temp = smoothedAverage(payload.temp, temperature*10, firstTime);
 
}

static void serialFlush () {
    #if ARDUINO >= 100
        Serial.flush();
    #endif  
    delay(2); // make sure tx buf is empty before going back to sleep
}

// periodic report, i.e. send out a packet and optionally report on serial port
static void doReport() {
    rf12_sleep(RF12_WAKEUP);
    rf12_sendNow(0, &payload, sizeof payload);
    rf12_sendWait(RADIO_SYNC_MODE);
    rf12_sleep(RF12_SLEEP);

    #if SERIAL
        Serial.print("ROOM ");
        Serial.print((int) payload.light);
        Serial.print(' ');
        Serial.print((int) payload.moved);
        Serial.print(' ');
        Serial.print((int) payload.humi);
        Serial.print(' ');
        Serial.print((int) payload.temp);
        Serial.print(' ');
        Serial.print((int) payload.lobat);
        Serial.println();
        serialFlush();
    #endif
}

// send packet and wait for ack when there is a motion trigger
static void doTrigger() {
    #if DEBUG
        Serial.print("PIR ");
        Serial.print((int) payload.moved);
        serialFlush();
    #endif

    for (byte i = 0; i < RETRY_LIMIT; ++i) {
        rf12_sleep(RF12_WAKEUP);
        rf12_sendNow(RF12_HDR_ACK, &payload, sizeof payload);
        rf12_sendWait(RADIO_SYNC_MODE);

        byte acked = waitForAck();
        rf12_sleep(RF12_SLEEP);

        if (acked) {
            #if DEBUG
                Serial.print(" ack ");
                Serial.println((int) i);
                serialFlush();
            #endif
            // reset scheduling to start a fresh measurement cycle
            scheduler.timer(MEASURE, MEASURE_PERIOD);
            return;
        }
        
        delay(RETRY_PERIOD * 100);
    }
    scheduler.timer(MEASURE, MEASURE_PERIOD);
    #if DEBUG
        Serial.println(" no ack!");
        serialFlush();
    #endif
}

void blink (byte pin) {
    for (byte i = 0; i < 6; ++i) {
        delay(100);
        digitalWrite(pin, !digitalRead(pin));
    }
}

void setup () {
    // Set-up RF12 library
    // Choose Roomnode id here !
    myNodeID = rf12_initialize(NODE, RF12_868MHZ, 0xd4); // 0xd4 needed for IT+
    // Overide settings for RFM01/IT+ compliance
    rf12_initialize_overide_ITP();

    #if SERIAL || DEBUG
        Serial.begin(57600);
        Serial.print("\n[roomNode.");
        Serial.print(NODE);
        Serial.print("]");
        serialFlush();
    #endif

    sensors.begin();
    sensors.getAddress(tempDeviceAddress, 0);
    sensors.setResolution(tempDeviceAddress, resolution);
  
    sensors.setWaitForConversion(false);
    delayInMillis = 750;       // for 12 bit resolution 
    
    rf12_sleep(RF12_SLEEP); // power down
    
    #if PIR_PORT
        pir.digiWrite(PIR_PULLUP);
#ifdef PCMSK2
        bitSet(PCMSK2, PIR_PORT + 3);
        bitSet(PCICR, PCIE2);
#else
        //XXX TINY!
#endif
    #endif

    reportCount = REPORT_EVERY;     // report right away for easy debugging
    scheduler.timer(MEASURE, 0);    // start the measurement loop going
}

void loop () {
    #if DEBUG
        Serial.print('.');
        serialFlush();
    #endif

    #if PIR_PORT
        if (pir.triggered()) {
            payload.moved = pir.state();
            doTrigger();
        }
    #endif
    
    switch (scheduler.pollWaiting()) {

        case MEASURE:
            // reschedule these measurements periodically
            scheduler.timer(MEASURE, MEASURE_PERIOD);
    
            doMeasure();

            // every so often, a report needs to be sent out
            if (++reportCount >= REPORT_EVERY) {
                reportCount = 0;
                scheduler.timer(REPORT, 0);
            }
            break;
            
        case REPORT:
            doReport();
            break;
    }
}
