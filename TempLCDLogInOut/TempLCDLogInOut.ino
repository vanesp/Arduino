#include <OneWire.h>
#include <DallasTemperature.h>
#include <SD.h>

// LCD display stuff
#include <Wire.h> // I2C library include
#include <Deuligne.h> // LCD library include
#include "RTClib.h"    // for RTC

Deuligne lcd;         // lcd object declaration
RTC_DS1307 RTC;       // rtc 

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Snootlab Memoire: pin 10
// Sparkfun SD shield: pin 8
const int chipSelect = 10;    


// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  5000 // mills between entries
#define ECHO_TO_SERIAL   0 // echo data to serial port
#define WAIT_TO_START    0 // Wait for serial input in setup()

#define MAXDEVICES  2      // how many devices can we handle...
#define INSIDE  0          // index to array
#define OUTSIDE  1         // index to array

// arrays to hold device address
DeviceAddress Thermometer[MAXDEVICES];

// the logging file
File logfile;

// keep track of max, min, avg temperature per hour
float min_temp[MAXDEVICES];
float max_temp[MAXDEVICES];
float avg_temp[MAXDEVICES];
int avg_cnt[MAXDEVICES];
float avg;

int hr, lasthr;            // index in array
int NrDevices;
int key = -1;                // did a key get pressed?
int oldkey = -1;

