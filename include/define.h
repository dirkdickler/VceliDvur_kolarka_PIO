#ifndef __DEFINE_H
#define __DEFINE_H

typedef struct
{
	String Nazov;
	float Tset;
} ROOM_t;

#define WDT_TIMEOUT 5
#define firmaware "ver20210514_02"
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