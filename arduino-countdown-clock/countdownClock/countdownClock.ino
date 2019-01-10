// 2016/2017 spartronics countdown clock using teensyduino arduino add-on
#include <Adafruit_NeoPixel.h>
#include <Timer.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define pin            17  // the physical pin on the arduino connected to signal -- only 17 will work with the teensy, since no others output 5V (only 3.3V)
#define numOfPixels    315   // total "pixels" in the strand

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(numOfPixels, pin, NEO_GRB + NEO_KHZ800);

Timer t;
int pixArray[7][45];        // 7 rows, 45 columns (y,x)
int timeRemaining = 393100;   // total time in seconds remaining from upload (393100 = seconds from noon Jan 7th to midnight on Feb 21, 2017)

int button1Val = 0;       // button1 open or closed, the button that controls the setting/state of the clock
int lastButton1Val = 0;   // stops multiple readings for one click
int button1State = 0;     // cycles through setting modes
int button2Val = 0;       // changes value of selected "slot" (days, hours, minutes, seconds, as determined by button1)
int lastButton2Val = 0;
int button3Val = 0;       // determines if button2 adds or subtracts the value
int lastButton3Val = 0;
int cycleDirection = 1;   // stores from button3, 1 = adding to clock value, -1 = subtracting from clock value

bool settingTime = false;

// current time devided up
int nowSec;
int nowMin;
int nowHrs;
int nowDays;

// rgb value for the LED display, the colons use (b, b, r) to create a yellow color
int r = 15;
int g = 15;
int b = 150;


void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  for (int i = 0; i < numOfPixels; i++) {
    int y = i / 45;
    int x = i % 45;
    if (y % 2 == 1) {
      x = 44 - x;
    }
    pixArray[y][x] = i;
  }

  t.every(1000, checkTime);
}

void loop() {
  // setting the value from the pin the button's connected to (14, 15, 16 labelled on the back of the Arduino chip)
  button1Val = analogRead(A0);
  button2Val = analogRead(A1);
  button3Val = analogRead(A2);

  // button1 logic, states 1-4 are "Setting the time" and state 5 sets back to 0, regular counting down
  if (button1Val != lastButton1Val && button1Val == 1) {
    button1State ++;
    if (button1State == 5) {
      button1State = 0;
      settingTime = false;
    }
    if (button1State != 0) {
      settingTime = true;
    }
    lastButton1Val = button1Val;
  }
  else if (button1Val == 0) {
    lastButton1Val = button1Val;
  }

  // button2 logic
  if (button2Val != lastButton2Val && button2Val == 1) {
    if (button2Val == 1 && button1State == 1) {
      timeRemaining = timeRemaining + cycleDirection * 86400; // changes time by one day, checkTime updates display
      checkTime();
    }
    if (button2Val == 1 && button1State == 2) {
      timeRemaining = timeRemaining + cycleDirection * 3600; // changes time by one hour
      checkTime();
    }
    if (button2Val == 1 && button1State == 3) {
      timeRemaining = timeRemaining + cycleDirection * 60; // changes time by one minute
      checkTime();
    }
    if (button2Val == 1 && button1State == 4) {
      timeRemaining = timeRemaining + cycleDirection * 1; // changes time by one second
      checkTime();
    }
    lastButton2Val = button2Val;
  }
  else if (button2Val == 0) {
    lastButton2Val = button2Val;
  }

  // button3 logic, changes cycle direction of button2
  if (button3Val != lastButton3Val && button3Val == 1) {
    cycleDirection = cycleDirection * -1;
    lastButton3Val = button3Val;
  }
  else if (button3Val == 0) {
    lastButton3Val = button3Val;
  }

  if (settingTime == false) {
    if (timeRemaining >= 0) {
      t.update();
    }
  }
}


