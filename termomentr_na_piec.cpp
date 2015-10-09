/*
Ta wersja dotyczy rejestrów podłączonych jeden po drugim poprzez pin 9
 
Versjon 1.0
 */
//do trasminisji RF
#include <Manchester.h>

#include <OneWire.h>
#include <DallasTemperature.h>
// DS18B20 definicja pinu
#define ONE_WIRE_BUS 5

//ds18b20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Obsługa rejestru 74hc595
int clockPin = 6;
int dataPin = 8;
int latchPin = 7;


//do RF
#define TX_PIN 9  //pin where your transmitter is connected



int n = 1;
int k = 1;

//tablica liczby od 0 do 9 oraz 10 - zapalony element G ( - ), 11 - wszystkie elementy zgaszone
// wersja prawidłowa
//byte dec_digits[] = {0b11000000,0b11111001,0b10100100,0b10110000,0b10011001,0b10010010,0b10000010,0b11111000,0b10000000,0b10010000, 0b10111111, 0b11111111 };

//wersja moja zmienione piny i bity
byte dec_digits[] = {0b11000000,0b11111010,0b10100100,0b10110000,0b10011010,0b10010001,0b10000001,0b11111000,0b10000000,0b10010000, 0b10111111, 0b11111111 };

void setup() {
	//Obsługa RF trasminisji
	man.workAround1MhzTinyCore(); //add this in order for transmitter to work with 1Mhz Attiny85/84
	man.setupTransmit(TX_PIN, MAN_1200);
  
	//ustawienie pinów jako wyjść (output) w celu sterowania rejestrem przesuwanym
	pinMode(clockPin, OUTPUT);
	pinMode(latchPin, OUTPUT);
	pinMode(dataPin, OUTPUT);
		
	//ds18b20
	sensors.begin();
	
}

void loop() {
	int temp = temperatura();
	man.transmit(temp);
	if (temp==-127) {display(10, 10, latchPin, dataPin);}
	else if (temp>=100){
		if (k%2==1){
			
			display(10, 10, latchPin, dataPin);
			k++;
		}
		else{
			display(11, 11, latchPin, dataPin);
			k=1;
		}
	}
	else if (temp>=90){
		if(n%2==1){
			display(11, 11, latchPin, dataPin);
			n++;
		}
		else{
		int liczba1 = temp/10%10;
		int liczba2 = temp%10;
		
		display(liczba1, liczba2, latchPin, dataPin);
		n=1;
		}
		
	}
	else
	{
		int liczba1 = temp/10%10;
		int liczba2 = temp%10;
		
		display(liczba1, liczba2, latchPin, dataPin);
			
	}
	
	delay(3000);
}


// Funkcja display 
void display(int liczba1, int liczba2, int latchPin, int dataPin) {
	digitalWrite(latchPin, LOW);
	shiftOut(dataPin, clockPin, MSBFIRST, dec_digits[liczba2]);
	shiftOut(dataPin, clockPin, MSBFIRST, dec_digits[liczba1]);
	digitalWrite(latchPin, HIGH);
}

//Funkcja zwrace temperature
int temperatura(){
	sensors.requestTemperatures();
	char tempC = sensors.getTempCByIndex(0); 
	return tempC;
}
