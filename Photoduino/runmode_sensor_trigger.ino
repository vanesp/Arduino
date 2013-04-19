/*
 * This file is part of Photoduino.
 *
 * Photoduino is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Photoduino is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Photoduino.  If not, see <http://www.gnu.org/licenses/>.
 */
 
 // Run sensor trigger mode
void runAs_sensorTriggerMode() { 
       
   display_printTitle(MSG_RUNNING);
   
   keyboard_waitForNokey();

#ifndef PVE    
   attachInterrupt(0, keyboard_interrupts, CHANGE);
   attachInterrupt(1, keyboard_interrupts, CHANGE);
#else
   // use the local interrupt PCI0 vector
   PCMSK0 |= 0b00010100; // enable PC interrupts on arduino pins PCINT2 = PB2 = pin D10,  and PCINT4 = PB2 = D12, which are in group PCI0
   PCICR |= 1; // enable PCI0 pin group interrupt
#endif
  
   
   for(unsigned int cyclesCounter = 0; (cancelFlag==false && !(sensorTriggerMode_numCycles>0 && cyclesCounter >= sensorTriggerMode_numCycles));cyclesCounter++) { 
     
     // Normal shooting mode
     if (sensorTriggerMode_shootingMode == SHOOTINGMODE_NORMAL) {

       laser_turnOn();
       solenoidValve_makeDrops();
       
       sensor_waitFor(sensorTriggerMode_sensorType, 0);
       laser_turnOff();
       
       if(!cancelFlag) {
         
         camera_autofocusBegin(sensorTriggerMode_autofocusTime); 
         camera_shutterBegin(system_cameraShutterLag); 
          
         if (system_useFlash1) flash_shoot(sensorTriggerMode_preFlash1Time, PINS_FLASH1);
#ifndef PVE
         if (system_useFlash2) flash_shoot(sensorTriggerMode_preFlash2Time, PINS_FLASH2);
#endif 
       }
     }
     
     // Pre-bulb shooting mode
     if (sensorTriggerMode_shootingMode == SHOOTINGMODE_PREBULB) {
      
       camera_autofocusBegin(sensorTriggerMode_autofocusTime);
       camera_shutterBegin(system_cameraShutterLag); 
       laser_turnOn();
       solenoidValve_makeDrops();
       sensor_waitFor(sensorTriggerMode_sensorType, 0);
       laser_turnOff();
       
       if(!cancelFlag) {
         if (system_useFlash1) flash_shoot(sensorTriggerMode_preFlash1Time, PINS_FLASH1);
#ifndef PVE
         if (system_useFlash2) flash_shoot(sensorTriggerMode_preFlash2Time, PINS_FLASH2); 
#endif
       }
     }  
     
     // Mirror lock-up shooting mode
     if (sensorTriggerMode_shootingMode == SHOOTINGMODE_MIRRORLOCKUP) {
              
       for(boolean result = false; result == false;  ){      
         camera_mirrorLockUp(sensorTriggerMode_autofocusTime, system_cameraShutterLag);
         laser_turnOn();
         solenoidValve_makeDrops();
         result = sensor_waitFor(sensorTriggerMode_sensorType, system_cameraMirrorLockUpTimeout);
         laser_turnOff();
       }
       camera_shutterBegin(1); 
      
       if(!cancelFlag) {
         
         if (system_useFlash1) flash_shoot(sensorTriggerMode_preFlash1Time, PINS_FLASH1);
#ifndef PVE
         if (system_useFlash2) flash_shoot(sensorTriggerMode_preFlash2Time, PINS_FLASH2); 
#endif
       }          
     }
    
     // Common for all shooting modes
     camera_shutterEnd(sensorTriggerMode_preCloseTime); 
     camera_autofocusEnd();
     if(!cancelFlag) delay(sensorTriggerMode_interCycleTime);
      
     if(remoteMode && FirmataLite.available()) FirmataLite.processInput();
  
   }
   
   display_printAborting();
   keyboard_waitForNokey();
   
   detachInterrupt(0);
   detachInterrupt(1);
   
   laser_turnOn();
}
