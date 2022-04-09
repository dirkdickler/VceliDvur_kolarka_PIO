

/*********
!!!!

$(DOCUMENTS_ROOT)/ArduinoData 

Pres Wrover nejde Serial2!!!   asi si to musis zmeniit na nejaky podla specifikacie Wrover modulu
C:\Users\seman\AppData\Local\Arduino15\packages\esp32\hardware  
!!!!!
GET /GetRFIDkarty1 HTTP/1.1<cr><lf>
GET /SetRFIDkarty1?0=1700AD85BC&1=1700AD85Ba HTTP/1.1<cr><lf> 
GET /GetRFIDkarty2 HTTP/1.1<cr><lf>
GET /SetRFIDkarty2?0=1700AD85BC&1=1700AD85Ba HTTP/1.1<cr><lf> 

ohladne wifi connect https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/#5

Nastavenie : wifi siet: hala  pass: 25252525   IP 192.168.1.10  SM:255.255.255.0  GW:192.168.1.1

*********/

#include <AsyncElegantOTA.h> //https://randomnerdtutorials.com/esp32-ota-over-the-air-arduino/#1-basic-elegantota
#include <elegantWebpage.h>

// Import required libraries
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino_JSON.h>
#include <esp_task_wdt.h>
#include "time.h"
#include <Ticker.h>
#include <EEPROM.h>
#include "index.h"
#include <ESP32Time.h>
#include <esp_spi_flash.h>
#include <SoftwareSerial.h>

#include "main.h"
#include "index.h"
#include "define.h"
#include "FS.h"
#include "Update.h"

const int Rele1 = 5;
const int Rele2 = 0;

SoftwareSerial mySerial; // RX, TX

const char *soft_ap_ssid = "RFID_Kolarka";
const char *soft_ap_password = "VceliDvur";
const char *ssid = "semiart";
const char *password = "aabbccddff";
char NazovSiete[30];
char Heslo[30];

const int output = 2;
String nacitaneRDIFkarta1;
String nacitaneRDIFkarta2;
uint32_t cnt;

const char *PARAM_INPUT = "value";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
Ticker timer_10ms(Loop_10ms, 10, 0, MILLIS);
Ticker timer_1sek(Loop_1sek, 1000, 0, MILLIS);
Ticker timer_10sek(Loop_10sek, 10000, 0, MILLIS);

u16_t CasovacOtvoreniZamku1 = 0;
u16_t CasovacOtvoreniZamku2 = 0;
JSONVar Karty1, Karty_2;
IPAddress local_IP(192, 168, 1, 11);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);	//optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600; //letny cas
struct tm timeinfo;
bool Casdostupny = false;

void FuncServer_On(void);
String func_manual1(AsyncWebServerRequest *request);
String func_manual2(AsyncWebServerRequest *request);

String handle_LenZobraz_IP_setting()
{
	char ttt[1000];
	char NazovSiete[56] = {"nazovSiete\0"};
	char ippadresa[56];
	char maskaIP[56];
	char brana[56];

	EEPROM.readString(EE_NazovSiete, NazovSiete, 16);
	IPAddress ip = WiFi.localIP();
	String stt = ipToString(ip);
	stt.toCharArray(ippadresa, 16);
	stt = ipToString(WiFi.subnetMask());
	stt.toCharArray(maskaIP, 16);
	stt = ipToString(WiFi.gatewayIP());
	stt.toCharArray(brana, 16);

	Serial.print("\r\nVyparsrovane IP: ");
	Serial.print(ippadresa);
	Serial.print("\r\nVyparsrovane MASKa: ");
	Serial.print(maskaIP);
	Serial.print("\r\nVyparsrovane Brana: ");
	Serial.print(brana);

	sprintf(ttt, LenzobrazIP_html, ippadresa, maskaIP, brana, NazovSiete);
	return ttt;
	//server.send (200, "text/html", ttt);
}