//
//   Draw number "num" in column "x"
//     ex: draw(6, 2); draws a 6 in the second column
void draw(int num, int x) {
  if (num == 0 || num == 8 || num == 6) {
    // pixels.Color takes RGB values, from 0, 0, 0 up to 255, 255, 255
    pixels.setPixelColor(pixArray[0][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[1][x], pixels.Color(r, g, b)); // Left vertical line
    pixels.setPixelColor(pixArray[2][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[4][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[5][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x], pixels.Color(r, g, b));

    pixels.setPixelColor(pixArray[0][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x + 3], pixels.Color(r, g, b)); // Part of right vertical line
    pixels.setPixelColor(pixArray[4][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[5][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 3], pixels.Color(r, g, b));

    if (num == 8 || num == 0) {
      pixels.setPixelColor(pixArray[1][x + 3], pixels.Color(r, g, b)); // The rest of the right vertical line
      pixels.setPixelColor(pixArray[2][x + 3], pixels.Color(r, g, b));
    }

    pixels.setPixelColor(pixArray[0][x + 1], pixels.Color(r, g, b)); // Top and bottom
    pixels.setPixelColor(pixArray[0][x + 2], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 1], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 2], pixels.Color(r, g, b));

    if (num == 8 || num == 6) {
      pixels.setPixelColor(pixArray[3][x + 1], pixels.Color(r, g, b)); // Middle just for 8 and 6
      pixels.setPixelColor(pixArray[3][x + 2], pixels.Color(r, g, b));
    }

  } else if (num == 1 || num == 7 || num == 9 || num == 4 || num == 3) {
    pixels.setPixelColor(pixArray[0][x + 3], pixels.Color(r, g, b)); // Right vertical line
    pixels.setPixelColor(pixArray[1][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[2][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[4][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[5][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 3], pixels.Color(r, g, b));

    if (num == 7 || num == 9 || num == 3) {
      pixels.setPixelColor(pixArray[0][x], pixels.Color(r, g, b)); // Top just for 7, 3, and 9
      pixels.setPixelColor(pixArray[0][x + 1], pixels.Color(r, g, b));
      pixels.setPixelColor(pixArray[0][x + 2], pixels.Color(r, g, b));
      if (num == 9 || num == 3) {
        pixels.setPixelColor(pixArray[6][x], pixels.Color(r, g, b)); // Bottom just for 3 and 9
        pixels.setPixelColor(pixArray[6][x + 1], pixels.Color(r, g, b));
        pixels.setPixelColor(pixArray[6][x + 2], pixels.Color(r, g, b));
      }
    }

    if (num == 4 || num == 9 || num == 3) {
      pixels.setPixelColor(pixArray[3][x], pixels.Color(r, g, b)); // Middle for 4, 9, and 3
      pixels.setPixelColor(pixArray[3][x + 1], pixels.Color(r, g, b));
      pixels.setPixelColor(pixArray[3][x + 2], pixels.Color(r, g, b));
      if (num == 4 || num == 9) {
        pixels.setPixelColor(pixArray[0][x], pixels.Color(r, g, b)); // Left top side for 9 and 4
        pixels.setPixelColor(pixArray[1][x], pixels.Color(r, g, b));
        pixels.setPixelColor(pixArray[2][x], pixels.Color(r, g, b));
      }
    }
  } else if (num == 2 || num == 5) {
    pixels.setPixelColor(pixArray[0][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[0][x + 1], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[0][x + 2], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[0][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x + 1], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x + 2], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[3][x + 3], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 1], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 2], pixels.Color(r, g, b));
    pixels.setPixelColor(pixArray[6][x + 3], pixels.Color(r, g, b));
    if (num == 2) {
      pixels.setPixelColor(pixArray[1][x + 3], pixels.Color(r, g, b)); // Right top side for 2
      pixels.setPixelColor(pixArray[2][x + 3], pixels.Color(r, g, b));
      pixels.setPixelColor(pixArray[4][x], pixels.Color(r, g, b)); // Left bottom side for 2
      pixels.setPixelColor(pixArray[5][x], pixels.Color(r, g, b));
    } else if (num == 5) {
      pixels.setPixelColor(pixArray[1][x], pixels.Color(r, g, b)); // Left top side for 5
      pixels.setPixelColor(pixArray[2][x], pixels.Color(r, g, b));
      pixels.setPixelColor(pixArray[4][x + 3], pixels.Color(r, g, b)); // Right bottom side for 5
      pixels.setPixelColor(pixArray[5][x + 3], pixels.Color(r, g, b));
    }
  } else {
  }

}

void checkTime() {
  nowSec = timeRemaining % 60;
  nowMin = (timeRemaining / 60) % 60;
  nowHrs = (timeRemaining / 3600) % 24;
  nowDays = (timeRemaining / 86400);

  /* Serial.print(nowDays);
    Serial.print(nowHrs);
    Serial.print(nowMin);
    Serial.println(nowSec);
  */

  pixels.clear();

  draw(nowDays / 10, 0);
  draw(nowDays % 10, 5);
  draw(nowHrs / 10, 12);
  draw(nowHrs % 10, 17);
  draw(nowMin / 10, 24);
  draw(nowMin % 10, 29);
  draw(nowSec / 10, 36);
  draw(nowSec % 10, 41);

  if (timeRemaining % 2 == 1) {
    drawColon(10);
    drawColon(22);
    drawColon(34);
  }

  pixels.show(); // This sends the updated pixel color to the hardware
  if (settingTime == false) { // Won't update time while you're setting the clock
    timeRemaining -= 1; // Updates counter for what "now" is. One second has elapsed
  }
}


void drawColon(int x) { // draws the colons, change values manually if colors are off, should be yellow
  pixels.setPixelColor(pixArray[2][x], pixels.Color(b, b, r));
  pixels.setPixelColor(pixArray[4][x], pixels.Color(b, b, r));
}
