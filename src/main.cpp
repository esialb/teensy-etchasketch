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

#define MAX_FILES 100

#define SCREEN_BUFFER_SIZE 1024
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define CANVAS_WIDTH 256
#define CANVAS_HEIGHT 64
#define LOAD_SAVE_DELAY 10

#define TMP_FILE "tmp.ets"

#define UP_KEY 23
#define RIGHT_KEY 22
#define DOWN_KEY 18
#define LEFT_KEY 19
#define INVERT_KEY 17
#define RESET_KEY 16
#define LOAD_KEY 29
#define SAVE_KEY 30
#define KEY_CAPACITANCE 1500

t3_Adafruit_SSD1306 gfx0(&Wire2);
t3_Adafruit_SSD1306 gfx1(&Wire1);

int x = SCREEN_WIDTH / 2;
int y = SCREEN_HEIGHT / 2;

bool blink;
bool color = WHITE;

int loaddelay = LOAD_SAVE_DELAY;
int savedelay = LOAD_SAVE_DELAY;

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
	return touchRead(UP_KEY) > KEY_CAPACITANCE;
}

bool right_key() {
	return touchRead(RIGHT_KEY) > KEY_CAPACITANCE;
}

bool down_key() {
	return touchRead(DOWN_KEY) > KEY_CAPACITANCE;
}

bool left_key() {
	return touchRead(LEFT_KEY) > KEY_CAPACITANCE;
}

bool invert_key() {
	return touchRead(INVERT_KEY) > KEY_CAPACITANCE;
}

bool reset_key() {
	return touchRead(RESET_KEY) > KEY_CAPACITANCE;
}

bool load_key() {
	return touchRead(LOAD_KEY) > KEY_CAPACITANCE;
}

bool save_key() {
	return touchRead(SAVE_KEY) > KEY_CAPACITANCE;
}

void save_file(const char *buf) {
	File file = SD.open(buf, FILE_WRITE);
	file.write(x);
	file.write(y);
	file.write(color);
	file.write(gfx0.buffer, SCREEN_BUFFER_SIZE);
	file.write(gfx1.buffer, SCREEN_BUFFER_SIZE);
	file.close();
}

void load_file(const char *buf) {
	File file = SD.open(buf, FILE_READ);
	x = file.read();
	y = file.read();
	color = file.read();
	file.read(gfx0.buffer, SCREEN_BUFFER_SIZE);
	file.read(gfx1.buffer, SCREEN_BUFFER_SIZE);
	gfx0.display();
	gfx1.display();
	gfx0.invertDisplay(!color);
	gfx1.invertDisplay(!color);
}

void select_save_file() {
	char buf[20];
	int idx = 0;
	sprintf(buf, "%i.ets", idx);
	while(SD.exists(buf)) {
		idx++;
		sprintf(buf, "%i.ets", idx);
	}
	if(idx == MAX_FILES) {
		save_file(TMP_FILE);
		gfx0.setCursor(0, 0);
		gfx0.print("SD card full");
		gfx0.display();
		while(!reset_key())
			;
		while(reset_key());
		load_file(TMP_FILE);
		SD.remove(TMP_FILE);
		return;
	}
	gfx0.invertDisplay(color);
	gfx1.invertDisplay(color);
	save_file(buf);
	gfx0.invertDisplay(!color);
	gfx1.invertDisplay(!color);
}

void ets_file(char *buf, int idx) {
	sprintf(buf, "%i.ets", idx);
}

void select_load_file() {
	char buf[20];
	int idx = 0;
	ets_file(buf, idx);
	for(; idx < MAX_FILES && !SD.exists(buf); idx++)
		ets_file(buf, idx);
	if(idx == MAX_FILES)
		return;

	save_file(TMP_FILE);
	load_file(buf);

	while(!invert_key()) {
		if(reset_key()) {
			load_file(TMP_FILE);
			SD.remove("tmp.ets");
			return;
		}
		if(up_key()) {
			if(++idx == MAX_FILES)
				idx = 0;
			ets_file(buf, idx);
			while(!SD.exists(buf)) {
				if(++idx == MAX_FILES)
					idx = 0;
				ets_file(buf, idx);
			}
			load_file(buf);
			while(up_key())
				;
		}
		if(down_key()) {
			if(--idx == -1)
				idx = MAX_FILES - 1;
			ets_file(buf, idx);
			while(!SD.exists(buf)) {
				if(--idx == -1)
					idx = MAX_FILES - 1;
				ets_file(buf, idx);
			}
			load_file(buf);
			while(down_key())
				;
		}
		gfx0.setCursor(0, 0);
		gfx0.print(buf);
		gfx0.display();
	}
	load_file(buf);
	SD.remove(TMP_FILE);
}

void set(int x, int y, bool isWhite) {
	if(x < SCREEN_WIDTH)
		gfx0.drawPixel(x, y, isWhite);
	else
		gfx1.drawPixel(x - SCREEN_WIDTH, y, isWhite);
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
		x += (x < 0 ? CANVAS_WIDTH : 0) + (x >= CANVAS_WIDTH ? -CANVAS_WIDTH : 0);
		y += (y < 0 ? CANVAS_HEIGHT : 0) + (y >= CANVAS_HEIGHT ? -CANVAS_HEIGHT : 0);
		set(x, y, !color);
	}
	if(invert_key()) {
		color = (color == BLACK) ? WHITE : BLACK;
		gfx0.invertDisplay(!color);
		gfx1.invertDisplay(!color);
	}
	if(reset_key()) {
		x = SCREEN_WIDTH / 2;
		y = SCREEN_HEIGHT / 2;
		color = WHITE;
		gfx0.invertDisplay(false);
		gfx1.invertDisplay(false);
		gfx0.clearDisplay();
		gfx1.clearDisplay();
		gfx0.display();
		gfx1.display();
	}

	if(load_key()) {
		if(loaddelay == 0)
			select_load_file();
		loaddelay--;
	} else
		loaddelay = LOAD_SAVE_DELAY;

	if(save_key()) {
		if(savedelay == 0)
			select_save_file();
		savedelay--;
	} else
		savedelay = LOAD_SAVE_DELAY;

	if(x < SCREEN_WIDTH || x == SCREEN_WIDTH || x == CANVAS_WIDTH - 1)
		gfx0.display();
	if(x >= SCREEN_WIDTH || x == 0 || x == SCREEN_WIDTH - 1)
		gfx1.display();

	blink = !blink;
}
