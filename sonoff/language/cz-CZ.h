/*
  cz-CZ.h - localization for Czech with fonetick - Czech for Sonoff-Tasmota

  Copyright (C) 2018  Theo Arends (translated by vs)

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _LANGUAGE_CZ_CZ_H_
#define _LANGUAGE_CZ_CZ_H_

/*************************** ATTENTION *******************************\
 *
 * Due to memory constraints only UTF-8 is supported.
 * To save code space keep text as short as possible.
 * Time and Date provided by SDK can not be localized (yet).
 * Use online command StateText to translate ON, OFF, HOLD and TOGGLE.
 * Use online command Prefix to translate cmnd, stat and tele.
 *
\*********************************************************************/

//#define LANGUAGE_MODULE_NAME         // Enable to display "Module Generic" (ie Spanish), Disable to display "Generic Module" (ie English)

// "2017-03-07T11:08:02" - ISO8601:2004
#define D_YEAR_MONTH_SEPARATOR "-"
#define D_MONTH_DAY_SEPARATOR "-"
#define D_DATE_TIME_SEPARATOR "T"
#define D_HOUR_MINUTE_SEPARATOR ":"
#define D_MINUTE_SECOND_SEPARATOR ":"

#define D_DAY3LIST "PonÚteStřČtvPátSobNed"
#define D_MONTH3LIST "LedÚnoBřeDubKvěČerČevSrpZářŘíjLisPro"

// Non JSON decimal separator
#define D_DECIMAL_SEPARATOR ","

// Common
#define D_ADMIN "Admin"
#define D_AIR_QUALITY "Kvalita vzduchu"
#define D_AP "AP"                    // Access Point
#define D_AS "jako"
#define D_AUTO "AUTO"
#define D_BLINK "Blikání"
#define D_BLINKOFF "BlikáníVyp"
#define D_BOOT_COUNT "Počítadlo spuštění"
#define D_BRIGHTLIGHT "Světlý"
#define D_BUTTON "Tlačítko"
#define D_BY "by"                    // Written by me
#define D_BYTES "Bytu"
#define D_CELSIUS "Celsia"
#define D_CO2 "Kysličníku uhličitého"
#define D_CODE "kód"                 // Button code
#define D_COLDLIGHT "Studený"
#define D_COMMAND "Příkaz"
#define D_CONNECTED "...připojeno"
#define D_COUNT "Počítej"
#define D_COUNTER "Počítadlo"
#define D_CURRENT "Proud"          // As in Voltage and Current
#define D_DATA "Data"
#define D_DARKLIGHT "Tmavý"
#define D_DEBUG "Debug"
#define D_DISABLED "Zablokováno"
#define D_DNS_SERVER "Server DNS"
#define D_DONE "Provedeno"
#define D_DST_TIME "DST"
#define D_EMULATION "Emulace"
#define D_ENABLED "Otevřený"
#define D_ERASE "Smaž"
#define D_ERROR "Chyba"
#define D_FAHRENHEIT "Fahrenheita"
#define D_FAILED "chyba"
#define D_FALLBACK "Záložní"
#define D_FALLBACK_TOPIC "Záložní topic"
#define D_FALSE "Nepravda"
#define D_FILE "Soubor"
#define D_FREE_MEMORY "Volná paměť"
#define D_GAS "Gas"
#define D_GATEWAY "Výchozí brána"
#define D_GROUP "Skupina"
#define D_HOST "Server"
#define D_HOSTNAME "Název serveru"
#define D_HUMIDITY "Vlhkost"
#define D_ILLUMINANCE "Osvětlení"
#define D_IMMEDIATE "Mžikové"      // Button immediate
#define D_INDEX "Index"
#define D_INFO "Informace"
#define D_INITIALIZED "Inicializovaný"
#define D_IP_ADDRESS "Adresa IP"
#define D_LIGHT "Světlo"
#define D_LWT "LWT"
#define D_MODULE "Modul"
#define D_MQTT "MQTT"
#define D_MULTI_PRESS "několikeré-stisknutí"
#define D_NOISE "Hluk"
#define D_NONE "Není"
#define D_OFF "Vypnutý"
#define D_OFFLINE "Neaktivní"
#define D_OK "OK"
#define D_ON "Sepnutý"
#define D_ONLINE "Aktivní"
#define D_PASSWORD "Heslo"
#define D_PORT "Port"
#define D_POWER_FACTOR "Součinitel příkonu"
#define D_POWERUSAGE "Příkon"
#define D_PRESSURE "Tlak"
#define D_PRESSUREATSEALEVEL "Tlak na úrovni hladiny moře"
#define D_PROGRAM_FLASH_SIZE "Velikost flash programu"
#define D_PROGRAM_SIZE "Velikost programu"
#define D_PROJECT "Projekt"
#define D_RECEIVED "Obdržený"
#define D_RESTART "Restart"
#define D_RESTARTING "Restartování"
#define D_RESTART_REASON "Příčina restartu"
#define D_RESTORE "Načtený"
#define D_RETAINED "Uloženo"
#define D_SAVE "Ulož"
#define D_SENSOR "Sensor"
#define D_SSID "SSID"
#define D_START "Start"
#define D_STD_TIME "STD"
#define D_STOP "Stop"
#define D_SUBNET_MASK "Maska podsítě"
#define D_SUBSCRIBE_TO "Přihlaš se do"
#define D_SUCCESSFUL "úspěšné."
#define D_TEMPERATURE "Teplota"
#define D_TO "do"
#define D_TOGGLE "Přepni"
#define D_TOPIC "Topic"
#define D_TRANSMIT "Odešli"
#define D_TRUE "Pravda"
#define D_UPGRADE "aktualizace"
#define D_UPLOAD "Nahrání..."
#define D_UPTIME "Uptime"
#define D_USER "Uživatel"
#define D_UTC_TIME "UTC"
#define D_UV_LEVEL "úroveň UV"
#define D_VERSION "Verze"
#define D_VOLTAGE "Napětí"
#define D_WARMLIGHT "Ohřev"
#define D_WEB_SERVER "Web Server"