String handle_Zadavanie_IP_setting()
{
	char ttt[1000];
	char NazovSiete[56] = {"nazovSiete\0"};
	char HesloSiete[56] = {"HesloSiete\0"};
	char ippadresa[56];
	char maskaIP[56];
	char brana[56];

	IPAddress ip = WiFi.localIP();
	String stt = ipToString(ip);
	stt.toCharArray(ippadresa, 16);
	stt = ipToString(WiFi.subnetMask());
	stt.toCharArray(maskaIP, 16);
	stt = ipToString(WiFi.gatewayIP());
	stt.toCharArray(brana, 16);

	EEPROM.readString(EE_NazovSiete, NazovSiete, 16);
	Serial.print("\r\nNazov siete: ");
	Serial.print(NazovSiete);

	EEPROM.readString(EE_Heslosiete, HesloSiete, 16);
	Serial.print("\r\nHeslo siete: ");
	Serial.print(HesloSiete);

	Serial.print("\r\nVyparsrovane IP: ");
	Serial.print(ippadresa);
	Serial.print("\r\nVyparsrovane MASKa: ");
	Serial.print(maskaIP);
	Serial.print("\r\nVyparsrovane Brana: ");
	Serial.print(brana);

	sprintf(ttt, zadavaci_html, ippadresa, maskaIP, brana, NazovSiete, HesloSiete);
	//Serial.print ("\r\nToto je bufer pre stranky:\r\n");
	//Serial.print(ttt);

	return ttt;
}

void handle_Nastaveni(AsyncWebServerRequest *request)
{
	String inputMessage;
	String inputParam;
	Serial.println("Mam tu nastaveni");

	if (request->hasParam("input1"))
	{
		inputMessage = request->getParam("input1")->value();
		if (inputMessage.length() < 16)
		{
			EEPROM.writeString(EE_IPadresa, inputMessage);
		}
	}

	if (request->hasParam("input2"))
	{
		inputMessage = request->getParam("input2")->value();
		if (inputMessage.length() < 16)
		{
			EEPROM.writeString(EE_SUBNET, inputMessage);
		}
	}

	if (request->hasParam("input3"))
	{
		inputMessage = request->getParam("input3")->value();
		if (inputMessage.length() < 16)
		{
			EEPROM.writeString(EE_Brana, inputMessage);
		}
	}

	if (request->hasParam("input4"))
	{
		inputMessage = request->getParam("input4")->value();
		if (inputMessage.length() < 16)
		{
			EEPROM.writeString(EE_NazovSiete, inputMessage);
		}
	}

	if (request->hasParam("input5"))
	{
		inputMessage = request->getParam("input5")->value();
		if (inputMessage.length() < 20)
		{
			EEPROM.writeString(EE_Heslosiete, inputMessage);
		}
	}

	EEPROM.commit();
}

String handle_GetRFID_1()
{
	for (u8_t i = 0; i < PocetKaret; i++)
	{
		char aaa[12];
		memset(aaa, 0, 12);
		EEPROM.readBytes((i * 11) + EE_zacateKaret_1, aaa, 10); //EE_zacateKaret je offset kede zacianju karty
		Karty1[String(i)] = aaa;								//EEPROM.readString (i * 11);
	}

	String jsonString = JSON.stringify(Karty1);
	//Serial.print ("\r\nToto je je JSON string pri GetRFID:\r\n");
	//Serial.println (jsonString);
	return jsonString;
}

String handle_GetRFID_2()
{
	for (u8_t i = 0; i < PocetKaret; i++)
	{
		char aaa[12];
		memset(aaa, 0, 12);
		EEPROM.readBytes((i * 11) + EE_zacateKaret_2, aaa, 10); //EE_zacateKaret je offset kede zacianju karty
		Karty_2[String(i)] = aaa;								//EEPROM.readString (i * 11);
	}
	String jsonString = JSON.stringify(Karty_2);
	//Serial.print ("\r\nToto je je JSON string pri GetRFID2:\r\n");
	//Serial.println (jsonString);
	return jsonString;
}

