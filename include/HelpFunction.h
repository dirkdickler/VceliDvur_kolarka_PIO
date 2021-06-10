#include <Arduino.h>
#include"main.h"

void ESPinfo(void);
int getIpBlock(int index, String str);
String ipToString(IPAddress ip);
IPAddress str2IP(String str);
void OdosliStrankeVentilData(void);
int8_t NacitajEEPROM_setting(void);
String handle_Zadavanie_IP_setting();
void handle_Nastaveni(AsyncWebServerRequest *request);
char **split(char **argv, int *argc, char *string, const char delimiter, int allowempty);