// sonoff.ino
#define D_WARNING_MINIMAL_VERSION "WARNING Tato verze nepodporuje trvalé nastavení"
#define D_LEVEL_10 "úroveň 1-0"
#define D_LEVEL_01 "úroveň 0-1"
#define D_SERIAL_LOGGING_DISABLED "Logování na sériovém portu ukončeno"
#define D_SYSLOG_LOGGING_REENABLED "Obnoven zápis do Syslog"

#define D_SET_BAUDRATE_TO "Nastavení rychlosti přenosu na"
#define D_RECEIVED_TOPIC "Obdržený topic"
#define D_DATA_SIZE "Velikost dat"
#define D_ANALOG_INPUT "Analogový vstup"

#define D_FINGERPRINT "Verifikuj otisk TLS..."
#define D_TLS_CONNECT_FAILED_TO "Nepřipojeno TLS do"
#define D_RETRY_IN "Zopakuji za"
#define D_VERIFIED "Ověřeno otisk "
#define D_INSECURE "Nesprávné připojení z důvodu chybného otisku TLS"
#define D_CONNECT_FAILED_TO "Spojení se nepodařilo navázat"

// support.ino
#define D_OSWATCH "osWatch"
#define D_BLOCKED_LOOP "Zablokování smyčky"
#define D_WPS_FAILED_WITH_STATUS "Chyba WPSconfig se statusem"
#define D_ACTIVE_FOR_3_MINUTES "aktivní 3 minuty"
#define D_FAILED_TO_START "nepodařilo se spustit"
#define D_PATCH_ISSUE_2186 "Chyba 2186"
#define D_CONNECTING_TO_AP "Připojování k AP"
#define D_IN_MODE "v módu"
#define D_CONNECT_FAILED_NO_IP_ADDRESS "Chyba připojení, nebyla obdržena IP adresa"
#define D_CONNECT_FAILED_AP_NOT_REACHED "Chyba připojení, nedostupný AP"
#define D_CONNECT_FAILED_WRONG_PASSWORD "Chyba připojení, nesprávné heslo pro AP"
#define D_CONNECT_FAILED_AP_TIMEOUT "Chyba připojení, uplynul AP timeout"
#define D_ATTEMPTING_CONNECTION "Připojování..."
#define D_CHECKING_CONNECTION "Zkouška spojení..."
#define D_QUERY_DONE "Vyslání dotazu. Nalezena služba MQTT"
#define D_MQTT_SERVICE_FOUND "Služba MQTT byla nalezena"
#define D_FOUND_AT "znalezeno v"
#define D_SYSLOG_HOST_NOT_FOUND "Syslog Host nebyl nalezen"

