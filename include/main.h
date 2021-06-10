#ifndef __MAIN_H
#define __MAIN_H

#include <Arduino.h>
#include "define.h"
#include <ESPAsyncWebServer.h>

typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef const uint32_t uc32; /*!< Read Only */
typedef const uint16_t uc16; /*!< Read Only */
typedef const uint8_t uc8;	 /*!< Read Only */

String ipToString(IPAddress ip);
IPAddress str2IP(String str);
int getIpBlock(int index, String str);
int8_t NacitajEEPROM_setting(void);
void Loop_10ms();
void Loop_1sek();
void Loop_10sek();
uint8_t ASCII2HEX(unsigned char b, unsigned char a);
#endif