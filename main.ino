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
  delay(50);
}

void readSensors() {
  temperature += random(-50, 51) / 100.0;
  humidity += random(-100, 101) / 100.0;

  if (temperature < 10.0) temperature = 10.0;
  if (temperature > 40.0) temperature = 40.0;
  if (humidity < 30.0) humidity = 30.0;
  if (humidity > 90.0) humidity = 90.0;

  Serial.print("T: ");
  Serial.print(temperature);
  Serial.print(" H: ");
  Serial.println(humidity);

  sendData("SENSOR");
}

void updateGPS() {
  float lat = -23.5505 + (random(-100, 101) / 10000.0);
  float lon = -46.6333 + (random(-100, 101) / 10000.0);
  currentGPS = String(lat, 4) + ", " + String(lon, 4);

  Serial.print("GPS: ");
  Serial.println(currentGPS);

  sendData("GPS");
}

void sendData(String dataType) {
  String status = cellularSignal ? "CELULAR" : "SATELITE";

  String dataPacket = "{\"type\": \"" + dataType + "\", ";
  dataPacket += "\"timestamp\": " + String(millis()) + ", ";
  dataPacket += "\"gps\": \"" + currentGPS + "\", ";
  dataPacket += "\"temp\": " + String(temperature, 1) + ", ";
  dataPacket += "\"hum\": " + String(humidity, 1) + ", ";
  dataPacket += "\"panic\": " + String(panicActive ? "true" : "false") + ", ";
  dataPacket += "\"signal\": \"" + status + "\"}";

  Serial.println(dataPacket);
}

void updateSignalLEDs() {
  digitalWrite(ledCellularPin, cellularSignal ? HIGH : LOW);
  digitalWrite(ledSatellitePin, cellularSignal ? LOW : HIGH);
}