// settings.ino
#define D_SAVED_TO_FLASH_AT "Uloženo do paměti flash v"
#define D_LOADED_FROM_FLASH_AT "Staženo z paměti flash z"
#define D_USE_DEFAULTS "Použij defaultní hodnoty"
#define D_ERASED_SECTOR "Smazaný sektor"

// webserver.ino
#define D_MINIMAL_FIRMWARE_PLEASE_UPGRADE "Firmware MINIMÁLNÍ - prosím zaktualizujte"
#define D_WEBSERVER_ACTIVE_ON "Aktivní Web server"
#define D_WITH_IP_ADDRESS "na IP adrese"
#define D_WEBSERVER_STOPPED "Web server zastaven"
#define D_FILE_NOT_FOUND "Soubor nebyl nalezen"
#define D_REDIRECTED "Přesměrování na vlastní portál"
#define D_WIFIMANAGER_SET_ACCESSPOINT_AND_STATION "Wi-Fi manager nastaví AP a zachová STA"
#define D_WIFIMANAGER_SET_ACCESSPOINT "Wi-Fi manager nastaví Access Point"
#define D_TRYING_TO_CONNECT "Zkouším připojit zařízení k síti"

#define D_RESTART_IN "Restart"
#define D_SECONDS "sekund"
#define D_DEVICE_WILL_RESTART "Zařízení bude zrestartováno během několika sekund"
#define D_BUTTON_TOGGLE "Přepínač"
#define D_CONFIGURATION "Nastavení"
#define D_INFORMATION "Informace"
#define D_FIRMWARE_UPGRADE "Aktualizace firmware"
#define D_CONSOLE "Konzole"
#define D_CONFIRM_RESTART "Potvrzení restartu"

#define D_CONFIGURE_MODULE "Nastavení modulu"
#define D_CONFIGURE_WIFI "Nastavení WiFi"
#define D_CONFIGURE_MQTT "Nastavení MQTT"
#define D_CONFIGURE_DOMOTICZ "Nastavení Domoticz"
#define D_CONFIGURE_LOGGING "Nastavení logování"
#define D_CONFIGURE_OTHER "Jiná nastavení"
#define D_CONFIRM_RESET_CONFIGURATION "Potvrzení resetu nastavení"
#define D_RESET_CONFIGURATION "Reset nastavení"
#define D_BACKUP_CONFIGURATION "Záloha nastavení"
#define D_RESTORE_CONFIGURATION "Obnovení nastavení"
#define D_MAIN_MENU "Hlavní menu"

#define D_MODULE_PARAMETERS "Parametry modulu"
#define D_MODULE_TYPE "Typ modulu"
#define D_GPIO "GPIO"
#define D_SERIAL_IN "Serial In"
#define D_SERIAL_OUT "Serial Out"

#define D_WIFI_PARAMETERS "Parametry WiFi"
#define D_SCAN_FOR_WIFI_NETWORKS "Scan sítí WiFi"
#define D_SCAN_DONE "Scan dokončen"
#define D_NO_NETWORKS_FOUND "Síť nebyla nalezena"
#define D_REFRESH_TO_SCAN_AGAIN "Zopakovat scan"
#define D_DUPLICATE_ACCESSPOINT "Kopie AP"
#define D_SKIPPING_LOW_QUALITY "Přeskočení z důvodu kvality signálu"
#define D_RSSI "RSSI"
#define D_WEP "WEP"
#define D_WPA_PSK "WPA PSK"
#define D_WPA2_PSK "WPA2 PSK"
#define D_AP1_SSID "AP1 SSID"
#define D_AP1_PASSWORD "Heslo AP1"
#define D_AP2_SSID "AP2 SSID"
#define D_AP2_PASSWORD "Heslo AP2"

