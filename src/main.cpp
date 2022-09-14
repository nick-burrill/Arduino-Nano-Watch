// Sept 14th 2022, RAM:   [====      ]  39.4% (used 807 bytes from 2048 bytes) Flash: [=======   ]  66.6% (used 20468 bytes from 30720 bytes)
#include <SPI.h> // Display communications
#include <Wire.h> // Display communications
#include <Adafruit_GFX.h> // Graphics library
#include <Adafruit_SSD1306.h> // Display library

#include <Arduino.h>
#include <ThreeWire.h> // RTC Communications
#include <RtcDS1302.h> // RTC Library

ThreeWire myWire(4,7,3); // IO/DAT, SCLK/CLK, CE/RST
RtcDS1302<ThreeWire> Rtc(myWire); // Clock pins

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define countof(x) (sizeof(x) / sizeof (x[0]))

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO/NANO:       A4(SDA), A5(SCL)
#define OLED_RESET 4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// System
int app = 0; // -1 = Menu, 0 = Clock, 1 = Alarm, 2 = Timer, 3 = Stopwatch, 4 = Counter      default = 0
int tick = 100; // Delay in main loop
unsigned long timeOld = 0; // Used to derive how long things have taken
// bool drawnYet = false;
int select = 1; // Menu Selection
// int refreshRate = 0; UNUSED
//String tempString = "sample"; // Used for serial monitor input

bool am = true; // AM = True, PM = False
char clockString[9]; // character length + 1
char dateString[11];

// Input
int btnPressed = 0; // Up = 2, Down = 4, Enter = 8, Escape = 16
int btnOld;
// Button pins
int pinUp = 5;
int pinDown = 8;
int pinEnter = 6;
int pinEscape = 9;
// Time selector
int setSec = 0;   
int setMin = 0;
int setHour = 0;
int currentValue = 1; // 1 = hr, 2 = min, 3 = sec
int editedValue = 0; 

bool stopwatchState; // True = stopwatch running, False = stopwatch paused

// Counter Function
int slot1old = -1;
int slot2old = -1;
int slot3old = -1;

const unsigned char letterP [] PROGMEM = { // 'p', 9x9px
	0x7f, 0x00, 0xff, 0x80, 0xc1, 0x80, 0xc1, 0x80, 0xff, 0x80, 0xff, 0x00, 0xc0, 0x00, 0xc0, 0x00, 
	0xc0, 0x00
};

const unsigned char letterM [] PROGMEM = { // 'm', 9x9px
	0x80, 0x80, 0xc1, 0x80, 0xe3, 0x80, 0xf7, 0x80, 0xdd, 0x80, 0xc9, 0x80, 0xc1, 0x80, 0xc1, 0x80, 
	0xc1, 0x80
};

