//
// медицинский термометр
//

#include "LCD5110_SSVS.h"
#include "ds18b20minim.h"

LCD5110 LCD(8, 9, 10, 11); //CLK, DIN, DC, RST
extern uint8_t RusFont[];
extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];
extern uint8_t MediumNumbers[];
const byte st0 = 0;  // строка 1
const byte st1 = 8;  // строка 2
const byte st2 = 24; // строка 3
const byte st3 = 32; // строка 4
const byte st4 = 40; // строка 5

const byte p_beep = 7; // пин пищалки
const byte p_sele = 3; // пин переключателя режимов

float tt = 0; // собственно температура
float t0 = 0; // прошлая температура
float t_max = 0; // max температура
float t_min = 0; // min температура
const float t_cor = 0.1; // корректировка датчика
byte fix = 0; // фиксация температуры
byte f = 0; // первое чтение температуры
byte sele = 0; // режим работы (1 - градусник, 0 - термометр)
#define DS_PIN 2  // пин датчика


void setup() {
	attachInterrupt(1, res, CHANGE);    // прерывание на 3 пине (переключателя режимов) 
	digitalWrite(13, LOW);   // встроенный диод
	pinMode(p_beep, OUTPUT);
	pinMode(p_sele, INPUT);
	digitalWrite(p_beep, HIGH);
	LCD.InitLCD(65);          //запуск LCD контраст
	LCD.setFont(RusFont);
	LCD.clrScr();
	LCD.print("Uhflecybr", CENTER, st0);
	LCD.setFont(SmallFont);
	LCD.print("by", CENTER, st1);
	LCD.print("Andrew Mamohin", CENTER, st2);
	LCD.print("2021 v.1.0", CENTER, st3);
	LCD.setFont(RusFont);
	beep();
	if (digitalRead(p_sele)) sele = 1;
	delay(3000);
	LCD.clrScr();
}

void loop() {
	if (digitalRead(p_sele) != sele) {
		res();
	}
	if (sele) {
		gra();
	}
	else {
		term();
	}

}


void bprint(float t, boolean b ) {
 LCD.setFont(BigNumbers);
// LCD.clrRow(st0);
 LCD.invertText(b);
 // if (b) LCD.print("      ", LEFT, st0);
 LCD.printNumF(t, 1, CENTER, st0);
// LCD.invertText(false);
}

void bbprint(float t) {
	LCD.setFont(BigNumbers);
	LCD.clrRow(st0);
	LCD.printNumF(t, 2, CENTER, st0);
}

void mprint(float t, boolean b) {
	if (b) {
		LCD.setFont(MediumNumbers);
		// LCD.clrRow(st3);
		LCD.printNumF(t, 1, CENTER, st3);
	}
}

void sprint(float t1, float t2) {
 LCD.setFont(SmallFont);
 // LCD.clrRow(st4);
 LCD.printNumF(t1, 2, LEFT, st4);
 LCD.printNumF(t2, 2, RIGHT, st4);
}

void gra() {
 while (sele == 1) {
  dallas_requestTemp(DS_PIN); // запрос
  delay(1000);
  tt = dallas_getTemp(DS_PIN);
  if (fix < 4) {
 	 if (tt == t0) {
 		 fix++;
 		 if (fix == 3) {
 			 fix = 4;
 			 bprint(t0, 1);
 			 beep();
 		 }
 	 }
 	 else {
 		 fix = 0;
 	 }
 	 bprint(tt, 0);
 	 mprint(t0, f);
 	 t0 = tt;
	 f = 1;
  }
  else {
 	 mprint(tt, f);
  }
  delay(9000);
 }
}

void term() {
 while (sele == 0) {
	dallas_requestTemp(DS_PIN); // запрос
	delay(1000);
	tt = dallas_getTemp(DS_PIN);
	if (t_max == 0) t_max = tt;
	if (t_min == 0) t_min = tt;
	if (t_max < tt) t_max = tt;
	if (t_min > tt) t_min = tt;
	bbprint(tt);
	sprint(t_min, t_max);
	delay(9000);
 }
}

void res() { 
	sele = digitalRead(p_sele);
	t_max = tt;
	t_min = tt;
	fix = 0;
	t0 = 0;
	LCD.clrScr();
	return;
}

void beep() { 
	digitalWrite(p_beep, HIGH);
	delay(500);
	digitalWrite(p_beep, LOW);
}