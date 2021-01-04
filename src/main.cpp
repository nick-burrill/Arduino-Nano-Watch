#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO/NANO:       A4(SDA), A5(SCL)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int app = -1; // Defaults to -1 for Menu
int stopwatchTime = 3200; // Stopwatch time elapsed
int select = 1; // Defaults to 1 for Stopwatch

// Button values per button:
// Up = 2, Down = 4, Enter = 8, Escape = 16
int btnPressed = 0;
// Add respective value for reach button to be pressed
// Button pins will be set here aswell
int pinUp = 10;
int pinDown = 9;
int pinEnter = 8;
int pinEscape = 7;

String tempString = "sample"; // Used for serial monitor input

void setup() {
    Serial.begin(9600);
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    
    Serial.println("Setup done!"); // Redundant but fun :)
}

void drawClock() {
    if (btnPressed == 16) // Return to menu on ESC
        app = -1;
    
    // Clear the display 
    display.clearDisplay();

    //Set the color - always use white despite actual display color
    display.setTextColor(WHITE);
 
    //Draw Date
    display.setTextSize(2);
    display.setCursor(8,0);
    display.print("Wed Dec 3");
    
    //Draw Clock
    display.setCursor(0,18);
    display.setTextSize(5);
    display.print("8:30");

    //Draw PM / AM
    display.setCursor(120,27);
    display.setTextSize(1);
    display.print("P");
    display.setCursor(120,36);
    display.print("M");
     
    //Draw Serial Status   
    display.setTextSize(1);
    display.setCursor(0,56);
    display.print("Serial: ");

}

void drawMenu() {
    // Setup string array for menu list
    const char *menu[5] = {"Back", "Timer", "Alarm", "Stopwatch", "Counter"};
    
    // Prepare for input
    if (btnPressed == 2) // Move selection up
        select = select - 1;

    if (btnPressed == 4) // Move selection down
        select = select + 1;
    
    if (btnPressed == 8) // Switch to selected app
        app = select;
     
     if (btnPressed == 16) // Return to clock from menu
        app = 0;       
    
    display.clearDisplay();

    // Draw the title
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.println("Main Menu");
    
    // Draw three lines of text
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

void drawStopwatch() {
    if (btnPressed == 16) // Return to menu on ESC
        app = -1;
    
    display.clearDisplay();

    //Draw the title
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.println("Stopwatch");
    
    if (stopwatchTime < ((59 * 60) + 59)) { // Draw the minites counter if LESS than 5599 seconds have gone by
        display.setTextSize(4);
    
        display.setCursor(12,18);
        display.print("00");
    
        display.setCursor(12+40,18);
        display.print(":");
    
        display.setCursor(12+ 56,18);
        display.print("00");
        //display.print("mm:ss");
    }

    if (stopwatchTime > ((59 * 60) + 59)) { //Draw the hours counter if OVER 
        display.setTextSize(3);
    
        display.setCursor(0,25);
        display.print("00");
    
        display.setCursor(30,25);
        display.print(":");
    
        display.setCursor(42,25);
        display.print("00");
    
        display.setCursor(72,25);
        display.print(":");
    
        display.setCursor(84,25);
        display.print("00");
        //display.print(" hr:mm:ss");
    } 

    // Draw the menu options
    display.setTextSize(2);
    display.setCursor((128-(6 * 9 * 2))/2,64/8*6 + 2);
    display.print("> Start <");

}

void serialInput() {
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
}

void buttonInput() {
    // For now this will be activated by void loop but eventually should be a hardware interupt

}

void loop() {
    delay(250); // Delay to avoid serial spam
    
    serialInput(); // Check for serial input as means of simulating button presses
    
    switch (app) { // Select screen based on app index
    case -1:
        drawMenu();
        break;
    case 0:
        drawClock();
        break;
    case 1:
        Serial.println("Not yet implimented...");
        app = -1;
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
    // Some debug shit
    //Serial.print("Button: ");
    //Serial.println(btnPressed);
    //Serial.print("Selection: ");
    //Serial.println(select);

    btnPressed = 0; // Reset button state  
    display.display(); //Refresh the display
}