String handle_SetRFID_1(AsyncWebServerRequest *request)
{
	u16_t PocetArgumentu = request->params();

	//Serial.print ("\r\nhandle_setRFID -Client posiela pocet karet: ");
	//Serial.println (PocetArgumentu);

	String jsonString;

	if (PocetArgumentu <= PocetKaret)
	{
		for (u8_t i = 0; i < PocetArgumentu; i++)
		{
			char buff[10];
			AsyncWebParameter *p = request->getParam(i);
			sprintf(buff, "%s", p->name().c_str());
			//Serial.printf ("\r\nCislo para:%u ma name:%s\r\n",i,buff);

			if (request->hasParam(buff)) // otestuj ic mam query tento parameter, inak by sa ti resetleo ESP
			{
				String par1 = request->getParam(buff)->value(); //server.arg (buff);
				//Serial.printf ("nkarta %s: %s\r\n", buff, par1);

				char aaa[12];
				memset(aaa, 0, sizeof(aaa));
				par1.toCharArray(aaa, 11);

				u8_t PozicevEEPROM = atoi(buff);
				Karty1[String(PozicevEEPROM)] = aaa;
				EEPROM.writeString((PozicevEEPROM * 11) + EE_zacateKaret_1, Karty1[String(PozicevEEPROM)]);
			}
		}

		//Serial.println ("\r\nKarty som poukldal a idem na inkrement");
		u16_t citac = EEPROM.readUShort(EE_citacZapisuDoEEPORM);
		citac++;
		EEPROM.writeUShort(EE_citacZapisuDoEEPORM, citac);
		EEPROM.commit();

		jsonString = JSON.stringify(Karty1);
	}
	else
	{
		jsonString = "Velky pocet argumentu v Query stringu!!  MAx pocet karet je 100!!!"; //PocetKaret
	}

	//Serial.print ("\r\nToto je je JSON stringpr SetRFID:\r\n");
	//Serial.println (jsonString);
	return jsonString;
}

String handle_SetRFID_2(AsyncWebServerRequest *request)
{
	u16_t PocetArgumentu = request->params();

	//Serial.print ("\r\nhandle_setRFID2 -Client posiela pocet karet: ");
	//Serial.println (PocetArgumentu);

	String jsonString;

	if (PocetArgumentu <= PocetKaret)
	{
		for (u8_t i = 0; i < PocetArgumentu; i++)
		{
			char buff[10];
			AsyncWebParameter *p = request->getParam(i);
			sprintf(buff, "%s", p->name().c_str());
			//Serial.printf ("\r\nCislo para:%u ma name:%s\r\n", i, buff);

			if (request->hasParam(buff)) // otestuj ic mam query tento parameter, inak by sa ti resetleo ESP
			{
				String par1 = request->getParam(buff)->value(); //server.arg (buff);
				//Serial.printf ("nkarta %s: %s\r\n", buff, par1);

				char aaa[12];
				memset(aaa, 0, sizeof(aaa));
				par1.toCharArray(aaa, 11);

				u8_t PozicevEEPROM = atoi(buff);
				Karty_2[String(PozicevEEPROM)] = aaa;
				EEPROM.writeString((PozicevEEPROM * 11) + EE_zacateKaret_2, Karty_2[String(PozicevEEPROM)]);
			}
		}

		//Serial.println ("\r\nKarty som poukldal a idem na inkrement");
		u16_t citac = EEPROM.readUShort(EE_citac2_ZapisuDoEEPORM);
		citac++;
		EEPROM.writeUShort(EE_citac2_ZapisuDoEEPORM, citac);
		EEPROM.commit();

		jsonString = JSON.stringify(Karty_2);
	}
	else
	{
		jsonString = "Velky pocet argumentu v Query stringu!!  MAx pocet karet je 100!!!"; //see PocetKaret
	}

	//Serial.print ("\r\nToto je je JSON stringpr SetRFID:\r\n");
	//Serial.println (jsonString);
	return jsonString;
}

/**********************************************************
 ***************        SETUP         **************
 **********************************************************/