void getTimeStrings(const RtcDateTime& dt) // Fetches clockString and dateString from RTC
{
    int twelveHour;
    if (dt.Hour() >= 12) {
        if (dt.Hour() > 12) {
            twelveHour = dt.Hour() - 12;
        }
        else { twelveHour = dt.Hour(); }
        am = false;
    }
    snprintf_P(clockString, 
            countof(clockString),
            PSTR("%02u:%02u:%02u"),
            // dt.Month(),
            // dt.Day(),
            // dt.Year(),
            twelveHour,
            dt.Minute(),
            dt.Second() );
    snprintf_P(dateString, 
            countof(dateString),
            PSTR("%02u/%02u/%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year()
            // dt.Hour(),
            // dt.Minute(),
            // dt.Second() 
            );
}

void setup() {
    Serial.begin(9600);
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    display.clearDisplay();

    pinMode(pinUp, INPUT);
    pinMode(pinDown, INPUT);
    pinMode(pinEnter, INPUT);
    pinMode(pinEscape, INPUT);

    RtcDateTime now = Rtc.GetDateTime();
    getTimeStrings(now);

    Serial.println(dateString);
    Serial.println(clockString);

    Serial.print("Setup done! Took: "); // Redundant but fun :)
    Serial.println(millis()); // Boot Time: 40ms
}

void drawTitle(String title, bool pos = 0, bool highlight = 0) {
    int y = 0;
    if (pos == 1) { y = 48; }
    else {display.clearDisplay(); }
    int x = (SCREEN_WIDTH/2)-(title.length()*6); // centering doesnt work for clock yet - Are you sure?
    display.setCursor(x,y); 
    display.setTextSize(2);
    if (highlight == 0) { display.setTextColor(WHITE, BLACK); }
    else { display.setTextColor(BLACK, WHITE); }
    
    display.setTextWrap(false);
    display.print(title);
}

void drawCounter(int slot1, int slot2, int slot3, int highlight) { 
    display.setTextSize(3);
    if (slot1 != slot1old) {
        //update slot1 if value has changed
        if (highlight == 1) { display.setTextColor(BLACK, WHITE); } // Highlight if selected
        else { display.setTextColor(WHITE, BLACK); }
        if (slot1 < 10) { // If less than 10, draw 0 before it
            display.setCursor(2,22);
            display.print("0");
            display.setCursor(21,22);
            display.print(slot1);
        }
        else {
            display.setCursor(2,22);
            display.print(slot1);
        }
    }

    if (slot2 != slot2old) {
        //update slot2 if value has changed
        if (highlight == 2) { display.setTextColor(BLACK, WHITE); } // Highlight if selected
        else { display.setTextColor(WHITE, BLACK); }
        if (slot2 < 10) { // If less than 10, draw 0 before it
            display.setCursor(47,22);
            display.print("0");

            display.setCursor(66,22);
            display.print(slot2);
        }
        else {
            display.setCursor(47,22);
            display.print(slot2);
        }
    }
    if (slot3 != slot3old) {
        //update slot3 if value has changed
        if (highlight == 3) { display.setTextColor(BLACK, WHITE); } // Highlight if selected
        else { display.setTextColor(WHITE, BLACK); }
        if (slot3 < 10) { // If less than 10, draw 0 before it
            display.setCursor(92,22);
            display.print("0");
            display.setCursor(111,22);
            display.print(slot3);
        }
        else {
            display.setCursor(92,22);
            display.print(slot3);
        }
    }
    display.setTextColor(WHITE);
    if ((slot1 != slot1old) or (slot2 != slot2old)) { // Check if inbetween slot1 and slot2 has been drawn over
        // Draw colons
        display.setCursor(34,22);
        display.print(":");
    }
    if ((slot2 != slot2old) or (slot3 != slot3old)) { // Check if inbetween slot2 and slot3 has been drawn over
        // Seccond colon
        display.setCursor(79,22);
        display.print(":");
    } 

}

void drawClock() {
    // Clock

    if (btnPressed == 16) // Return to menu on ESC
        app = -1;
    
    RtcDateTime now = Rtc.GetDateTime();
    getTimeStrings(now);

    display.setTextColor(WHITE, BLACK);
    drawTitle(dateString); // BUG For some reason needs to be printed before time, after color setting, or time will not show up

    //Draw Clock
    display.setTextSize(5);

    if (clockString[0] != '0') {
    display.setCursor(-5,18);
    display.print(clockString[0]); }

    display.setCursor(40,18);
    display.print(":");

    display.setCursor(20,18);
    display.print(clockString[1]);

    display.setCursor(60,18);
    display.print(clockString[3]);
    display.print(clockString[4]);

    //Draw PM / AM
    display.drawBitmap(117, 25,letterP, 9, 9, 1);
    display.drawBitmap(117, 37,letterM, 9, 9, 1);
    if (am == true) {
        display.drawRect(124,30,2,4,1); 
    }
     
    //Draw Serial Status   
    display.setTextSize(1);
    display.setCursor(0,56);
    display.print("Sunset in 4hr 30m");

}

void drawMenu() {
    // Setup string array for menu list
    const char *menu[5] = {"Back", "Timer", "Alarm", "Stopwatch", "Counter"};
    
    // Prepare for input
    if ((btnPressed == 2) && (select > 0)) {// Move selection up
        select = select - 1; 
    }

    if ((btnPressed == 4) && (select < 4)) { // Move selection down
        select = select + 1; 
    }

    if (btnPressed == 8) // Switch to selected app
        app = select;
     
     if (btnPressed == 16) // Return to clock from menu
        app = 0;       
    
    drawTitle("Main Menu");
    
    // Draw three lines of text
    display.setTextSize(2);
    display.setCursor(0,16);
    for (int i = 1; i < 4; i++) {
        //Serial.println(i);
        if (i == select) { // Highlight the selected option
            display.setTextColor(BLACK, WHITE);
            display.println(menu[i]);
            display.setTextColor(WHITE, BLACK);
        }
        else
        display.println(menu[i]);    
    }
}

void drawTimeSet() {
    drawTitle("Set time:");
    if ((setHour == 0) && (setMin == 0) && (setSec == 0)) {
        drawTitle("Cancel",1,0);
    }
    else { drawTitle("Submit",1,0); }

    switch (btnPressed) { // Button input
    case 2:
        editedValue = editedValue + 1;
        break;
    case 4:
        editedValue = editedValue - 1;
        break;
    case 8:
        currentValue = currentValue + 1;
        btnPressed = 0;
        break;
    case 16:
        currentValue = currentValue - 1;
        break;
    default:
        break;
    }
    
    switch (currentValue) { // Cursor movement
    case 0:
        drawTitle("Cancel",1,1);
        if ((btnPressed == 16) or (btnPressed == 8)) {
            Serial.println("Canceled time selection");
            app = -1;
            // drawnYet = false;
        }
        break;
    case 1:
        setHour = editedValue;
        break;
    case 2:
        setMin = editedValue;
        break;
    case 3:
        setSec = editedValue;
        break;
    case 4:
        drawTitle("Submit",1,1);
        if (btnPressed == 8) {
            Serial.println("Submit/Cancel time");
            app = -1;
            // drawnYet = false;
        }
    default:
        break;
    }

    drawCounter(setHour, setMin, setSec, currentValue);

}

void drawStopwatch() {
    // Stopwatch
    unsigned long stopwatchTime; // Stopwatch time elapsed in millisecconds     default = 0
    unsigned long stopwatchStart;
    
    //bool stopwatchMode; // True = Double counter, False = tripple counter

    int stopwatchMili; // Easily readible milisecconds counter
    int stopwatchSec; // Easiliy readable secconds counter
    int stopwatchMin; // Easily readable minutes counter
    int stopwatchHours; // Easily readable minutes counter
    
    if (btnPressed == 8) { // Toggle stopwatch
        if (stopwatchState == 0); { stopwatchStart = millis(); } 
        stopwatchState = !stopwatchState;
    }
    if (stopwatchState) { // Add tick to time. This is temp
        stopwatchTime = millis() - stopwatchStart;
        stopwatchMili = stopwatchTime % 1000 / 10; 
        stopwatchSec = stopwatchTime / 1000 % 60; 
        stopwatchHours = stopwatchTime / 3600000;
        stopwatchMin = stopwatchTime / 60000 - stopwatchHours * 60;

        /*Serial diagnostics
        //Serial.print("Mili: ");
        //Serial.println(stopwatchMili);
        
        if (stopwatchMili > 90) {
            
            Serial.print("Total: ");
            Serial.println(stopwatchTime);
            
            Serial.print("Sec: ");
            Serial.println(stopwatchSec);
            Serial.print("Min: ");
            Serial.println(stopwatchMin);
            Serial.print("Hour: ");
            Serial.println(stopwatchHours);
            Serial.println("-------------");
        } */
    }
    
    if (btnPressed == 16) { // Return to menu on ESC
        app = -1;
    }
    
    drawTitle("Stopwatch");
    if (stopwatchState == 1) {
        drawTitle("Pause",1,0);
    }
    if (stopwatchState == 0) {
        drawTitle("Start",1,1);
    }
    
    // Three counters mode
    if (stopwatchHours < 1) { // Draw min:sec:mili
        drawCounter(stopwatchMin, stopwatchSec, stopwatchMili, 0);
    }
    else { // Draw hour:min:sec
        drawCounter(stopwatchHours, stopwatchMin, stopwatchSec, 0);
    }
    /*if (drawnYet == false) {
        // Draw the menu options
        display.setTextSize(2);
        display.setCursor((128-(6 * 9 * 2))/2,64/8*6 + 2);
        if (stopwatchState)
            display.print("> Pause <");
        if (!stopwatchState)
            display.print("> Start <");
    } */
    //drawnYet = true;

}

/* void serialInput() {
    if (Serial.available() > 0) {
        tempString = Serial.readString();
        
        if (tempString == "w") { // Emulated "UP" key
            Serial.println(tempString);
            btnPressed = 2;
        }
        if (tempString == "s") { // Emulated "DOWN" key
            Serial.println(tempString);
            btnPressed = 4;
        }
        if (tempString == "e") { // Emulated "ENTER" key
            Serial.println(tempString);
            btnPressed = 8;
        }
        if (tempString == "q") { // Emulated "ESCAPE" key
            Serial.println(tempString);
            btnPressed = 16;
        }
    }
} */

void buttonInput() {
    // For now this will be activated by void loop but eventually should be a hardware interupt

    
    if (digitalRead(pinUp) == HIGH) {
        btnPressed = btnPressed + 2;   
    }
    if (digitalRead(pinDown) == HIGH) {
        btnPressed = btnPressed + 4;   
    }
    if (digitalRead(pinEnter) == HIGH) {
        btnPressed = btnPressed + 8;   
    }
    if (digitalRead(pinEscape) == HIGH) {
        btnPressed = btnPressed + 16;   
    }
    if (btnPressed == 0) {
        btnOld = 0;
    }
    if (btnPressed == btnOld) {
        btnPressed = 0;
    }
    if (btnPressed != 0) {
        btnOld = btnPressed;
        
        Serial.print("Button: ");
        Serial.println(btnPressed);
        Serial.print("Button old: ");
        Serial.println(btnOld);
    }
}

void loop() {
    // myRTC.updateTime();
    buttonInput();
    //serialInput(); // Check for serial input as means of simulating button presses
    
    //if (btnPressed != 0) {
    if ((millis() > timeOld + tick) or (btnPressed != 0)) {
        switch (app) { // Select screen based on app index
        case -1:
            drawMenu();
            break;
        case 0:
            drawClock();
            break;
        case 1:
            drawTimeSet();
            //Serial.println("Not yet implimented...");
            //app = -1;
            break;
        case 2:
            Serial.println("Not yet implimented...");
            app = -1;
            break;
        case 3:
            drawStopwatch();
            break;
        default:
            Serial.println("Bad app selection!"); // This is bad lol
            break;
        }
        // Serial.println(millis()-timeOld-tick); print the response time
        display.display(); //Refresh the display
        timeOld = millis();
    }
    
    /* Some debug shit
    Serial.print("Button: ");
    Serial.println(btnPressed);
    Serial.print("Selection: ");
    Serial.println(select); */
    
    btnPressed = 0; // Reset button state  

}