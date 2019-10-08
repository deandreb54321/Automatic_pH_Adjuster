/*
 * Author: Deandre Baker
 * Description: Source code for the automatic pH adjuster project
 */

// Includes the following libraries
#include "DHT.h"
#include "DHT_U.h"
#include <SPI.h>
#include <SD.h>

// Pins
const int chipSelect = 10;
const int pHPin = A0;
const int moisturePin = A3;
const int DHTPin = 5;
const int waterPin = 2;
const int acidPin = 3;
const int basePin = 4;

// Calibration offset value
const float calibration = 21.10;

unsigned long int avgValue;
float b;
int buf[10], temp;
bool dry = false;

int moistureThreshold = 2;
int optimalpH = 7;
int tolerance = 1;

float pH;
float moisture;
float temperature;
float humidity;
unsigned long timeElapsed;

// Creates the humidity and temperature sensor object
DHT dht(DHTPin, DHT11);

void setup()
{
  // Starts the serial communication with the computer and with the DHT sensor
  Serial.begin(9600);
  dht.begin();

  // Initializes pins
  pinMode(chipSelect, OUTPUT);    
  pinMode(waterPin, OUTPUT);
  pinMode(acidPin, OUTPUT);
  pinMode(basePin, OUTPUT);

  // Initalizes SD card adapter and halts program if not initialized
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    while (1);
  }
}

void loop()
{
  // Sets timeElapsed to the current run time of the program
  timeElapsed = millis();

  // Reads input from the serial monitor and sets the optimal pH level to what the user types in
  if (Serial.available()) {
    while (Serial.available() > 0) {
      optimalpH = Serial.readString().toInt();
      Serial.print("optimal pH is set to ");
      Serial.println(optimalpH);
    }
  }    

  // Runs following block of statements every ten seconds
  if(timeElapsed%(10*1000) == 0){

    // Measures the pH, moisture, temperature and humidity from the sensors
    pH = getpH();
    moisture = getMoisture();
    temperature = getTemperature();
    humidity = getHumidity();

    adjustMoisture();
    adjust_pH();

    // Creates the datalog file or opens an existing file
    File dataFile = SD.open("datalog.txt", FILE_WRITE); 

    // Writes following data to the file if file is available
    if (dataFile) {
      dataFile.print("Time = ");
      dataFile.print(timeElapsed / 1000);
      dataFile.print(" s");
      writeData(dataFile);
      Serial.print("Time = ");
      Serial.print(timeElapsed / 1000);
      Serial.print(" s");
      printData();
    }      
    else {
      Serial.println("error opening datalog.txt");
    }    
  }
}

// Measures the pH of the container
float getpH() {
  for (int i = 0; i < 10; i++)
  {
    buf[i] = analogRead(pHPin);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buf[i] > buf[j])
      {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++){
    avgValue += buf[i];
  }
  float pHVol = (float)avgValue * 5.0 / 1024 / 6;
  float phValue = -5.70 * pHVol + calibration;
  return phValue;
}

// Measures the moisture of the soil
int getMoisture(){
  float moisture = analogRead(moisturePin);
  moisture = map(moisture, 0, 1023, 0, 10);
  return (int) moisture;
}

// Measures the temperature of the air
float getTemperature(){
  return dht.readTemperature();
}

// Measures the humidity of the air
float getHumidity(){
  return dht.readHumidity();
}

// Adjusts the pH
void adjust_pH(){

  // Adds a basic solution if the pH is above the optimal range
  if(pH < optimalpH - tolerance){
    digitalWrite(basePin, HIGH);
    delay(1000);
    digitalWrite(basePin, LOW);
  }

  // Adds an acidic solution if the pH is below the optimal range
  else if(pH > optimalpH + tolerance){
    digitalWrite(acidPin, HIGH);
    delay(1000);
    digitalWrite(acidPin, LOW);
  }
}

// Adjusts the moisture of the soil
void adjustMoisture(){

  // Adds the pH adjusted water to the soil if below the moisture threshold
  if(moisture < moistureThreshold){
    digitalWrite(waterPin, HIGH);
    delay(2000);
    digitalWrite(waterPin, LOW);
  }
}

// Sends sensor data to the data file
void writeData(File dataFile){  
    dataFile.print("  pH = ");
    dataFile.print(pH);
    dataFile.print("  Moisture = ");
    dataFile.print(moisture);
    dataFile.print(" / 10");
    dataFile.print("  Temperature = ");
    dataFile.print(temperature);
    dataFile.print(" degrees C");
    dataFile.print("  Humidity = ");
    dataFile.print(humidity);
    dataFile.println(" %");
    dataFile.close();
}

// Sends sensor data to the serial monitor
void printData(){
    Serial.print("  pH = ");
    Serial.print(pH);
    Serial.print("  Moisture = ");
    Serial.print(moisture);
    Serial.print(" / 10");
    Serial.print("  Temperature = ");
    Serial.print(temperature);
    Serial.print(" degrees C");
    Serial.print("  Humidity = ");
    Serial.print(humidity);
    Serial.println(" %");
}