void setup()
{
	pinMode(Rele1, OUTPUT);
	pinMode(Rele2, OUTPUT);
	digitalWrite(Rele1, LOW);
	digitalWrite(Rele2, LOW);

	Serial.begin(115200);
	Serial2.begin(9600); //PIN 16 je RX serial2    Pres Wrover nejde Serial2!!!   asi si to musis zmeniit na nejaky podla specifikacie Wrover modulu
	Serial.println("Booting Sketcik.044.");
	Serial2.println("Test Serial2 9600baud..");

	//prve cislo za SWSERIAL_8N1  je Rx pin
	mySerial.begin(9600, SWSERIAL_8N1, 15, 4, false, 64);

	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	Serial.println("\r\nHardware info");
	Serial.printf("%d cores Wifi %s%s\n",
				  chip_info.cores,
				  (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
				  (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
	Serial.printf("\r\nSilicon revision: %d\r\n ", chip_info.revision);
	Serial.printf("%dMB %s flash\r\n",
				  spi_flash_get_chip_size() / (1024 * 1024),
				  (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embeded" : "external");

	Serial.printf("\r\nTotal heap: %d\r\n", ESP.getHeapSize());
	Serial.printf("Free heap: %d\r\n", ESP.getFreeHeap());
	Serial.printf("Total PSRAM: %d\r\n", ESP.getPsramSize());
	Serial.printf("Free PSRAM: %d\r\n", ESP.getFreePsram()); // log_d("Free PSRAM: %d", ESP.getFreePsram());

	NacitajEEPROM_setting();

	WiFi.mode(WIFI_MODE_APSTA);
	Serial.println("Creating Accesspoint");
	WiFi.softAP(soft_ap_ssid, soft_ap_password, 7, 0, 3);
	//String Macc ="ahoj" + WiFi.macAddress();
	//char fff[30];
	//Serial.println ("Mac:"+Macc);
	//Macc.toCharArray (fff,21);

	//Serial.println ("Mac:" + Macc);
	//WiFi.softAP ( fff , soft_ap_password, 7, 0, 1);
	Serial.print("IP address:\t");
	Serial.println(WiFi.softAPIP());

	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
	{
		Serial.println("STA Failed to configure");
	}
	WiFi.begin(NazovSiete, Heslo);
	/*u8_t aa = 0;
  while (WiFi.status() != WL_CONNECTED && aa < 10) {
   delay(1000);
   Serial.println("Connecting to WiFi..");
   aa++;
  }*/
	u8_t aa = 0;
	while (WiFi.waitForConnectResult() != WL_CONNECTED && aa < 2)
	{
		Serial.print(".");
		aa++;
	}
	// Print ESP Local IP Address
	Serial.println(WiFi.localIP());

	FuncServer_On();

	AsyncElegantOTA.begin(&server, "admin", "VceliDvur");
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	getLocalTime(&timeinfo);

	server.begin();
	timer_10ms.start();
	timer_1sek.start();
	timer_10sek.start();

	esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
	esp_task_wdt_add(NULL);				  //add current thread to WDT watch
}

/**********************************************************
 ***************       LOOP                **************
 **********************************************************/
void loop()
{
	timer_10ms.update();
	timer_1sek.update();
	timer_10sek.update();
	//AsyncElegantOTA.loop();
	esp_task_wdt_reset();
}

String ipToString(IPAddress ip)
{
	String s = "";
	for (int i = 0; i < 4; i++)
		s += i ? "." + String(ip[i]) : String(ip[i]);
	return s;
}

IPAddress str2IP(String str)
{

	IPAddress ret(getIpBlock(0, str), getIpBlock(1, str), getIpBlock(2, str), getIpBlock(3, str));
	return ret;
}

int getIpBlock(int index, String str)
{
	char separator = '.';
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = str.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
		if (str.charAt(i) == separator || i == maxIndex)
		{
			found++;
			strIndex[0] = strIndex[1] + 1;
			strIndex[1] = (i == maxIndex) ? i + 1 : i;
		}
	}

	return found > index ? str.substring(strIndex[0], strIndex[1]).toInt() : 0;
}

int8_t NacitajEEPROM_setting(void)
{

	if (!EEPROM.begin(2500))
	{
		Serial.println("Failed to initialise EEPROM");
		return -1;
	}
	else
	{
		Serial.println("Succes to initialise EEPROM");

		for (u8_t i = 0; i < PocetKaret; i++)
		{
			char aaa[202];
			memset(aaa, 0, sizeof(aaa));
			EEPROM.readBytes((i * 11) + EE_zacateKaret_1, aaa, 10);
			Karty1[String(i)] = aaa;
			EEPROM.readBytes((i * 11) + EE_zacateKaret_2, aaa, 10);
			Karty_2[String(i)] = aaa;
		}
	}
	String apipch = EEPROM.readString(EE_IPadresa); // "192.168.1.11";
	local_IP = str2IP(apipch);

	apipch = EEPROM.readString(EE_SUBNET);
	subnet = str2IP(apipch);

	apipch = EEPROM.readString(EE_Brana);
	gateway = str2IP(apipch);

	memset(NazovSiete, 0, sizeof(NazovSiete));
	memset(Heslo, 0, sizeof(Heslo));
	EEPROM.readBytes(EE_NazovSiete, NazovSiete, 16);
	EEPROM.readBytes(EE_Heslosiete, Heslo, 20);
	Serial.printf("Nacitany nazov siete a heslo z EEPROM: %s  a %s\r\n", NazovSiete, Heslo);
	return 0;
}

void Loop_10ms()
{
	static uint8_t TimeOut_RXdata = 0;	 //musi byt static lebo sem skaces z Loop
	static uint16_t KolkkoNplnenych = 0; //musi byt static lebo sem skaces z Loop
	static char budd[100];				 //musi byt static lebo sem skaces z Loop

	uint16_t aktualny;

	aktualny = Serial2.available();
	if (aktualny)
	{

		//Serial2.readBytes (temp, aktualny);
		for (uint16_t i = 0; i < aktualny; i++)
		{
			budd[KolkkoNplnenych + i] = Serial2.read();
		}
		KolkkoNplnenych += aktualny;
		TimeOut_RXdata = 5;
	}

	if (TimeOut_RXdata)
	{
		if (--TimeOut_RXdata == 0)
		{
			{
				Serial.printf("[Serial 2] doslo:%u a to %s\n", KolkkoNplnenych, budd);
				if (KolkkoNplnenych == 16)
				{
					char RFIDko[15];
					char kod[5];
					u16_t sumaa;
					kod[0] = ASCII2HEX(budd[1], budd[2]);
					kod[1] = ASCII2HEX(budd[3], budd[4]);
					kod[2] = ASCII2HEX(budd[5], budd[6]);
					kod[3] = ASCII2HEX(budd[7], budd[8]);
					kod[4] = ASCII2HEX(budd[9], budd[10]);
					sumaa = ASCII2HEX(budd[11], budd[12]);

					if ((kod[0] ^ kod[1] ^ kod[2] ^ kod[3] ^ kod[4] ^ sumaa) == 0)
					{
						Serial.println("[Serial 2] Suma sedi s Kodem karty- paradaaa!!!u\n");
						for (u8_t i = 0; i < 10; i++)
						{
							RFIDko[i] = budd[i + 1];
						}
						RFIDko[10] = 0;
						Serial.printf("[Serial 2] Id karty je:%s\r\n", RFIDko);
						nacitaneRDIFkarta1 = String(RFIDko);
						Serial.print("[Serial 2] Idem skontrolovat ci mam kartu:");
						Serial.println(nacitaneRDIFkarta1);
						for (u8_t i = 0; i < PocetKaret; i++)
						{
							//Serial.print ("\r\nKotrolujem kartu:" + String (i) + " s ID:");
							//Serial.print (Karty1[String (i)]);

							if (nacitaneRDIFkarta1 == Karty1[String(i)] || nacitaneRDIFkarta1 == "010717239B" || nacitaneRDIFkarta1 == "01053B090E")
							{
								Serial.println("\r\n[Serial 2] Supeeer nasel som karu !!!");
								Serial.println("[Serial 2] Otvaram zamork 1 !!!");
								CasovacOtvoreniZamku1 = 5;
								digitalWrite(Rele1, HIGH);
								break;
							}
						}
					}
					else
					{
						Serial.println("[Serial 2] Suma RFID ERROR !!!");
					}
				}

				memset(budd, 0, sizeof(budd));
				KolkkoNplnenych = 0;
			}
		}
	}

	static uint8_t TimeOut_RXdata_softUart = 0;	  //musi byt static lebo sem skaces z Loop
	static uint16_t KolkkoNplnenych_softUart = 0; //musi byt static lebo sem skaces z Loop
	static char budd_softUart[100];				  //musi byt static lebo sem skaces z Loop

	aktualny = mySerial.available();
	if (aktualny)
	{
		for (uint16_t i = 0; i < aktualny; i++)
		{
			budd_softUart[KolkkoNplnenych_softUart + i] = mySerial.read(); // temp_softUart[i];
		}
		KolkkoNplnenych_softUart += aktualny;
		TimeOut_RXdata_softUart = 5;
	}

	if (TimeOut_RXdata_softUart)
	{
		if (--TimeOut_RXdata_softUart == 0)
		{
			{
				Serial.printf("[softSerial]  doslo:%u a to %s\n", KolkkoNplnenych_softUart, budd_softUart);
				if (KolkkoNplnenych_softUart == 16)
				{
					char RFIDko[15];
					char kod[5];
					u16_t sumaa;
					kod[0] = ASCII2HEX(budd_softUart[1], budd_softUart[2]);
					kod[1] = ASCII2HEX(budd_softUart[3], budd_softUart[4]);
					kod[2] = ASCII2HEX(budd_softUart[5], budd_softUart[6]);
					kod[3] = ASCII2HEX(budd_softUart[7], budd_softUart[8]);
					kod[4] = ASCII2HEX(budd_softUart[9], budd_softUart[10]);
					sumaa = ASCII2HEX(budd_softUart[11], budd_softUart[12]);

					if ((kod[0] ^ kod[1] ^ kod[2] ^ kod[3] ^ kod[4] ^ sumaa) == 0)
					{
						Serial.println("[softSerial] Suma sedi s Kodem karty- paradaaa!!!u\n");
						for (u8_t i = 0; i < 10; i++)
						{
							RFIDko[i] = budd_softUart[i + 1];
						}
						RFIDko[10] = 0;
						Serial.printf("[softSerial] Id karty je:%s\r\n", RFIDko);
						nacitaneRDIFkarta2 = String(RFIDko);
						Serial.print("[softSerial] Idem skontrolovat ci mam kartu:");
						Serial.println(nacitaneRDIFkarta2);
						for (u8_t i = 0; i < PocetKaret; i++)
						{
							//Serial.print ("\r\nKotrolujem kartu:" + String (i) + " s ID:");
							//Serial.print (Karty_2[String (i)]);

							if (nacitaneRDIFkarta2 == Karty_2[String(i)] || nacitaneRDIFkarta2 == "010717239B" || nacitaneRDIFkarta2 == "01053B090E")
							{
								Serial.println("\r\n[softSerial] Supeeer nasel som karu !!!");
								Serial.println("[softSerial] Otvaram zamork 2 !!!");
								CasovacOtvoreniZamku2 = 5;
								digitalWrite(Rele2, HIGH);
								break;
							}
						}
					}
					else
					{
						Serial.println("[softSerial]  Suma RFID ERROR !!!");
					}
				}

				memset(budd_softUart, 0, sizeof(budd_softUart));
				KolkkoNplnenych_softUart = 0;
			}
		}
	}
}

void Loop_1sek()
{
	cnt++;

	Serial.printf("\r\nCitac je:%d\r\n", cnt);
	if (CasovacOtvoreniZamku1)
	{
		if (--CasovacOtvoreniZamku1 == 0)
		{
			Serial.println("Vypinam Zamek 1");
			digitalWrite(Rele1, LOW);
		}
	}
	else
	{
		digitalWrite(Rele1, LOW);
	}

	if (CasovacOtvoreniZamku2)
	{
		if (--CasovacOtvoreniZamku2 == 0)
		{
			Serial.println("Vypinam Zamek 2");
			digitalWrite(Rele2, LOW);
		}
	}
	else
	{
		digitalWrite(Rele2, LOW);
	}
}

void Loop_10sek(void)
{
	static u8_t loc_cnt = 0;
	Serial.println("Mam Loop 10 sek............");

	Serial.print("Wifi status:");
	Serial.println(WiFi.status());

	if (WiFi.status() != WL_CONNECTED)
	{
		loc_cnt++;
	}
	else
	{
		loc_cnt = 0;
	}

	if (loc_cnt == 2)
	{
		Serial.println("[10sek] Odpajam WIFI, lebo wifi nieje: WL_CONNECTED ");
		WiFi.disconnect(1, 1);
	}

	if (loc_cnt == 3)
	{
		loc_cnt = 255;
		WiFi.mode(WIFI_MODE_APSTA);
		Serial.println("znovu -Creating Accesspoint");
		WiFi.softAP(soft_ap_ssid, soft_ap_password, 7, 0, 3);

		if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
		{
			Serial.println("STA Failed to configure");
		}
		Serial.println("znovu -Wifi begin");
		WiFi.begin(NazovSiete, Heslo);
		u8_t aa = 0;
		while (WiFi.waitForConnectResult() != WL_CONNECTED && aa < 2)
		{
			Serial.print(".");
			aa++;
		}
	}

	//ak je Wifi connect tak tu hore nulujem a tak sa loc_cnt rovna NULA a potom teda idem nacitat RTO
	if (loc_cnt == 0)
	{
		if (!getLocalTime(&timeinfo, 1000))
		{
			Serial.println("Failed to obtain time");
			Casdostupny = false;
		}
		else
		{
			Serial.println("Nacitanie casu OK");
			Casdostupny = true;
		}
	}
}

uint8_t ASCII2HEX(unsigned char b, unsigned char a)
{

	//pri volani tejto funkcie se musel otocit b,a protoze to zacalo po urave FW zobrazovat a posielat na net opacne
	//napa AB to poslalo ako BA, nechapem
	//
	a = a - 0x30;
	b = b - 0x30;
	if (a == 0x11)
	{
		a = 0xa0;
	}
	else if (a == 0x12)
	{
		a = 0xb0;
	}
	else if (a == 0x13)
	{
		a = 0xc0;
	}
	else if (a == 0x14)
	{
		a = 0xd0;
	}
	else if (a == 0x15)
	{
		a = 0xe0;
	}
	else if (a == 0x16)
	{
		a = 0xf0;
	}
	else if (a)
	{
		a = a * 0x10;
	}
	else
		a = 0;

	if (b == 0x11)
	{
		b = 0x0a;
	}
	else if (b == 0x12)
	{
		b = 0x0b;
	}
	else if (b == 0x13)
	{
		b = 0x0c;
	}
	else if (b == 0x14)
	{
		b = 0x0d;
	}
	else if (b == 0x15)
	{
		b = 0x0e;
	}
	else if (b == 0x16)
	{
		b = 0x0f;
	}

	return a + b;
}

String func_manual1(AsyncWebServerRequest *request)
{
	char ttt[1200];
	char NazovSiete[30] = {"nazovSiete\0"};

	Serial.println("\r\nfunc_manual1 ide naplnit ttt string");

	sprintf(ttt, zadavaci_html_manual1, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete);
	//Serial.print ("\r\nToto je bufer pre stranky:\r\n");
	//Serial.print(ttt);

	return ttt;
}

String func_manual2(AsyncWebServerRequest *request)
{

	char ttt[1200];
	char NazovSiete[30] = {"nazovffS\0"};

	Serial.println("\r\nfunc_manual2 ide naplnit ttt string");

	sprintf(ttt, zadavaci_html_manual1, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete, NazovSiete);
	//Serial.print ("\r\nToto je bufer pre stranky:\r\n");
	//Serial.print(ttt);

	return ttt;
}

void FuncServer_On(void)
{
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/html", handle_LenZobraz_IP_setting()); });

	server.on("/nastavip", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (!request->authenticate("admin", "VceliDvur"))
					  return request->requestAuthentication();
				  request->send(200, "text/html", handle_Zadavanie_IP_setting());
			  });

	server.on("/Nastaveni", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  handle_Nastaveni(request);
				  request->send(200, "text/html", "Nastavujem a ukladam do EEPROM");
				  Serial.println("Idem resetovat ESP");
				  delay(2000);
				  esp_restart();
			  });

	server.on("/GetRFIDkarty1", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/html", handle_GetRFID_1()); });

	server.on("/GetRFIDkarty2", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/html", handle_GetRFID_2()); });

	server.on("/SetRFIDkarty1", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/html", handle_SetRFID_1(request)); });

	server.on("/SetRFIDkarty2", HTTP_GET, [](AsyncWebServerRequest *request)
			  { request->send(200, "text/html", handle_SetRFID_2(request)); });

	server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  char ttt[500];
				  u16_t citac = EEPROM.readUShort(EE_citacZapisuDoEEPORM);
				  u16_t citac2 = EEPROM.readUShort(EE_citac2_ZapisuDoEEPORM);

				  char loc_buf[60]= {0,};
				  
				  if (Casdostupny == true)
				  {
					  strftime(loc_buf, sizeof(loc_buf), " %H:%M:%S    %d.%m.%Y    ", &timeinfo);
				  }
				  else
				  {
					  sprintf(loc_buf, "Online cas nedostupny!!");
				  }

				  char loc_buf1[30];
				  char loc_buf2[30]; 
				  nacitaneRDIFkarta1.toCharArray(loc_buf1,30,0),
				  nacitaneRDIFkarta2.toCharArray(loc_buf2,30,0),

				  sprintf(ttt, "Firmware :%s<br>Pocet zapisov do EEPROM 1: %u EEPROM 2: %u <br>"
							   "Zamok 1:%u zostava cas:%u sek<br>"
							   "Zamok 2:%u zostava cas:%u sek<br>"
							   "Posledne nacitana karta 1:%s<br>"
							   "Posledne nacitana karta 2:%s<br>Citac:%u<br>"
							   "RTC:%s",
						  firmware, citac, citac2,
						  digitalRead(Rele1), CasovacOtvoreniZamku1,
						  digitalRead(Rele2), CasovacOtvoreniZamku2,
						  loc_buf1, loc_buf2, cnt, loc_buf);

				  request->send(200, "text/html", ttt);
			  });

	server.on("/otvorzamok1", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (!request->authenticate("admin", "radecek78"))
					  return request->requestAuthentication();
				  CasovacOtvoreniZamku1 = 5;
				  digitalWrite(Rele1, HIGH);
				  request->send(200, "text/html", "Nahodeny zamek cislo 1 na 5 sekund");
			  });

	server.on("/otvorzamok2", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (!request->authenticate("admin", "radecek78"))
					  return request->requestAuthentication();
				  CasovacOtvoreniZamku2 = 5;
				  digitalWrite(Rele2, HIGH);
				  request->send(200, "text/html", "Nahodeny zamekcislo 2 na 5 sekund");
			  });

	server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (!request->authenticate("admin", "radecek78"))
					  return request->requestAuthentication();

				  request->send(200, "text/html", "resetujem!!!");
				  delay(1000);
				  esp_restart();
			  });

	server.on("/manual1", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (!request->authenticate("admin", "VceliDvur"))
					  return request->requestAuthentication();
				  request->send(200, "text/html", func_manual1(request));
			  });

	server.on("/manual2", HTTP_GET, [](AsyncWebServerRequest *request)
			  {
				  if (!request->authenticate("admin", "VceliDvur"))
					  return request->requestAuthentication();
				  request->send(200, "text/html", func_manual2(request));
			  });
}