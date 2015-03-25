// This sketch uses a DHT11 sensor to report temperature, humidity and dew point data to http://www.thingspeak.com.

#include <dht11.h>
#include <SPI.h>
#include <Ethernet.h>

// ThingSpeak Settings
char thingSpeakAddress[] = "184.106.153.149";
String writeAPIKey = "BS73DM5XUUUL56VB"; // Add your Thingspeak API key here
EthernetClient client;

// Temperature sensor settings
dht11 DHT11;
#define DHT11PIN 2
const int ONE_MINUTE = 30 * 1000;
int status;
int failedConnectionAttempCounter;
#include <LiquidCrystal.h>

// setup the lcd pins
LiquidCrystal lcd(3,4,5,6,7,8,9); // (rs, rw, enable, d4, d5, d6, d7)

void setup()
{
Serial.begin(9600);
Serial.println("DHT11 Temperature Sensor Program");
Serial.print("DHT11 library version: ");
Serial.println(DHT11LIB_VERSION);
Serial.println();
connectToInternet();

  // LCD setup 
  lcd.begin (2, 20);
  lcd.clear();
}


void connectToInternet()
{
if (client.connected())
{
client.stop();
}
Serial.println("Connecting to the internet via ethernet...");
// the media access control (ethernet hardware) address for the shield
// Leave this as is if your MAC address is not labelled on your ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
if (Ethernet.begin(mac) == 0) {
  Serial.println("Failed to configure Ethernet using DHCP");

}
  Serial.println(Ethernet.localIP());
}

int LightValue = 0;

void loop()
{

  //DHT11 diagnostics
Serial.println("\n");
int dht11ReadingStatus = DHT11.read(DHT11PIN);
Serial.print("Reading sensor...");
switch (dht11ReadingStatus)
{
case DHTLIB_OK:
Serial.println("Success!");
break;
case DHTLIB_ERROR_CHECKSUM:
Serial.println("Checksum error");
break;
case DHTLIB_ERROR_TIMEOUT:
Serial.println("Timeout error");
break;
default:
Serial.println("Unknown error");
break;
}


LightValue = analogRead(0); // reads voltage from light sensor


// call voids 
ReportToLCD(DHT11.temperature, DHT11.humidity);
ReportToSerialOut(DHT11.temperature, DHT11.humidity,LightValue);
ReportToThingspeak(DHT11.temperature, DHT11.humidity,LightValue);

}

//display data on LCD screen
void ReportToLCD(int temperature, int humidity)
{
lcd.clear();
lcd.setCursor(0,0);
   lcd.print("Temp:");
   lcd.print(temperature);
   lcd.print("C");

lcd.setCursor(0,1);
   lcd.print("Humidity:");
   lcd.print(humidity);
   lcd.print("%");
}


// send temp data to serial
void ReportToSerialOut(int temperature, int humidity, int LightValue)
{
Serial.print("Temperature (oC): ");
Serial.println((float)temperature, 2);
Serial.print("Humidity (%): ");
Serial.println((float)humidity, 2);
Serial.print("Raw light value: ");
Serial.println((float)LightValue, 2);
}

void ReportToThingspeak(int temperature, int humidity, int LightValue)
{
// Use short field names i.e. 1 instead of field1
String fields = "1=" + String(temperature, DEC);
fields += "&2=" + String(humidity, DEC);
fields += "&3=" + String(LightValue, DEC);

if (client.connect(thingSpeakAddress, 80))
{
Serial.println("Connected to thingspeak.com");
// Create HTTP POST Data
client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+writeAPIKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(fields.length());
client.print("\n\n");
client.print(fields);
Serial.print(fields);
Serial.print("\n");
Serial.println("Fields sent sent to www.thingspeak.com");
delay(ONE_MINUTE);
}
else
{
Serial.println("Connection to thingSpeak Failed");
Serial.println();
failedConnectionAttempCounter++;
// Re-start the ethernet connection after three failed connection attempts
if (failedConnectionAttempCounter > 3 )
{
Serial.println("Re-starting the ethernet connection...");
connectToInternet();
failedConnectionAttempCounter = 0;
}
}
}
