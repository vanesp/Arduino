/*
  xdrv_07_home_assistant.ino - home assistant support for Sonoff-Tasmota

  Copyright (C) 2018  Theo Arends

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

#ifdef USE_HOME_ASSISTANT

const char HASS_DISCOVER_SWITCH[] PROGMEM =
  "{\"name\":\"%s\","                              // dualr2 1
  "\"command_topic\":\"%s\","                      // cmnd/dualr2/POWER2
  "\"state_topic\":\"%s\","                        // stat/dualr2/RESULT  (implies "\"optimistic\":\"false\",")
  "\"value_template\":\"{{value_json.%s}}\","      // POWER2
  "\"payload_off\":\"%s\","                        // OFF
  "\"payload_on\":\"%s\","                         // ON
//  "\"optimistic\":\"false\","                    // false is Hass default when state_topic is set
  "\"availability_topic\":\"%s\","                 // tele/dualr2/LWT
  "\"payload_available\":\"" D_ONLINE "\","        // Online
  "\"payload_not_available\":\"" D_OFFLINE "\"";   // Offline

const char HASS_DISCOVER_LIGHT_DIMMER[] PROGMEM =
  "%s,\"brightness_command_topic\":\"%s\","        // cmnd/led2/Dimmer
  "\"brightness_state_topic\":\"%s\","             // stat/led2/RESULT
  "\"brightness_scale\":100,"                      // 100%
  "\"on_command_type\":\"brightness\","            // power on (first), power on (last), no power on (brightness)
  "\"brightness_value_template\":\"{{value_json." D_CMND_DIMMER "}}\"";

const char HASS_DISCOVER_LIGHT_COLOR[] PROGMEM =
  "%s,\"rgb_command_topic\":\"%s\","               // cmnd/led2/Color
  "\"rgb_state_topic\":\"%s\","                    // stat/led2/RESULT
  "\"rgb_value_template\":\"{{value_json." D_CMND_COLOR "}}\"";
//  "\"rgb_value_template\":\"{{value_json." D_CMND_COLOR " | join(',')}}\"";

const char HASS_DISCOVER_LIGHT_CT[] PROGMEM =
  "%s,\"color_temp_command_topic\":\"%s\","        // cmnd/led2/CT
  "\"color_temp_state_topic\":\"%s\","             // stat/led2/RESULT
  "\"color_temp_value_template\":\"{{value_json." D_CMND_COLORTEMPERATURE "}}\"";
/*
const char HASS_DISCOVER_LIGHT_SCHEME[] PROGMEM =
  "%s,\"effect_command_topic\":\"%s\","            // cmnd/led2/Scheme
  "\"effect_state_topic\":\"%s\","                 // stat/led2/RESULT
  "\"effect_value_template\":\"{{value_json." D_CMND_SCHEME "}}\","
  "\"effect_list\":\"[0, 1, 2, 3, 4]\"";           // Needs to be a Python string list providing Scheme parameter values (Unable to get this functional)
*/
/*
#1690 - investigate
effect_list:
- 0
- 1
- 2
- 3
- 4
- 5
- 6
- 7
- 8
- 9
- 10
- 11
- 12
*/
void HAssDiscovery()
{
  char sidx[8];
  char stopic[TOPSZ];
  bool is_light = false;

  // Configure Tasmota for default Home Assistant parameters to keep discovery message as short as possible
  if (Settings.flag.hass_discovery) {
    Settings.flag.mqtt_response = 0;     // Response always as RESULT and not as uppercase command
    Settings.flag.decimal_text = 1;      // Respond with decimal color values
//    Settings.light_scheme = 0;           // To just control color it needs to be Scheme 0
//    strncpy_P(Settings.mqtt_fulltopic, PSTR("%prefix%/%topic%/"), sizeof(Settings.mqtt_fulltopic));  // Make MQTT topic as short as possible to make this process posible within MQTT_MAX_PACKET_SIZE
  }

  for (int i = 1; i <= devices_present; i++) {
    is_light = ((i == devices_present) && (light_type));

    mqtt_data[0] = '\0';

    snprintf_P(sidx, sizeof(sidx), PSTR("_%d"), i);
    // Clear "other" topic first in case the device has been reconfigured
    snprintf_P(stopic, sizeof(stopic), PSTR(HOME_ASSISTANT_DISCOVERY_PREFIX "/%s/%s%s/config"), (is_light) ? "switch" : "light", mqtt_topic, sidx);
    MqttPublish(stopic, true);
    snprintf_P(stopic, sizeof(stopic), PSTR(HOME_ASSISTANT_DISCOVERY_PREFIX "/%s/%s%s/config"), (is_light) ? "light" : "switch", mqtt_topic, sidx);

    if (Settings.flag.hass_discovery) {
      char name[33];
      char value_template[33];
      char command_topic[TOPSZ];
      char state_topic[TOPSZ];
      char availability_topic[TOPSZ];

      if (i > MAX_FRIENDLYNAMES) {
        snprintf_P(name, sizeof(name), PSTR("%s %d"), Settings.friendlyname[0], i);
      } else {
        snprintf_P(name, sizeof(name), Settings.friendlyname[i -1]);
      }
      GetPowerDevice(value_template, i, sizeof(value_template));
      GetTopic_P(command_topic, CMND, mqtt_topic, value_template);
      GetTopic_P(state_topic, STAT, mqtt_topic, S_RSLT_RESULT);
      GetTopic_P(availability_topic, TELE, mqtt_topic, S_LWT);
      snprintf_P(mqtt_data, sizeof(mqtt_data), HASS_DISCOVER_SWITCH, name, command_topic, state_topic, value_template, Settings.state_text[0], Settings.state_text[1], availability_topic);

      if (is_light) {
        char brightness_command_topic[TOPSZ];

        GetTopic_P(brightness_command_topic, CMND, mqtt_topic, D_CMND_DIMMER);
        snprintf_P(mqtt_data, sizeof(mqtt_data), HASS_DISCOVER_LIGHT_DIMMER, mqtt_data, brightness_command_topic, state_topic);

        if (light_subtype >= LST_RGB) {
          char rgb_command_topic[TOPSZ];

          GetTopic_P(rgb_command_topic, CMND, mqtt_topic, D_CMND_COLOR);
          snprintf_P(mqtt_data, sizeof(mqtt_data), HASS_DISCOVER_LIGHT_COLOR, mqtt_data, rgb_command_topic, state_topic);
/*
          char effect_command_topic[TOPSZ];

          GetTopic_P(effect_command_topic, CMND, mqtt_topic, D_CMND_SCHEME);
          snprintf_P(mqtt_data, sizeof(mqtt_data), HASS_DISCOVER_LIGHT_SCHEME, mqtt_data, effect_command_topic, state_topic);
*/
        }
        if ((LST_COLDWARM == light_subtype) || (LST_RGBWC == light_subtype)) {
          char color_temp_command_topic[TOPSZ];

          GetTopic_P(color_temp_command_topic, CMND, mqtt_topic, D_CMND_COLORTEMPERATURE);
          snprintf_P(mqtt_data, sizeof(mqtt_data), HASS_DISCOVER_LIGHT_CT, mqtt_data, color_temp_command_topic, state_topic);
        }
      }
      snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s}"), mqtt_data);
    }
    MqttPublish(stopic, true);
  }
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

#define XDRV_07

boolean Xdrv07(byte function)
{
  boolean result = false;

  if (Settings.flag.mqtt_enabled) {
    switch (function) {
      case FUNC_MQTT_INIT:
        HAssDiscovery();
        break;
    }
  }
  return result;
}

#endif  // USE_HOME_ASSISTANT
