/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These displays use SPI to communicate, 4 or 5 pins are required to  
interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen below must be included in any redistribution
*********************************************************************/

#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include "t3_Adafruit_SSD1306.h"

#include "Adafruit_GFX.h"

// the memory buffer for the LCD



// the most basic function, set a single pixel
void t3_Adafruit_SSD1306::drawPixel(int16_t x, int16_t y, uint16_t color) {
	int16_t z;
  if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
    return;

  // check rotation, move pixel around if necessary
  switch (getRotation()) {
  case 1:
	  z = x;
	  x = y;
	  y = z;
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    break;
  case 3:
	  z = x;
	  x = y;
	  y = z;
    y = HEIGHT - y - 1;
    break;
  }  

  // x is which column
  if (color == WHITE) 
    buffer[x+ (y/8)*SSD1306_LCDWIDTH] |= _BV((y%8));  
  else
    buffer[x+ (y/8)*SSD1306_LCDWIDTH] &= ~_BV((y%8)); 
}

// initializer for I2C - we only indicate the reset pin!
t3_Adafruit_SSD1306::t3_Adafruit_SSD1306(i2c_t3 *w) :
Adafruit_GFX(SSD1306_LCDWIDTH, SSD1306_LCDHEIGHT) {
  sclk = dc = cs = sid = -1;
  rst = 0;
  wire = w;
}
  
i2c_t3& t3_Adafruit_SSD1306::get_wire() {
	return (wire ? *wire : Wire);
}

void t3_Adafruit_SSD1306::begin(uint8_t vccstate, uint8_t i2caddr) {
  _i2caddr = i2caddr;

  // set pin directions
  if (sid != -1)
  {
    // SPI
    pinMode(sid, OUTPUT);
    pinMode(sclk, OUTPUT);
    pinMode(dc, OUTPUT);
    pinMode(cs, OUTPUT);
    clkport     = portOutputRegister(digitalPinToPort(sclk));
    clkpinmask  = digitalPinToBitMask(sclk);
    mosiport    = portOutputRegister(digitalPinToPort(sid));
    mosipinmask = digitalPinToBitMask(sid);
    csport      = portOutputRegister(digitalPinToPort(cs));
    cspinmask   = digitalPinToBitMask(cs);
    dcport      = portOutputRegister(digitalPinToPort(dc));
    dcpinmask   = digitalPinToBitMask(dc);
  }
  else
  {
    // I2C Init
	get_wire().begin(); // Is this the right place for this?
  }

  // Setup reset pin direction (used by both SPI and I2C)  
//  pinMode(rst, OUTPUT);
//  digitalWrite(rst, HIGH);
  // VDD (3.3V) goes high at start, lets just chill for a ms
//  delay(1);
  // bring reset low
//  digitalWrite(rst, LOW);
  // wait 10ms
//  delay(10);
  // bring out of reset
//  digitalWrite(rst, HIGH);
  // turn on VCC (9V?)

   #if defined SSD1306_128_32
    // Init sequence for 128x32 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x1F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x10); }
    else 
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
	ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x22); }
    else 
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
  #endif

  #if defined SSD1306_128_64
    // Init sequence for 128x64 OLED module
    ssd1306_command(SSD1306_DISPLAYOFF);                    // 0xAE
//    ssd1306_command(0x00);//---set low column address
//    ssd1306_command(0x10);//---set high column address
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);            // 0xD5
    ssd1306_command(0x80);                                  // the suggested ratio 0x80
    ssd1306_command(SSD1306_SETMULTIPLEX);                  // 0xA8
    ssd1306_command(0x3F);
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);              // 0xD3
    ssd1306_command(0x0);                                   // no offset
    ssd1306_command(SSD1306_SETSTARTLINE | 0x0);            // line #0
    ssd1306_command(SSD1306_CHARGEPUMP);                    // 0x8D
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x10); }
    else 
      { ssd1306_command(0x14); }
    ssd1306_command(SSD1306_MEMORYMODE);                    // 0x20
    ssd1306_command(0x00);                                  // 0x0 act like ks0108
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);                    // 0xDA
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);                   // 0x81
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x9F); }
    else 
      { ssd1306_command(0xCF); }
    ssd1306_command(SSD1306_SETPRECHARGE);                  // 0xd9
    if (vccstate == SSD1306_EXTERNALVCC) 
      { ssd1306_command(0x22); }
    else 
      { ssd1306_command(0xF1); }
    ssd1306_command(SSD1306_SETVCOMDETECT);                 // 0xDB
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYALLON_RESUME);           // 0xA4
    ssd1306_command(SSD1306_NORMALDISPLAY);                 // 0xA6
    
    ssd1306_command(0xb0);
    ssd1306_command(0x10);
    ssd1306_command(0x01);//Set original position to (0,0)
  #endif
  
  ssd1306_command(SSD1306_DISPLAYON);//--turn on oled panel
}


void t3_Adafruit_SSD1306::invertDisplay(uint8_t i) {
  if (i) {
    ssd1306_command(SSD1306_INVERTDISPLAY);
  } else {
    ssd1306_command(SSD1306_NORMALDISPLAY);
  }
}

