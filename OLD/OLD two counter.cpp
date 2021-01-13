if (stopwatchMode) { // Two counters mode
        if (stopwatchHours >= 1) { // Switch to three counters mode if hours
            display.clearDisplay();
            stopwatchMode = !stopwatchMode;
        }

        display.setTextSize(4);
    
        display.setCursor(12,18);
        if (stopwatchMin <= 9) { // If less than ten minutes print 0 before ones digit
                display.print("0");
                display.setCursor(36,18);
                display.print(stopwatchMin);
        }
        else
            display.print(stopwatchMin); // Over ten minutes, print both digits
    
        display.setCursor(12+40,18);
        display.print(":");
    
        display.setCursor(12+ 56,18);
        if (stopwatchSec < 10) {
            display.print("0");
            display.setCursor(12+56+24,18);
            display.print(stopwatchSec);
        }
        else {
            display.print(stopwatchSec);
        }
        //display.print("mm:ss");
        
    }
