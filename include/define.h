#ifndef __DEFINE_H
#define __DEFINE_H


#define firmware "ver20220409_1"
//"ver20220409_1" tu je len UPDATE knihoven ESPAsyncWebServer-esphome @^2.1.0   z  @^1.x.x   a  OTA na 2.2.6
//"ver20210208_1"  - u LEOSA rok 2021 az do 4/2022 potom davam "ver20220409_1"

typedef struct
{
	String Nazov;
	float Tset;
} ROOM_t;

#define WDT_TIMEOUT 5
#define PocetKaret 100
//EEPROM adrese

#define EE_IPadresa 00				//16bytes
#define EE_SUBNET 16				//16bytes
#define EE_Brana 32					//16bytes
#define EE_NazovSiete 48			//16bytes
#define EE_Heslosiete 64			//20bytes
#define EE_citacZapisuDoEEPORM 84	//2bytes
#define EE_citac2_ZapisuDoEEPORM 86 //2bytes
#define EE_dalsi 88
#define EE_zacateKaret_1 200
#define EE_zacateKaret_2 1300 //EE_zacateKaret + 100*11tj 1300

#endif