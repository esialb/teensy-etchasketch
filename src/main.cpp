/*
 * main.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: robin
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <i2c_t3.h>

#include <t3_Adafruit_SSD1306.h>



t3_Adafruit_SSD1306 gfx0(&Wire2);
t3_Adafruit_SSD1306 gfx1(&Wire1);

int x = 63;
int y = 31;

bool blink;
bool color = WHITE;

void setup() {
	gfx0.begin();
	gfx1.begin();

	gfx0.clearDisplay();
	gfx1.clearDisplay();

	gfx0.setCursor(0, 0);
	gfx1.setCursor(0, 0);

	gfx0.setTextColor(WHITE);
	gfx1.setTextColor(WHITE);

	gfx0.display();
	gfx1.display();
}

bool up() {
	return touchRead(23) > 1500;
}

bool right() {
	return touchRead(22) > 1500;
}

bool down() {
	return touchRead(18) > 1500;
}

bool left() {
	return touchRead(19) > 1500;
}

void set(int x, int y, bool isWhite) {
	if(x < 128)
		gfx0.drawPixel(x, y, isWhite);
	else
		gfx1.drawPixel(x - 128, y, isWhite);
}

void loop() {
	bool u = up();
	bool d = down();
	bool l = left();
	bool r = right();

	set(x, y, blink);

	if(u || d || l || r) {
		set(x, y, color);
		x += (l ? -1 : 0) + (r ? 1 : 0);
		y += (u ? -1 : 0) + (d ? 1 : 0);
		x += (x < 0 ? 256 : 0) + (x >= 256 ? -256 : 0);
		y += (y < 0 ? 64 : 0) + (y >= 64 ? -64 : 0);
		set(x, y, !color);
	}
	if(touchRead(17) > 1500) {
		color = (color == BLACK) ? WHITE : BLACK;
		gfx0.invertDisplay(!color);
		gfx1.invertDisplay(!color);
	}
	if(touchRead(16) > 1500) {
		x = 63;
		y = 31;
		color = WHITE;
		gfx0.invertDisplay(false);
		gfx1.invertDisplay(false);
		gfx0.clearDisplay();
		gfx1.clearDisplay();
		gfx0.display();
		gfx1.display();
	}

	if(touchRead(29) > 1500) {
		x = EEPROM[0];
		y = EEPROM[1];
		color = EEPROM[2];
		for(int i = 0; i < 1024; i++) {
			gfx0.buffer[i] = EEPROM[i + 3];
			gfx1.buffer[i] = EEPROM[i + 3 + 1024];
		}
		gfx0.display();
		gfx1.display();
		gfx0.invertDisplay(!color);
		gfx1.invertDisplay(!color);
	}

	if(touchRead(30) > 1500) {
		EEPROM.begin();
		EEPROM.write(0, x);
		EEPROM.write(1, y);
		EEPROM.write(2, color);
		for(int i = 0; i < 1024; i++) {
			EEPROM.write(i + 3, gfx0.buffer[i]);
			EEPROM.write(i + 3 + 1024, gfx1.buffer[i]);
		}
		EEPROM.end();
	}

	if(x < 128 || x == 128 || x == 255)
		gfx0.display();
	if(x >= 128 || x == 0 || x == 127)
		gfx1.display();

	blink = !blink;
//	gfx1.display();
}
