#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int app = 0; //Defaults to 0 for clock
int stopwatchTime = 3200;

String tempString = "sample";

void setup() {
    Serial.begin(9600);
    Wire.begin();
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    Serial.println("Test ping");
}

void drawClock() {
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
    display.clearDisplay();

    //Draw the title
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.println("Main Menu");
    
    //Draw the options
    display.println(" > Timer <");
    display.println("   Alarm");
    display.println(" Stopwatch");
    //display.println("> Counter");
}

void drawStopwatch() {
    display.clearDisplay();

    //Draw the title
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.println("Stopwatch");
    
    //Draw the minites counter
    if (stopwatchTime < ((59 * 60) + 59)) {
        display.setTextSize(4);
    
        display.setCursor(12,18);
        display.print("00");
    
        display.setCursor(12+40,18);
        display.print(":");
    
        display.setCursor(12+ 56,18);
        display.print("00");
        //display.print("mm:ss");
    }


    //Draw the hours counter
    if (stopwatchTime > ((59 * 60) + 59)) {
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

    //Draw the menu options
    display.setTextSize(2);
    display.setCursor((128-(6 * 9 * 2))/2,64/8*6 + 2);
    display.print("> Start <");

}

void serialInput() {
    if (Serial.available() > 0) {
        tempString = Serial.readString();
        app = tempString.toInt();
        Serial.println(app);
    }
}

void loop() {
    //Check for serial input as means of simulating button presses
    serialInput();
    
    //Select screen based on app index
    if (app == 0) {
        drawClock();
    }
    if (app == 1) {
        drawMenu();
    }
    if (app == 2) {
        drawStopwatch();
    }


    //Refresh the display
    display.display();
}