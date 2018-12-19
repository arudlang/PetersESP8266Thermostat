#include <DHT.h>

#define DHTPIN D1
#define RELAYPIN D2
#define DHTTYPE DHT22

DHT DHT1(DHTPIN, DHTTYPE);

float MostRecentTempRead = 0.0;
const int tempSmoothing = 5;                       // How many readings to take a running average from
float tempReadings[tempSmoothing];                   // the readings from the analog input
int tempReadIndex = 0;                             // the index of the current reading
float tempReadingsTotal = 0.0; 
float currentAverageTemp = 0.0;

bool RelayON = false;

float LowTemp = 72.0;
float HighTemp = 74.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Startup Complete");

  pinMode(RELAYPIN, OUTPUT);
  
  digitalWrite(RELAYPIN, LOW);

  // Initialize all the temp readings to 60:
  for (int i = 0; i <= tempSmoothing; i++) {
    tempReadings[i] = 60.0;
  }

  tempReadingsTotal = 60.0 * tempSmoothing;
  
  DHT1.begin();
 
}

void loop() {
  // put your main code here, to run repeatedly:
  updateTempAverage();
  
  if(RelayON == true){
    // Check to see if conditions to shut off are met:
    if (currentAverageTemp > HighTemp){
        RelayON = false;
        digitalWrite(RELAYPIN, LOW);
    }
  } else {
    // Check to see if conditions to call for heat are met:
    if (currentAverageTemp < LowTemp){
        RelayON = true;
        digitalWrite(RELAYPIN, HIGH);
    }
  }

  //updateRelayStatus(RelayON);

  Serial.println("LastReading: "+String(MostRecentTempRead));
  Serial.println("Average: "+String(currentAverageTemp));
  Serial.println("RelayStatus: "+String(RelayON));
  Serial.println("");
  delay(1000); 
}


void updateRelayStatus(bool turnON){
    if (turnON == true){
      digitalWrite(RELAYPIN, HIGH);
    }else{
      digitalWrite(RELAYPIN, LOW);
    }
}

void updateTempAverage(){
  // Get a reading from the TEMP module
  MostRecentTempRead = DHT1.readTemperature(true);
  
  if (isnan(MostRecentTempRead)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  // Mix latest reading into the running average
  // remove the last reading
  tempReadingsTotal = tempReadingsTotal - tempReadings[tempReadIndex];
  // put the new reading in
  tempReadings[tempReadIndex] = MostRecentTempRead;
  // add the reading to the total
  tempReadingsTotal = tempReadingsTotal + tempReadings[tempReadIndex];
  // advance to the next position in the array
  tempReadIndex = tempReadIndex + 1;

  // if we're at the end of the array...
  if (tempReadIndex >= tempSmoothing) {
    // ...wrap around to the beginning:
    tempReadIndex = 0;
  }

  // calculate the average
  currentAverageTemp = tempReadingsTotal / tempSmoothing;  
}
