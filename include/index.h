#ifndef __INDEX_H
#define __INDEX_H

const char zadavaci_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <meta charset="utf-8">
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
   <h1>Kolárka Včelí dvůr</h1>
  <form action="/Nastaveni">
    IPadresa: <input type="text" value=%s max="16" name="input1"><br><br>
	Maska: <input type="text" value=%s max="16" name="input2"><br><br>
	GateWay : <input type="text" value=%s max="16" name="input3"><br><br>
	Nazev site: <input type="text" value=%s max="16" name="input4"><br><br>
	Heslo site: <input type="text" value=%s max="16" name="input5"><br><br>
    <input type="submit" value="Uloz">
  </form>
</body></html>)rawliteral";

const char LenzobrazIP_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <meta charset="utf-8">
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
	<h1>Kolárka Včelí dvůr</h1>
  <form action="/Nastaveni">
    IPadresa: <input type="text" value=%s max="30" name="input1"><br><br>
	Maska: <input type="text" value=%s max="30" name="input2"><br><br>
	GateWay : <input type="text" value=%s max="30" name="input3"><br><br>
	Nazev site: <input type="text" value=%s max="30" name="input4"><br>
  </form>
</body></html>)rawliteral";

const char zadavaci_html_manual1[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <meta charset="utf-8">
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
   <h1>Kolárka Včelí dvůr -manuální zadaní RFID karet</h1>
  <form action="/Nastaveni">
    1: <input type="text" value=%s max="16" name="input1"><br><br>
	2: <input type="text" value=%s max="16" name="input2"><br><br>
	3: <input type="text" value=%s max="16" name="input3"><br><br>
	4: <input type="text" value=%s max="16" name="input4"><br><br>
	5: <input type="text" value=%s max="16" name="input5"><br><br>
    6: <input type="text" value=%s max="16" name="input6"><br><br>
	7: <input type="text" value=%s max="16" name="input7"><br><br>
	8: <input type="text" value=%s max="16" name="input8"><br><br>
	9: <input type="text" value=%s max="16" name="input9"><br><br>
   10: <input type="text" value=%s max="16" name="input10"><br><br>
    <input type="submit" value="Uloz">
  </form>
</body></html>)rawliteral";

#endif