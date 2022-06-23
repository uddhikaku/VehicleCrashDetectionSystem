#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <TimeLib.h>
#include <SPI.h>
#include <SD.h>

//Reset Arduino Uno
int Reset = 4;

//Read SD Card Support Variables
File myFile;
File myFileDa;

//first number
int recLine1 = 0;
String list1;
String eq1;
char phone_noo[12]; 

//GPS Module
int state = 0;
const int pin = 2;
float gpslat, gpslon;
float gpsSpeed;
unsigned long age;
int Year;
byte Month, Day, Hour, Minute, Second;
int ye;
byte mo, da, ho, mi, se;
const int offset = 5.5;   
time_t prevDisplay = 0; // when the digital clock was displayed
const int UTC_offset = 5.5;

TinyGPS gps;
SoftwareSerial sgps(5, 6); //rx tx   Arduino board rx pin =5 ---> GPS TX Pin 
SoftwareSerial sgsm(7, 8);

void setup()
{
  //Reset Arduino Uno
  pinMode(Reset, OUTPUT);
  digitalWrite(Reset, HIGH);
  delay(2000); //2 secods delay for reset arduino uno
  
  Serial.begin(9600);
  sgsm.begin(9600);
  sgps.begin(9600);

  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);         //Use to Power on sim900 module
  delay(5000);                 //5 Seconds dely to work other functions because sim900 module get some time to switch on

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
}

void loop()
{
  readMobileNum();

  int n = eq1.length() + 1;
  for (int i = 0; i < n; i++){
    eq1.toCharArray(phone_noo, n);
  }
  for (int i = 0; i < n; i++){
    eq1.toCharArray(phone_noo, n);
  }
  
  sgps.listen();
  while (sgps.available())
  {
    int c = sgps.read();
    if (gps.encode(c))
    {
      gps.crack_datetime(&Year, &Month, &Day, &Hour, &Minute, &Second, NULL, &age);
      if (age < 500) {
        // set the Time to the latest GPS reading
        setTime(Hour, Minute, Second, Day, Month, Year);
        adjustTime((UTC_offset * SECS_PER_HOUR) + SECS_PER_HOUR/2);
      }
      gps.f_get_position(&gpslat, &gpslon);
      gpsSpeed = gps.f_speed_kmph();
      ho = hour();
      mi = minute();
      se = second();
      da = day();
      mo = month();
      ye = year();
    }
  }
    if (digitalRead(pin) == HIGH && state == 0) {
      sgsm.listen();
      sgsm.print("\r");
      delay(1000);
      sgsm.print("AT+CMGF=1\r");
      delay(1000);
      sgsm.print("AT+CMGS=\"");sgsm.print(eq1);sgsm.println("\"\r\n");
      delay(1000);
      //The text of the message to be sent.
      sgsm.print("Latitude :");
      sgsm.println(gpslat, 6);
      sgsm.print("Longitude:");
      sgsm.println(gpslon, 6);
      sgsm.print("Speed : ");
      sgsm.print(gpsSpeed, 2);
      sgsm.println("kmph");
      sgsm.print("Date : ");
      sgsm.print(da);
      sgsm.print("/");
      sgsm.print(mo);
      sgsm.print("/");
      sgsm.println(ye);
      sgsm.print("Time : ");
      sgsm.print(ho);
      sgsm.print(":");
      sgsm.print(mi);
      sgsm.print(":");
      sgsm.println(se);
      delay(1000);
      sgsm.write(0x1A);
      delay(1000);
      state = 1;
      storeData();
    } 
  if (digitalRead(pin) == LOW) {
      state = 0;
    }
      delay(100);
}

void readMobileNum(){
 
  myFile = SD.open("number.txt", FILE_WRITE);
  
  // re-open the file for reading:
  myFile = SD.open("number.txt");
  if (myFile) {
    while (myFile.available())
    {
      list1 = myFile.readStringUntil('\r');
      recLine1++; // Count the record
      if(recLine1 == 1)
      {
         //Serial.println(list1);
         eq1 = list1;
      }
    }
    // close the file:
    myFile.close();
  } else {
  }
  myFile.close();
}



void storeData(){

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
 
  myFileDa = SD.open("data.txt", FILE_WRITE);
  if (myFileDa) {
    while (myFileDa.available())
    {
      myFileDa.print("Latitude :");
      myFileDa.println(gpslat, 6);
      myFileDa.print("Longitude:");
      myFileDa.println(gpslon, 6);
      myFileDa.print("Speed : ");
      myFileDa.print(gpsSpeed, 2);
      myFileDa.println("kmph");
      myFileDa.print("Date : ");
      myFileDa.print(da);
      myFileDa.print("/");
      myFileDa.print(mo);
      myFileDa.print("/");
      myFileDa.println(ye);
      myFileDa.print("Time : ");
      myFileDa.print(ho);
      myFileDa.print(":");
      myFileDa.print(mi);
      myFileDa.print(":");
      myFileDa.println(se);
      delay(1000);
    }
    // close the file:
    myFileDa.close();
  } else {
  } 
}
