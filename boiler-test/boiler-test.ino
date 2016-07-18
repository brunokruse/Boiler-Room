/*
	Boiler Room testing software
*/

#include <FileIO.h>
#include <Process.h>
#include <OneWire.h>
#include <DallasTemperature.h> // DS18B20

// phant setup
//http://ec2-54-218-89-249.us-west-2.compute.amazonaws.com:8080/streams/xp9WjxBl1pUlOjVMreJoUMdpxOM
// deleteKey = "7rN8yn0JArixdw7mB2R4Ua0GeLa";

String phantURL = "http://ec2-54-218-89-249.us-west-2.compute.amazonaws.com:8080/input/"; // AWS 8080
String publicKey = "xp9WjxBl1pUlOjVMreJoUMdpxOM"; // public
String privateKey = "gxqMzyZ9WxUmWv5j8Re4ieQ4l3e"; // private

const int NUM_FIELDS = 8;
String fieldName[NUM_FIELDS] = { "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8" };
String fieldData[NUM_FIELDS]; // array to store our field data
int postInterval = 10000; // how often to post (every minute)

// one wire + dallas temperature
#define ONE_WIRE_BUS_PIN 4
OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature sensors(&oneWire);

DeviceAddress temp1 = { 0x28, 0x34, 0xB5, 0x5F, 0x07, 0x00, 0x00, 0x8A }; // ok
DeviceAddress temp2 = { 0x28, 0x3C, 0x63, 0x60, 0x07, 0x00, 0x00, 0x3B }; // ok
DeviceAddress temp3 = { 0x28, 0x2E, 0x81, 0x5F, 0x07, 0x00, 0x00, 0x85 }; // ok
DeviceAddress temp4 = { 0x28, 0x05, 0xC2, 0x5F, 0x07, 0x00, 0x00, 0x6C }; // ok
DeviceAddress temp5 = { 0x28, 0x9E, 0x45, 0x5F, 0x07, 0x00, 0x00, 0xBF }; // ok
DeviceAddress temp6 = { 0x28, 0x49, 0x93, 0x5F, 0x07, 0x00, 0x00, 0x3C }; // ok
DeviceAddress temp7 = { 0x28, 0x4B, 0x18, 0x60, 0x07, 0x00, 0x00, 0x3B }; // ok
DeviceAddress temp8 = { 0x28, 0x1F, 0xED, 0x5F, 0x07, 0x00, 0x00, 0x6F }; // ok

// LED
int ledPin = 9;

void setup() {
  
  // LED Debugging
  pinMode(ledPin, OUTPUT);
  
  ledOn();

  Bridge.begin();

  Serial.begin(9600);

  while (!Serial); // arduino yun hack

  initSensors(); // initialize the DS18B20 sensors

  FileSystem.begin(); // write to SD card YO

  while (!SerialUSB);
  
  SerialUSB.println("Boiler room datalogger\n");

  ledOff();
}

void initSensors() {

  Serial.print("Initializing Temperature Control Library Version ");
  Serial.println(DALLASTEMPLIBVERSION);

  sensors.begin();

  // set the resolution to 10 bit (Can be 9 to 12 bits .. lower is faster
  sensors.setResolution(temp1, 10);
  sensors.setResolution(temp2, 10);
  sensors.setResolution(temp3, 10);
  sensors.setResolution(temp4, 10);
  sensors.setResolution(temp5, 10);
  sensors.setResolution(temp6, 10);
  sensors.setResolution(temp7, 10);
  sensors.setResolution(temp8, 10);

  Serial.println("Sensors have been set.");
}

void loop() {

  Serial.println("Grabbing data from sensors.");
  getDataFromSensors();

  Serial.println("Writing most recent data to SD");
  writeToSD();

  //Serial.println("Posting Data to server.");
  postData();

  // we don't want to do it too often
  delay(postInterval);

}

void ledOn() {
  digitalWrite(ledPin, HIGH);
}

void ledOff() {
  digitalWrite(ledPin, LOW);
}

void ledBlink() {
  // 3 blinks
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
}

