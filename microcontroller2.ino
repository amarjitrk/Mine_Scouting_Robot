
int gasSensor, LDR, tiltSensor;
float temperatureSensor;
int piezo = 7;
int LED_light = 6;
int LED_tilt = 11;
float vout1, temperature;
int RED = 10;
int BLUE = 9;
int GREEN = 8;
String ssid = "Simulator Wifi";     // SSID to connect to
String password = "";               // virtual wifi has no password
String host = "api.thingspeak.com"; // Open Weather Map API
const int httpPort = 80;
String url = "/update?api_key=07AFN60W7F2DQK9Q&field1=";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
void setup()
{
    pinMode(A0, INPUT);
    pinMode(A1, INPUT);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);
    pinMode(RED, OUTPUT);
    pinMode(BLUE, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(piezo, OUTPUT);
    pinMode(LED_light, OUTPUT);
    pinMode(LED_tilt, OUTPUT);
    setupESP8266();
}
void setupESP8266(void)
{

    // Start our ESP8266 Serial Communication
    Serial.begin(115200); // Serial connection over USB to computer
    Serial.println("AT"); // Serial connection on Tx / Rx port to ESP8266
    delay(10);            // Wait a little for the ESP to respond
    if (Serial.find("OK"))
        Serial.println("ESP8266 OK!!!");

        // Connect to Simulator Wifi
        Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + password + "\"");
    delay(10); // Wait a little for the ESP to respond
    if (Serial.find("OK"))
        Serial.println("Connected to WiFi!!!");

    // Open TCP connection to the host:
    // ESP8266 connects to the server as a TCP client.

    Serial.println("AT+CIPSTART=\"TCP\",\"" + host + "\"," + httpPort);
    delay(50); // Wait a little for the ESP to respond
    if (Serial.find("OK"))
        Serial.println("ESP8266 Connected to server!!!");
}

void loop()
{
    anydata();
    delay(1000);
    serial();
    delay(1000);
}

void anydata(void)
{

    gasSensor = analogRead(A0);
    LDR = analogRead(A1);
    temperatureSensor = analogRead(A2);
    vout1 = (temperatureSensor / 1023) * 5000;
    temperature = (vout1 - 500) / 10;
    tiltSensor = digitalRead(A3);
    String httpPacket = "GET " + url + String(temperature) + "&field2=" + String(gasSensor) + "&field3=" + String(tiltSe nsor) + "&field4=" + String(LDR) + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";

    int length = httpPacket.length();
    Serial.print("AT+CIPSEND=");
    Serial.println(length);
    delay(10); // Wait a little for the ESP to respond if (!Serial.find(">")) return -1;

    // Send our http request

    Serial.print(httpPacket);
    delay(10); // Wait a little for the ESP to respond
    if (!Serial.find("SEND OK\r\n"))
        return;
}

void serial(void)
{
    gasSensor = analogRead(A0); // ideally between 0 and 1023
    LDR = analogRead(A1);
    temperatureSensor = analogRead(A2);
    vout1 = (temperatureSensor / 1023) * 5000; // Convert to mV
    temperature = (vout1 - 500) / 10;
    tiltSensor = digitalRead(A3);
    Serial.print("Gas Sensor: ");
    Serial.println(gasSensor);
    if (gasSensor >= 200)
    {
        digitalWrite(piezo, HIGH);
        Serial.println("High concentrations of gas! Care required! ");
    }
    else
    {
        digitalWrite(piezo, LOW);
    }
    Serial.print("Ambient Light: ");
    Serial.println(LDR);
    if (LDR < 100)
    {
        analogWrite(LED_light, 255);
    }
    else if ((100 <= LDR) & (LDR <= 500))
    {
        analogWrite(LED_light, 50);
    }
    else
    {
        analogWrite(LED_light, 0);
    }
    Serial.print("The temperature is: ");
    Serial.print(temperature);
    Serial.println(" degrees C");
    if (temperature > 40)
    {
        digitalWrite(RED, HIGH);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, LOW);
        Serial.println("Possible fire hazard!");
    }
    else if ((temperature >= 15) & (temperature <= 40))
    {
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, HIGH);
        digitalWrite(BLUE, LOW);
    }
    else
    {
        digitalWrite(RED, LOW);
        digitalWrite(GREEN, LOW);
        digitalWrite(BLUE, HIGH);
    }
    if (tiltSensor == 0)
    {
        digitalWrite(LED_tilt, HIGH);
        Serial.println("Uneven Ground! Caution Required! ");
    }
    else
    {
        digitalWrite(LED_tilt, LOW);
        Serial.println("No significant slope");
    }
}