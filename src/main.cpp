/*
 * main.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: robin
 */

#include <Arduino.h>
#include <EEPROM.h>
#include <i2c_t3.h>
#include <SD.h>
#include <string.h>
#include <stdio.h>

#include <t3_Adafruit_SSD1306.h>



t3_Adafruit_SSD1306 gfx0(&Wire2);
t3_Adafruit_SSD1306 gfx1(&Wire1);

int x = 63;
int y = 31;

bool blink;
bool color = WHITE;

int savedelay = 10;

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

	if(!SD.begin(BUILTIN_SDCARD)) {
		gfx0.print("SD error");
	}
}

bool up_key() {
	return touchRead(23) > 1500;
}

bool right_key() {
	return touchRead(22) > 1500;
}

bool down_key() {
	return touchRead(18) > 1500;
}

bool left_key() {
	return touchRead(19) > 1500;
}

bool invert_key() {
	return touchRead(17) > 1500;
}

bool reset_key() {
	return touchRead(16) > 1500;
}

bool load_key() {
	return touchRead(29) > 1500;
}

bool save_key() {
	return touchRead(30) > 1500;
}

void save_file() {
	char buf[20];
	int idx = 0;
	sprintf(buf, "%i.ets", idx);
	while(SD.exists(buf)) {
		idx++;
		sprintf(buf, "%i.ets", idx);
	}
	File file = SD.open(buf, FILE_WRITE);
	file.write(x);
	file.write(y);
	file.write(color);
	file.write(gfx0.buffer, 1024);
	file.write(gfx1.buffer, 1024);
	file.close();
}

void load_file() {
	char buf[20];
	int idx = 0;
	sprintf(buf, "%i.ets", idx);
	for(; idx < 256 && !SD.exists(buf); idx++)
		sprintf(buf, "%i.ets", idx);
	if(idx == 256)
		return;

	uint8_t line1[128];
	memcpy(line1, gfx0.buffer, 128);
	memset(gfx0.buffer, 0, 128);

	while(!invert_key()) {
		if(reset_key()) {
			memcpy(gfx0.buffer, line1, 128);
			return;
		}
		if(up_key()) {
			idx++;
			if(idx == 256)
				idx = 0;
			sprintf(buf, "%i.ets", idx);
			while(!SD.exists(buf)) {
				idx++;
				if(idx == 256)
					idx = 0;
				sprintf(buf, "%i.ets", idx);
			}
			while(up_key())
				;
		}
		if(down_key()) {
			idx--;
			if(idx == -1)
				idx = 255;
			sprintf(buf, "%i.ets", idx);
			while(!SD.exists(buf)) {
				idx--;
				if(idx == -1)
					idx = 255;
				sprintf(buf, "%i.ets", idx);
			}
			while(down_key())
				;
		}
		memset(gfx0.buffer, 0, 128);
		gfx0.setCursor(0, 0);
		gfx0.print(buf);
		gfx0.display();
	}
	File file = SD.open(buf, FILE_READ);
	x = file.read();
	y = file.read();
	color = file.read();
	file.read(gfx0.buffer, 1024);
	file.read(gfx1.buffer, 1024);
}

void set(int x, int y, bool isWhite) {
	if(x < 128)
		gfx0.drawPixel(x, y, isWhite);
	else
		gfx1.drawPixel(x - 128, y, isWhite);
}

void loop() {
	bool u = up_key();
	bool d = down_key();
	bool l = left_key();
	bool r = right_key();

	set(x, y, blink);

	if(u || d || l || r) {
		set(x, y, color);
		x += (l ? -1 : 0) + (r ? 1 : 0);
		y += (u ? -1 : 0) + (d ? 1 : 0);
		x += (x < 0 ? 256 : 0) + (x >= 256 ? -256 : 0);
		y += (y < 0 ? 64 : 0) + (y >= 64 ? -64 : 0);
		set(x, y, !color);
	}
	if(invert_key()) {
		color = (color == BLACK) ? WHITE : BLACK;
		gfx0.invertDisplay(!color);
		gfx1.invertDisplay(!color);
	}
	if(reset_key()) {
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

	if(load_key()) {
		load_file();
		gfx0.display();
		gfx1.display();
		gfx0.invertDisplay(!color);
		gfx1.invertDisplay(!color);
	}

	if(save_key()) {
		if(savedelay == 0) {
			gfx0.invertDisplay(color);
			gfx1.invertDisplay(color);
			save_file();
/*
			EEPROM.begin();
			EEPROM.write(0, x);
			EEPROM.write(1, y);
			EEPROM.write(2, color);
			for(int i = 0; i < 1024; i++) {
				EEPROM.write(i + 3, gfx0.buffer[i]);
				EEPROM.write(i + 3 + 1024, gfx1.buffer[i]);
			}
			EEPROM.end();
*/
			delay(200);
			gfx0.invertDisplay(!color);
			gfx1.invertDisplay(!color);
		}
		savedelay--;
	} else
		savedelay = 10;

	if(x < 128 || x == 128 || x == 255)
		gfx0.display();
	if(x >= 128 || x == 0 || x == 127)
		gfx1.display();

	blink = !blink;
//	gfx1.display();
}
