/*
  Post data to SparkFun's data stream server system (phant) using
  an Arduino Yun

  Hardware Hookup:
    These components are connected to the Arduino's I/O pins:
      D3 - Active-low momentary button (pulled high internally)
      A0 - Photoresistor (which is combined with a 10k resistor
           to form a voltage divider output to the Arduino).
      D5 - SPST switch to select either 5V or 0V to this pin.
    Your Yun should also, somehow, be connected to the Internet.
    You can use Ethernet, or the on-board WiFi module.

  Development environment specifics:
    IDE: Arduino 1.5.6-r2
    Hardware Platform: Arduino Yun

*/

#include <Process.h> // Process class used to construct Shell commands

// phant setup
String phantURL = "http://ec2-54-218-89-249.us-west-2.compute.amazonaws.com:8080/input/"; // AWS 8080
String publicKey = "eJPyyB3pQaHzLEqbMPXlum3wMO1A"; // public
String privateKey = "2LErr9V0m5hml4LbqyQ7FQwLNOYg"; // private

const int NUM_FIELDS = 1;
String fieldName[NUM_FIELDS] = { "test" };
String fieldData[NUM_FIELDS]; // array to store our field data

int postInterval = 60000; // how often do we post

void setup() {

  Bridge.begin();

  Serial.begin(115200);

  while (!Serial); // yun l337 hax

}

void loop() {

  // assemble our test data
  fieldData[0] = String("sup");
  //fieldData[1] = String(digitalRead(switchPin));
  //fieldData[2] = name;

  // post Data
  Serial.println("Posting Data to server.");
  postData();

  delay(postInterval); // post once every 60 seconds
  
}

void postData() {
  
  Process phant; // Used to send command to Shell, and view response
  String curlCmd; // Where we'll put our curl command
  String curlData[NUM_FIELDS]; // temp variables to store curl data

  // Construct curl data fields
  // Should look like: --data "fieldName=fieldData"
  for (int i = 0; i < NUM_FIELDS; i++)
  {
    curlData[i] = "--data \"" + fieldName[i] + "=" + fieldData[i] + "\" ";
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
