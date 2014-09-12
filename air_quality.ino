// Include the GSM library
#include <GSM.h>
#include "credentials.h"

// APN data - this is unique to the carrier
//#define GPRS_APN "wap.cingular" // AT&T
#define GPRS_APN "epc.tmobile.com" // T-Mobile
#define GPRS_LOGIN "admin" // this is not required by the carrier but by the library
#define GPRS_PASSWORD "password" // ditto

#define PINNUMBER "" 

// initialize the library instance
GSMClient client;
GPRS gprs;
GSM gsmAccess(false); // true for debugging

char server[] = "api.xively.com";

unsigned long lastConnectionTime = 0; 
boolean lastConnected = false;
const unsigned long postingInterval = 30*1000; // milliseconds 

// Setting up the PM Sensor
int pin = 8; // Yellow wire
unsigned long duration;
unsigned long starttime;
unsigned long sampletime_ms = 30000; //sampe 30s ;
unsigned long lowpulseoccupancy = 0;
float ratio = 0;
float concentration = 0;

void setup()
{
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);

  pinMode(8, INPUT);
  starttime = millis(); // get the current time
  Serial.println("Starting Arduino Shield");

  // connection state
  boolean notConnected = true;

  // Start GSM shield
  // If your SIM has PIN, pass it as a parameter of begin() in quotes
  while(notConnected)
  {
    if(gsmAccess.begin(PINNUMBER)==GSM_READY &
      (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
}

void loop()
{
  // as long as you're not sending anything, start counting
  if(!client.connected()){
    duration = pulseIn(pin, LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration;  
  }
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available())
  {
    char c = client.read();
    Serial.print(c);
  }
  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected)
  {
    client.stop();
  }
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && ((millis() - lastConnectionTime) > postingInterval))
  {
    Serial.println(lowpulseoccupancy);
    sendData(lowpulseoccupancy);
  }

  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();    

}

void sendData(unsigned long thisData)
{
  // if there's a successful connection:
  if (client.connect(server, 80))
  {
    Serial.println("connecting...");
    
    // Get character length of incoming data
    String dataLength = String(thisData);
    int dataLengthReal = dataLength.length();

    // Calculate the dust particle concentration and ratio
    ratio = thisData / (sampletime_ms*10.0); // Interger percentage 0=>100
    concentration = 1.1*pow(ratio,3)-3.8*pow(ratio,2)+520*ratio+0.62; //using spec sheet curve

    // For viewing in serial monitor
    Serial.print(thisData);
    Serial.print(",");
    Serial.print(ratio);
    Serial.print(",");
    Serial.println(concentration); 

    // Get the length of the concentration value
    int sendConcentration = int(concentration);
    String sendReal = String(sendConcentration);
    int sendRealReal = sendReal.length();

    // Total length of data being sent
    // 14 characters for the labels plus data lengths
    int sendLength = 14 + sendRealReal + dataLengthReal + 8;

    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");

    // This is the charachter length of the content
    client.println(sendLength);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print("lpo,");
    client.println(lowpulseoccupancy);

    client.print("ratio,");
    client.println(ratio);

    client.print("con,");
    client.println(concentration);
  } 
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // reset the count
  lowpulseoccupancy = 0;
  // note the time that the connection was made or attempted
  lastConnectionTime = millis();
}