void getDataFromSensors() {

  /*

  	DUMMY DATA

    fieldData[0] = String("90");
    fieldData[1] = String("90");
    fieldData[2] = String("90");
    fieldData[3] = String("90");
    fieldData[4] = String("90");
    fieldData[5] = String("90");
    fieldData[6] = String("90");
    fieldData[7] = String("90");
    fieldData[8] = String("90");


  */


  Serial.println();
  Serial.print("Number of Devices found on bus = ");
  Serial.println(sensors.getDeviceCount());
  Serial.print("Getting temperatures... ");
  Serial.println();

  // Command all devices on bus to read temperature
  sensors.requestTemperatures();

  // Data from sensors
  fieldData[0] = DallasTemperature::toFahrenheit(sensors.getTempC(temp1));
  fieldData[1] = DallasTemperature::toFahrenheit(sensors.getTempC(temp2));
  fieldData[2] = DallasTemperature::toFahrenheit(sensors.getTempC(temp3));
  fieldData[3] = DallasTemperature::toFahrenheit(sensors.getTempC(temp4));
  fieldData[4] = DallasTemperature::toFahrenheit(sensors.getTempC(temp5));
  fieldData[5] = DallasTemperature::toFahrenheit(sensors.getTempC(temp6));
  fieldData[6] = DallasTemperature::toFahrenheit(sensors.getTempC(temp7));
  fieldData[7] = DallasTemperature::toFahrenheit(sensors.getTempC(temp8));

}


// UTILITY

// This function return a string with the time stamp
String getTimeStamp() {
  String result;
  Process time;
  // date is a command line utility to get the date and the time
  // in different formats depending on the additional parameter
  time.begin("date");
  time.addParameter("+%D-%T");  // parameters: D for the complete date mm/dd/yy
  //             T for the time hh:mm:ss
  time.run();  // run the command

  // read the output of the command
  while (time.available() > 0) {
    char c = time.read();
    if (c != '\n') {
      result += c;
    }
  }

  return result;
}

void postData() {
  Process phant; // Used to send command to Shell, and view response
  String curlCmd; // Where we'll put our curl command
  String curlData[NUM_FIELDS]; // temp variables to store curl data

  // Construct curl data fields
  // Should look like: --data "fieldName=fieldData"
  for (int i = 0; i < NUM_FIELDS; i++)
  {
    curlData[i] = "--data \"" + fieldName[i] + "=" + String(fieldData[i]) + "\" ";
  }

  // Construct the curl command:
  curlCmd = "curl ";
  curlCmd += "--header "; // Put our private key in the header.
  curlCmd += "\"Phant-Private-Key: "; // Tells our server the key is coming
  curlCmd += privateKey;
  curlCmd += "\" "; // Enclose the entire header with quotes.
  for (int i = 0; i < NUM_FIELDS; i++)
    curlCmd += curlData[i]; // Add our data fields to the command
  curlCmd += phantURL + publicKey; // Add the server URL, including public key

  // Send the curl command:
  Serial.print("Sending command: ");
  Serial.println(curlCmd); // Print command for debug
  phant.runShellCommand(curlCmd); // Send command through Shell

  // Read out the response:
  Serial.print("Response: ");
  
  // Use the phant process to read in any response from Linux:
  while (phant.available())
  {
    char c = phant.read();
    Serial.write(c);
  }
}

void writeToSD() {

  // assemble the data point
  String dataString;
  dataString += getTimeStamp();
  dataString += "|";

  for (int f = 0; f < NUM_FIELDS; f++) {
    dataString += String(fieldData[f]);

    if (f != NUM_FIELDS - 1)
      dataString += ",";
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  // The FileSystem card is mounted at the following "/mnt/FileSystema1"
  File dataFile = FileSystem.open("/mnt/sd/boiler-log.txt", FILE_APPEND);

  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    // print to the serial port too:
    SerialUSB.println(dataString);
  }
  // if the file isn't open, pop up an error:
  else {
    SerialUSB.println("error opening datalog.txt");
  }

  ledBlink();
  //delay(15000);

}