#define D_MQTT_PARAMETERS "Parametry MQTT"
#define D_CLIENT "Klient"
#define D_FULL_TOPIC "Celý topic"

#define D_LOGGING_PARAMETERS "Volby logování"
#define D_SERIAL_LOG_LEVEL "Seriová úroveň logu"
#define D_WEB_LOG_LEVEL "Webová úroveň logu"
#define D_SYS_LOG_LEVEL "Systemová úroveň logu"
#define D_MORE_DEBUG "Více debug informací"
#define D_SYSLOG_HOST "Syslog host"
#define D_SYSLOG_PORT "Syslog port"
#define D_TELEMETRY_PERIOD "Interval telemetrie"

#define D_OTHER_PARAMETERS "Jiné parametry"
#define D_WEB_ADMIN_PASSWORD "Heslo Web administrátora"
#define D_MQTT_ENABLE "MQTT aktivní"
#define D_FRIENDLY_NAME "Friendly Name"
#define D_BELKIN_WEMO "Belkin WeMo"
#define D_HUE_BRIDGE "Hue Bridge"
#define D_SINGLE_DEVICE "single device"
#define D_MULTI_DEVICE "multi device"

#define D_SAVE_CONFIGURATION "Ulož nastavení"
#define D_CONFIGURATION_SAVED "Nastavení uloženo"
#define D_CONFIGURATION_RESET "Nastavení resetováno"

#define D_PROGRAM_VERSION "Verze programu"
#define D_BUILD_DATE_AND_TIME "Datum a čas kompilace"
#define D_CORE_AND_SDK_VERSION "Verze Core/SDK"
#define D_FLASH_WRITE_COUNT "Počet zápisů do paměti"
#define D_MAC_ADDRESS "Adresa MAC"
#define D_MQTT_HOST "Host MQTT"
#define D_MQTT_PORT "Port MQTT"
#define D_MQTT_CLIENT "Klient MQTT"
#define D_MQTT_USER "Uživatel MQTT"
#define D_MQTT_TOPIC "Topic MQTT"
#define D_MQTT_GROUP_TOPIC "Topic skupiny MQTT"
#define D_MQTT_FULL_TOPIC "Celý topic MQTT"
#define D_MDNS_DISCOVERY "Získávání mDNS"
#define D_MDNS_ADVERTISE "Rozesílání mDNS"
#define D_ESP_CHIP_ID "ID systému ESP"
#define D_FLASH_CHIP_ID "ID systému paměti flash"
#define D_FLASH_CHIP_SIZE "Velikost flash"
#define D_FREE_PROGRAM_SPACE "Volné místo pro program"

#define D_UPGRADE_BY_WEBSERVER "Aktualizace z Web serveru"
#define D_OTA_URL "URL OTA"
#define D_START_UPGRADE "Start aktualizace"
#define D_UPGRADE_BY_FILE_UPLOAD "Aktualizace nahráním souboru"
#define D_UPLOAD_STARTED "Nahrávání zahájeno"
#define D_UPGRADE_STARTED "Zahájení aktualizace"
#define D_UPLOAD_DONE "Nahrávání ukončeno"
#define D_UPLOAD_ERR_1 "Soubor nebyl vybrán"
#define D_UPLOAD_ERR_2 "Málo místa"
#define D_UPLOAD_ERR_3 "Magický byte má hodnotu jinou než 0xE9"
#define D_UPLOAD_ERR_4 "Velikost programu je větší než skutečná velikost paměti flash"
#define D_UPLOAD_ERR_5 "Chyba nahrávání, nesouhlasí porovnávané bity"
#define D_UPLOAD_ERR_6 "Chyba nahrávání. Spuštěn zápis do logu na úrovni 3"
#define D_UPLOAD_ERR_7 "Nahrávání přerušeno"
#define D_UPLOAD_ERR_8 "Špatný soubor"
#define D_UPLOAD_ERR_9 "Soubor je příliš velký"
#define D_UPLOAD_ERROR_CODE "Chyba nahrávání"