void setup(void)
{
  int j;
  // start serial port
  Serial.begin(9600);
  // Bluetooth set-up
  // Serial.print("$$$");
  // delay(1000);
  // Serial.println("SP,0000");  // set pin to 0000
  // delay(1000);
  // Serial.println("---");      // back to normal mode
  Serial.println();
  
#if WAIT_TO_START
  Serial.println("Type any character to start");
  while (!Serial.available());
#endif //WAIT_TO_START

  // set-up LCD
  Wire.begin(); // join i2c
  lcd.init(); // LCD init
  lcd.clear(); // Clear Display
  lcd.backLight(true); // Backlight ON

  RTC.begin();
  if (! RTC.isrunning()) {
    lcd.print("RTC is NOT running!");
    // date and time should be set
    delay(1000);
  }

  // locate devices on the bus
  sensors.begin();
  lcd.print("Found ");
  NrDevices = sensors.getDeviceCount();
  lcd.print(NrDevices, DEC);
  lcd.print(" devices.");
  if (NrDevices < 1) {
    lcd.clear();
    lcd.print("No temperature sensor");
    delay(2000);
    while(1);
  }
  delay(1000);
  
  // initialize the SD card
  lcd.clear(); // Clear Display
  lcd.print("Init SD...");
  lcd.setCursor(0,1);  // second line
  // make sure that the default chip select pin is set to
  // output, even if you don't use it:
  pinMode(chipSelect, OUTPUT);
  
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.print("Card fail");
    delay(2000);
    while(1);
  }
  lcd.print("SD OK.");
  delay(1000);

  // create a new file
  char filename[] = "LOGGER00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (!SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (!logfile) {
    lcd.clear();
    lcd.print("can't make file");
    delay(2000);
    while(1);
  }
  
  lcd.clear();
  lcd.print("Logging to: ");
  lcd.setCursor(0,1);  // second line
  lcd.print(filename);
  delay(2000);

    // and write the stuff to the logger file...
  logfile.println("millis, which, time, min, max, avg");    
  logfile.flush();

  // assign address manually.  the addresses below will beed to be changed
  // to valid device addresses on your bus.  device address can be retrieved
  // by using either oneWire.search(deviceAddress) or individually via
  // sensors.getAddress(deviceAddress, index)
  //insideThermometer = { 0x28, 0x1D, 0x39, 0x31, 0x2, 0x0, 0x0, 0xF0 };

  // Method 1:
  // search for devices on the bus and assign based on an index.  ideally,
  // you would do this to initially discover addresses on the bus and then 
  // use those addresses and manually assign them (see above) once you know 
  // the devices on your bus (and assuming they don't change).
  // clear the arrays
  lcd.clear();
  for (j = 0; j<MAXDEVICES; j++) {
      min_temp[j] = 99;
      max_temp[j] = -99;
      avg_temp[j] = 0;
      avg_cnt [j] = 0;
      if (!sensors.getAddress(Thermometer[j], j)) 
      { 
        lcd.print("Cannnot find Device ");
        lcd.print(j);
      } 
      // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(Thermometer[j], 9);
  }
  lasthr = -1;  
  
  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices, 
  // or you have already retrieved all of them.  It might be a good idea to 
  // check the CRC to make sure you didn't get garbage.  The order is 
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  
  // take one temperature measurement to throw away (it frequently is off)
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(1000);  
 }

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress, int idx)
{
  // method 1 - slower
#if ECHO_TO_SERIAL  
  Serial.print("Temp C: ");
  Serial.print(sensors.getTempC(deviceAddress));
  Serial.print(" Temp F: ");
  Serial.println(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit
#endif

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  if (idx == 0) lcd.setCursor(0,1);  // 2nd line, start
  if (idx == 1) lcd.setCursor(9,1);  // 2nd line, half way
  lcd.print(tempC);
  lcd.print(" C");

  if (tempC > max_temp[idx]) {
    max_temp[idx] = tempC;
  }
  if (tempC < min_temp[idx]) {
    min_temp[idx] = tempC;
  }
  avg_temp[idx] = avg_temp[idx] + tempC;
  avg_cnt[idx] = avg_cnt[idx] + 1;

}

void loop(void)
{ 
  int i,j;
  // delay for the amount of time we want between readings
  delay((LOG_INTERVAL -1) - (millis() % LOG_INTERVAL));
  // log milliseconds since starting
  uint32_t m = millis();

#if ECHO_TO_SERIAL  
  Serial.print(m);
  Serial.println(" in loop");
#endif

  // figure out the time
  DateTime now = RTC.now();
  hr = now.hour();
  // did we switch hours?
  if (lasthr != hr) {
    for (j = 0; j<MAXDEVICES; j++) {
      // housekeeping
      avg_temp[j] = avg_temp[j] / avg_cnt[j];

      if (j==INSIDE) {
        logfile.print("I, ");
      } else {
        logfile.print("O, ");
      }  
      // log this record
      logfile.print(m);           // milliseconds since start
      logfile.print(", ");    

      // log time
      logfile.print(now.unixtime()); // seconds since 2000
      logfile.print(", ");
      logfile.print(now.year(), DEC);
      logfile.print("/");
      logfile.print(now.month(), DEC);
      logfile.print("/");
      logfile.print(now.day(), DEC);
      logfile.print(" ");
      logfile.print(now.hour(), DEC);
      logfile.print(":");
      logfile.print(now.minute(), DEC);
      logfile.print(":");
      logfile.print(now.second(), DEC);
   
      // log temp data  
      logfile.print(", ");    
      logfile.print(min_temp[j]);
      logfile.print(", ");    
      logfile.print(max_temp[j]);
      logfile.print(", ");    
      logfile.println(avg_temp[j]);
      logfile.flush();

       min_temp[j] = 99;
       max_temp[j] = -99;
       avg_temp[j] = 0;
       avg_cnt[j] = 0;
    } // for j
    lasthr = hr;
  }
  
  // check if a key was pressed
  key = lcd.get_key();	       // read the value from the sensor & convert into key press
  if (key != oldkey)	       // if keypress is detected
  {
#if ECHO_TO_SERIAL  
    Serial.print(key);
    Serial.println(" keypress");
#endif
    delay(50);		        // wait for debounce time
    key = lcd.get_key();        // read the value from the sensor & convert into key press
    if (key != oldkey)				
    {			
      oldkey = key;
      if (key >=0) {
          lcd.clear();
          lcd.print("~ ");
          lcd.print(avg_temp[INSIDE]/avg_cnt[INSIDE]);
          lcd.print(" o~ ");
          lcd.print(avg_temp[OUTSIDE]/avg_cnt[OUTSIDE]);

          lcd.setCursor(0,1);  // second line
          lcd.print("> ");
          lcd.print(min_temp[OUTSIDE]);
          lcd.print(" < ");
          lcd.print(max_temp[OUTSIDE]);
          delay(5000);
      }
    }
  }
    
  lcd.clear();
  lcd.setCursor(0,0);   // first character, 1st line (count from 0); 
  // make a time string
  if (now.hour() < 10) {
   lcd.print('0');
  }
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) {
   lcd.print('0');
  }
  lcd.print(now.minute());
  lcd.print(':');
  if (now.second() < 10) {
   lcd.print('0');
  }
  lcd.print (now.second());

#if ECHO_TO_SERIAL  
  Serial.println(" request temperatures ");
#endif

  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  sensors.requestTemperatures(); // Send the command to get temperatures
  
  // It responds almost immediately. Let's print out the data
  printTemperature(Thermometer[INSIDE],INSIDE); // Use a simple function to print out the data
  printTemperature(Thermometer[OUTSIDE],OUTSIDE); // Use a simple function to print out the data
}

/*
// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) lcd.print("0");
    lcd.print(deviceAddress[i], HEX);
  }
}
*/

