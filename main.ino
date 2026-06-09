const int panicButtonPin = 2;
const int signalSwitchPin = 3;
const int ledCellularPin = 4;
const int ledSatellitePin = 5;

float temperature = 25.0;
float humidity = 60.0;
String currentGPS = "-23.5505, -46.6333";
bool panicActive = false;
bool cellularSignal = true;

unsigned long lastSensorRead = 0;
const long sensorInterval = 5000;
unsigned long lastGPSUpdate = 0;
const long gpsInterval = 10000;

void setup() {
  Serial.begin(9600);
  Serial.println("OrbitalTrack - Iniciado");

  pinMode(panicButtonPin, INPUT_PULLUP);
  pinMode(signalSwitchPin, INPUT_PULLUP);
  pinMode(ledCellularPin, OUTPUT);
  pinMode(ledSatellitePin, OUTPUT);

  updateSignalLEDs();
}

void loop() {
  if (millis() - lastSensorRead >= sensorInterval) {
    readSensors();
    lastSensorRead = millis();
  }

  if (millis() - lastGPSUpdate >= gpsInterval) {
    updateGPS();
    lastGPSUpdate = millis();
  }

  if (digitalRead(panicButtonPin) == LOW) {
    if (!panicActive) {
      panicActive = true;
      Serial.println("!!! PANIC !!!");
      sendData("PANIC");
       }
  } else {
    if (panicActive) {
      panicActive = false;
      sendData("PANIC_OFF");
    }
  }

  if (digitalRead(signalSwitchPin) == LOW) {
    if (cellularSignal) {
      cellularSignal = false;
      Serial.println("Sinal: Satelite");
      updateSignalLEDs();
    }
  } else {
    if (!cellularSignal) {
      cellularSignal = true;
      Serial.println("Sinal: Celular");
      updateSignalLEDs();
    }
  }
}