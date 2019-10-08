// Imports following libraries
import processing.serial.*;
import java.awt.event.KeyEvent;
import java.io.IOException;

// Creates an instance of the serial object
Serial myPort;

// Global variables
String data = "";
String time = "";
String pH = "";
String moisture = "";
String temperature = "";
String humidity = "";

void setup()
{
  // Creates the GUI background
  size(1920, 1080);
  smooth();
  background(0);
 
  // Creates the Serial object
  myPort = new Serial(this, "COM13", 9600);
  myPort.bufferUntil('\n');
}

void draw()
{
  
  while(myPort.available() > 0)
  {
    delay(1000);
    // Reads data from serial monitor
    data = (myPort.readStringUntil('\n'));
    
    // Resets background color to black and text color to white
    background(0);
    color(255);
    textSize(50);
    
    // Assigns each variable the corresponding data
    time = data.substring(0, data.indexOf("pH"));    
    pH = data.substring(data.indexOf("pH"), data.indexOf("Moisture"));
    moisture = data.substring(data.indexOf("Moisture"), data.indexOf("Temperature"));
    temperature = data.substring(data.indexOf("Temperature"), data.indexOf("Humidity"));
    humidity = data.substring(data.indexOf("Humidity"), data.indexOf('%') + 1);
        
    // Displays the data as text to the screen
    text(time, width / 5, height * 1 / 6);
    text(pH, width / 5, height * 2 / 6);
    text(moisture, width / 5, height * 3 / 6);
    text(temperature, width / 5, height * 4 / 6);
    text(humidity, width / 5, height * 5 / 6);
  }
  
 
}