#define D_ENTER_COMMAND "Vlož příkaz"
#define D_ENABLE_WEBLOG_FOR_RESPONSE "Zapni úroveň 2 zápisu Weblog, pokud je očekávána odpověď"
#define D_NEED_USER_AND_PASSWORD "Vyžadován uživatel=<username>&heslo=<password>"

// xdrv_wemohue.ino
#define D_MULTICAST_DISABLED "Multicast je vypnutý"
#define D_MULTICAST_REJOINED "Multicast opět připojený"
#define D_MULTICAST_JOIN_FAILED "Multicast neúspěšný"
#define D_FAILED_TO_SEND_RESPONSE "Nepodařilo se odeslat odpověď"

#define D_WEMO "WeMo"
#define D_WEMO_BASIC_EVENT "WeMo základní událost"
#define D_WEMO_EVENT_SERVICE "WeMo servisní událost"
#define D_WEMO_META_SERVICE "WeMo meta událost"
#define D_WEMO_SETUP "WeMo setup"
#define D_RESPONSE_SENT "Odpověď odeslána"

#define D_HUE "Hue"
#define D_HUE_BRIDGE_SETUP "Hue setup"
#define D_HUE_API_NOT_IMPLEMENTED "Hue API není implementováno"
#define D_HUE_API "Hue API"
#define D_HUE_POST_ARGS "Hue POST args"
#define D_3_RESPONSE_PACKETS_SENT "3 pakety odpovědi odeslány"

// xdrv_05_domoticz.ino
#define D_DOMOTICZ_PARAMETERS "Parametry Domoticz"
#define D_DOMOTICZ_IDX "Idx"
#define D_DOMOTICZ_KEY_IDX "Key idx"
#define D_DOMOTICZ_SWITCH_IDX "Spinac idx"
#define D_DOMOTICZ_SENSOR_IDX "Sensor idx"
  #define D_DOMOTICZ_TEMP "Temp"
  #define D_DOMOTICZ_TEMP_HUM "Temp,Vlhk"
  #define D_DOMOTICZ_TEMP_HUM_BARO "Temp,Vlhk,Tlak"
  #define D_DOMOTICZ_POWER_ENERGY "Příkon,Energie"
  #define D_DOMOTICZ_ILLUMINANCE "Osvětlení"
  #define D_DOMOTICZ_COUNT "Počítadlo"
  #define D_DOMOTICZ_VOLTAGE "Napětí"
  #define D_DOMOTICZ_CURRENT "Proud"
  #define D_DOMOTICZ_AIRQUALITY "AirQuality"
#define D_DOMOTICZ_UPDATE_TIMER "Aktualizace stopek"

// xdrv_03_energy.ino
#define D_ENERGY_TODAY "Energie Dnes"
#define D_ENERGY_YESTERDAY "Energie Včera"
#define D_ENERGY_TOTAL "Energie Součet"

// xsns_05_ds18b20.ino
#define D_SENSOR_BUSY "Sensor DS18x20 obsazen"
#define D_SENSOR_CRC_ERROR "Sensor DS18x20 chyba CRC"
#define D_SENSORS_FOUND "Nalezen sensor DS18x20"

// xsns_06_dht.ino
#define D_TIMEOUT_WAITING_FOR "Čekání na"
#define D_START_SIGNAL_LOW "signál, startovní nízký"
#define D_START_SIGNAL_HIGH "signál, startovní vysoký"
#define D_PULSE "puls"
#define D_CHECKSUM_FAILURE "Chybný kontrolní součet"

// xsns_07_sht1x.ino
#define D_SENSOR_DID_NOT_ACK_COMMAND "Sensor neobdržel pžíkaz ACK"
#define D_SHT1X_FOUND "SHT1X nalezen"

// xsns_18_pms5003.ino
#define D_STANDARD_CONCENTRATION "CF-1 PM"     // Standard Particle CF-1 Particle Matter
#define D_ENVIRONMENTAL_CONCENTRATION "PM"     // Environmetal Particle Matter
#define D_PARTICALS_BEYOND "Particals"

