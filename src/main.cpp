/*
 * main.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: robin
 */

#include <Arduino.h>
#include <i2c_t3.h>

#include <t3_Adafruit_SSD1306.h>



t3_Adafruit_SSD1306 gfx0(&Wire2);
t3_Adafruit_SSD1306 gfx1(&Wire1);

int x = 63;
int y = 31;

bool blink;
int color = WHITE;

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

void loop() {
	bool u = up();
	bool d = down();
	bool l = left();
	bool r = right();
/*
	if(u)
		gfx1.drawTriangle(64, 0, 48, 16, 80, 16, WHITE);
	if(d)
		gfx1.drawTriangle(64, 63, 48, 47, 80, 47, WHITE);
	if(l)
		gfx1.drawTriangle(0, 32, 16, 16, 16, 48, WHITE);
	if(r)
		gfx1.drawTriangle(127, 32, 111, 16, 111, 48, WHITE);
*/

	if(u || d || l || r) {
		if(x < 128) {
			gfx0.drawPixel(x, y, color);
		} else {
			gfx1.drawPixel(x - 128, y, color);
		}
		x += (l ? -1 : 0) + (r ? 1 : 0);
		y += (u ? -1 : 0) + (d ? 1 : 0);
		x += (x < 0 ? 256 : 0) + (x >= 256 ? -256 : 0);
		y += (y < 0 ? 64 : 0) + (y >= 64 ? -64 : 0);
	}
	if(touchRead(17) > 1500) {
		color = (color == BLACK) ? WHITE : BLACK;
		gfx0.invertDisplay(!color);
		gfx1.invertDisplay(!color);

	}
	if(touchRead(16) > 1500) {
		gfx0.clearDisplay();
		gfx1.clearDisplay();
		gfx0.display();
		gfx1.display();
	}
	if(x < 128) {
		gfx0.drawPixel(x, y, blink ? WHITE : BLACK);
	} else {
		gfx1.drawPixel(x - 128, y, blink ? WHITE : BLACK);
	}

	if(x < 128 || x == 0 || x == 255)
		gfx0.display();
	if(x >= 128 || x == 0 || x == 255)
		gfx1.display();

	blink = !blink;
//	gfx1.display();
}