void t3_Adafruit_SSD1306::ssd1306_command(uint8_t c) { 
  if (sid != -1)
  {
    // SPI
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
    //digitalWrite(dc, LOW);
    *dcport &= ~dcpinmask;
    //digitalWrite(cs, LOW);
    *csport &= ~cspinmask;
    fastSPIwrite(c);
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
  }
  else
  {
    // I2C
    uint8_t control = 0x00;   // Co = 0, D/C = 0
    get_wire().beginTransmission(_i2caddr);
    get_wire().write(control);
    get_wire().write(c);
    get_wire().endTransmission();
  }
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void t3_Adafruit_SSD1306::startscrollright(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void t3_Adafruit_SSD1306::startscrollleft(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(0XFF);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void t3_Adafruit_SSD1306::startscrolldiagright(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);	
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void t3_Adafruit_SSD1306::startscrolldiagleft(uint8_t start, uint8_t stop){
	ssd1306_command(SSD1306_SET_VERTICAL_SCROLL_AREA);	
	ssd1306_command(0X00);
	ssd1306_command(SSD1306_LCDHEIGHT);
	ssd1306_command(SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1306_command(0X00);
	ssd1306_command(start);
	ssd1306_command(0X00);
	ssd1306_command(stop);
	ssd1306_command(0X01);
	ssd1306_command(SSD1306_ACTIVATE_SCROLL);
}

void t3_Adafruit_SSD1306::stopscroll(void){
	ssd1306_command(SSD1306_DEACTIVATE_SCROLL);
}

void t3_Adafruit_SSD1306::ssd1306_data(uint8_t c) {
  if (sid != -1)
  {
    // SPI
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
    //digitalWrite(dc, HIGH);
    *dcport |= dcpinmask;
    //digitalWrite(cs, LOW);
    *csport &= ~cspinmask;
    fastSPIwrite(c);
    //digitalWrite(cs, HIGH);
    *csport |= cspinmask;
  }
  else
  {
    // I2C
    uint8_t control = 0x40;   // Co = 0, D/C = 1
    get_wire().beginTransmission(_i2caddr);
    get_wire().write(control);
    get_wire().write(c);
    get_wire().endTransmission();
  }
}

void t3_Adafruit_SSD1306::display(void) {
  ssd1306_command(SSD1306_SETLOWCOLUMN | 0x0);  // low col = 0
  ssd1306_command(SSD1306_SETHIGHCOLUMN | 0x0);  // hi col = 0
  ssd1306_command(SSD1306_SETSTARTLINE | 0x0); // line #0

  if (sid != -1)
  {
    // SPI
    *csport |= cspinmask;
    *dcport |= dcpinmask;
    *csport &= ~cspinmask;

    for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
      fastSPIwrite(buffer[i]);
      //ssd1306_data(buffer[i]);
    }
    // i wonder why we have to do this (check datasheet)
    if (SSD1306_LCDHEIGHT == 32) {
      for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
        //ssd1306_data(0);
        fastSPIwrite(0);
      }
    }
    *csport |= cspinmask;
  }
  else
  {
    // save I2C bitrate
//    uint8_t twbrbackup = TWBR;
//    TWBR = 12; // upgrade to 400KHz!

    //Serial.println(TWBR, DEC);
    //Serial.println(TWSR & 0x3, DEC);

    // I2C
    for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
      // send a bunch of data in one xmission
      get_wire().beginTransmission(_i2caddr);
      get_wire().write(0x40);
      for (uint8_t x=0; x<16; x++) {
	get_wire().write(buffer[i]);
	i++;
      }
      i--;
      get_wire().endTransmission();
    }
    // i wonder why we have to do this (check datasheet)
    if (SSD1306_LCDHEIGHT == 32) {
      for (uint16_t i=0; i<(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8); i++) {
	// send a bunch of data in one xmission
	get_wire().beginTransmission(_i2caddr);
	get_wire().write(0x40);
	for (uint8_t x=0; x<16; x++) {
	  get_wire().write((uint8_t)0x00);
	  i++;
	}
	i--;
	get_wire().endTransmission();
      }
    }
//    TWBR = twbrbackup;
  }
}

// clear everything
void t3_Adafruit_SSD1306::clearDisplay(void) {
  memset(buffer, 0, (SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8));
}


inline void t3_Adafruit_SSD1306::fastSPIwrite(uint8_t d) {
  
  for(uint8_t bit = 0x80; bit; bit >>= 1) {
    *clkport &= ~clkpinmask;
    if(d & bit) *mosiport |=  mosipinmask;
    else        *mosiport &= ~mosipinmask;
    *clkport |=  clkpinmask;
  }
  //*csport |= cspinmask;
}

inline void t3_Adafruit_SSD1306::slowSPIwrite(uint8_t d) {
 for (int8_t i=7; i>=0; i--) {
   digitalWrite(sclk, LOW);
   if (d & _BV(i)) {
     digitalWrite(sid, HIGH);
   } else {
     digitalWrite(sid, LOW);
   }
   digitalWrite(sclk, HIGH);
 }
}