// sonoff_template.h
#define D_SENSOR_NONE     "Není"
#define D_SENSOR_DHT11    "DHT11"
#define D_SENSOR_AM2301   "AM2301"
#define D_SENSOR_SI7021   "SI7021"
#define D_SENSOR_DS18X20  "DS18x20"
#define D_SENSOR_I2C_SCL  "I2C SCL"
#define D_SENSOR_I2C_SDA  "I2C SDA"
#define D_SENSOR_WS2812   "WS2812"
#define D_SENSOR_IRSEND   "IRsend"
#define D_SENSOR_SWITCH   "Spínač"   // Suffix "1"
#define D_SENSOR_BUTTON   "Tlačít"   // Suffix "1"
#define D_SENSOR_RELAY    "Relé"    // Suffix "1i"
#define D_SENSOR_LED      "Led"      // Suffix "1i"
#define D_SENSOR_PWM      "PWM"      // Suffix "1",
#define D_SENSOR_COUNTER  "Počíta"  // Suffix "1"
#define D_SENSOR_IRRECV   "IRrecv"
#define D_SENSOR_MHZ_RX   "MHZ Rx"
#define D_SENSOR_MHZ_TX   "MHZ Tx"
#define D_SENSOR_PZEM_RX  "PZEM Rx"
#define D_SENSOR_PZEM_TX  "PZEM Tx"
#define D_SENSOR_SAIR_RX  "SAir Rx"
#define D_SENSOR_SAIR_TX  "SAir Tx"
#define D_SENSOR_SPI_CS   "SPI CS"
#define D_SENSOR_SPI_DC   "SPI DC"
#define D_SENSOR_BACKLIGHT "BkLight"
#define D_SENSOR_PMS5003  "PMS5003"
#define D_SENSOR_SDS0X1   "SDS0X1"

// Units
#define D_UNIT_AMPERE "A"
#define D_UNIT_HOUR "Hod"
#define D_UNIT_KILOOHM "kOhm"
#define D_UNIT_KILOWATTHOUR "kWh"
#define D_UNIT_LUX "lx"
#define D_UNIT_MICROGRAM_PER_CUBIC_METER "ug/m3"
#define D_UNIT_MICROMETER "um"
#define D_UNIT_MICROSECOND "us"
#define D_UNIT_MILLIAMPERE "mA"
#define D_UNIT_MILLISECOND "ms"
#define D_UNIT_MINUTE "Min"
#define D_UNIT_PARTS_PER_DECILITER "ppd"
#define D_UNIT_PARTS_PER_MILLION "ppm"
#define D_UNIT_PRESSURE "hPa"
#define D_UNIT_SECOND "sec"
#define D_UNIT_SECTORS "sektory"
#define D_UNIT_VOLT "V"
#define D_UNIT_WATT "W"
#define D_UNIT_WATTHOUR "Wh"

// Log message prefix
#define D_LOG_APPLICATION "APP: "  // Application
#define D_LOG_BRIDGE "BRG: "       // Bridge
#define D_LOG_CONFIG "CFG: "       // Settings
#define D_LOG_COMMAND "CMD: "      // Command
#define D_LOG_DEBUG "DBG: "        // Debug
#define D_LOG_DHT "DHT: "          // DHT sensor
#define D_LOG_DOMOTICZ "DOM: "     // Domoticz
#define D_LOG_DSB "DSB: "          // DS18xB20 sensor
#define D_LOG_HTTP "HTP: "         // HTTP webserver
#define D_LOG_I2C "I2C: "          // I2C
#define D_LOG_IRR "IRR: "          // Infra Red Received
#define D_LOG_LOG "LOG: "          // Logging
#define D_LOG_MODULE "MOD: "       // Module
#define D_LOG_MDNS "DNS: "         // mDNS
#define D_LOG_MQTT "MQT: "         // MQTT
#define D_LOG_OTHER "OTH: "        // Other
#define D_LOG_RESULT "RSL: "       // Result
#define D_LOG_RFR "RFR: "          // RF Received
#define D_LOG_SERIAL "SER: "       // Serial
#define D_LOG_SHT1 "SHT: "         // SHT1x sensor
#define D_LOG_UPLOAD "UPL: "       // Upload
#define D_LOG_UPNP "UPP: "         // UPnP
#define D_LOG_WIFI "WIF: "         // Wifi

#endif  // _LANGUAGE_CZ_CZ_H_
