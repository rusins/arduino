#include <Adafruit_NeoPixel.h>  // https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-installation
#include <stdlib.h>

#define PIN 6 // Pie kura pin pielikta diozhu virkne
#define LEDS  // Cik ir diodes
#define DEFTIME 20000 // Cik ilgi parastais efekts strādā (mili-sekundes)
#define TIME 10000 // Cik ilgi speciālie efekti strādā (mili-sekundes)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LEDS, PIN, NEO_GRB + NEO_KHZ800);
void setup() {
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
	srand(23);
}

uint32_t backg = strip.Color(0, 0, 150);
uint32_t forg = strip.Color(255, 60, 0);

int time;
int pos = 0;

void loop() {
	for (int i = 0; i < LEDS; ++i)
		strip.setPixelColor(i, backg);
	strip.setPixelColor(pos, forg);
	strip.show();
	delay(150);
	effect0(5000);
	int effect = rand() % 3 + 1;
	time = rand() % TIME + TIME;
	switch (1) {
	case 1:
		effect1();
		break;
	case 2:
		effect2();
		break;
	case 3:
		effect3();
		break;
	}
}

void effect0(int t) {
	int time_elapsed = 0;
	while (time_elapsed < t) {
		strip.setPixelColor(pos, backg);
		++pos %= LEDS;
		strip.setPixelColor(pos, forg);
		strip.show();
		delay(150);
		time_elapsed += 150;
	}
}

void effect1() {
	float d = 0.015f * TIME;
	while (d > 20) {
		float delta = d / 80;
		strip.setPixelColor(pos, backg);
		++pos %= LEDS;
		strip.setPixelColor(pos, forg);
		strip.show();
		delay(d);
		d -= delta;
	}
	for (int i = 0; i < LEDS; ++i) {
		++pos %= LEDS;
		strip.setPixelColor(pos, forg);
		strip.show();
		delay(d);
	}
	delay(500);
	bool on[LEDS];
	for (int i = 0; i < LEDS; ++i)
		on[i] = true;
	do {
		done = true;
		for (int i= 0; done && i < LEDS; ++i) {
			if (on[i])
				done = false;
		}
		int x = rand() % LEDS;
		on[x] = false;
		strip.setPixelColor(x, 0);
		strip.show();
		delay(100);
	} while (!done);
	
}

void effect2() {
	int d = 50 * LEDS;
	int t = 0;
	uint32_t mem[100];
	int m = 0;
	while (t < TIME) {
		int x = rand() % LEDS;
		uint32_t c = Wheel(rand() & 255);
		for (int i = 0; i <= LEDS/2; ++i){
			strip.setPixelColor(x+i % LEDS, c);
			strip.setPixelColor(x-1 % LEDS, c);
			strip.show();
			delay(d);
			t += d;
		}
		mem[m++] = c;
	}
	m--;
	while (i >= 0) {
		for (int i = 0; i < LEDS; ++i) {
			strip.setPixelColor(i, mem[m]);
			m--;
			delay(d);
			t += d;
			d *= 0.8; // Paātrina replay :)
		}
	}
}

void effect3() {
	int d = TIME / LEDS / LEDS;
	uint32_t mem[LEDS];
	for (int split = 1; split <= LEDS; ++split) {
		int t = TIME / LEDS;
		for (int i = 0; i < split; ++i)
			mem[i] = wheel(rand() & 255);
		for (n = 0; t > 0; ++n){
			for (int i = 0; i < LEDS; ++i) {
				strip.setColor((i + n) % LEDS, mem[i/split]);
			}
			strip.show();
			delay(d);
			t += d;
		}
	       
	}
}

uint32_t Wheel(byte WheelPos) {
	WheelPos = 255 - WheelPos;
	if(WheelPos < 85) {
		return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
	}
	if(WheelPos < 170) {
		WheelPos -= 85;
		return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
	}
	WheelPos -= 170;
	return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}