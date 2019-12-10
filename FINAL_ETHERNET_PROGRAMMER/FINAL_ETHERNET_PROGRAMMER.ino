/*
 * 8 RST (black)
 * 9 SDA  (Green)
 * 11 MOSI (Orange)
 * 12 MISO (purple)
 * 13 SCK (yellow)

*/





#include <SPI.h>
#include <Ethernet.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

const int BUFFER_SIZE = 14; // RFID DATA FRAME FORMAT: 1byte head (value: 2), 10byte data (2byte version + 8byte tag), 2byte checksum, 1byte tail (value: 3)
const int DATA_SIZE = 10; // 10byte data (2byte version + 8byte tag)
const int DATA_VERSION_SIZE = 2; // 2byte version (actual meaning of these two bytes may vary)
const int DATA_TAG_SIZE = 8; // 8byte tag
const int CHECKSUM_SIZE = 2; // 2byte checksum
SoftwareSerial ssrfid = SoftwareSerial(2,3); 

uint8_t buffer[BUFFER_SIZE]; // used to store an incoming data frame 
int buffer_index = 0;


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 11); // if there the router is not assigned a ip address to the host this will be the ip address of the ethernet shield
IPAddress myDns(192, 168, 1, 1); //ip address of the router
EthernetClient client;
IPAddress server(192,168,1,10); // ip address of the pc
unsigned long lastConnectionTime = 0;           
const unsigned long postingInterval = 10*1000; 
void setup() {
   ssrfid.begin(9600);
   ssrfid.listen(); 
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  Serial.begin(9600);SPI.begin();

  while (!Serial) {;}
  
  Serial.println("Initialize Ethernet with DHCP:");
  
  if (Ethernet.begin(mac) == 0)
  {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware)
    {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true)
      {
        delay(1);
        }
     }
     if (Ethernet.linkStatus() == LinkOFF)
     {
       Serial.println("Ethernet cable is not connected.");
     }
    Ethernet.begin(mac, ip);// , myDns);
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
  }
  else
  {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  delay(1000);

}

void loop()
{
      



  if (ssrfid.available() > 0)
  {
    bool call_extract_tag = false;
    
    int ssvalue = ssrfid.read(); // read 
    if (ssvalue == -1) 
    { 
      // no data was read
      return;
    }
    if (ssvalue == 2)
    {
      // RDM630/RDM6300 found a tag => tag incoming 
      buffer_index = 0;
    }
    else if (ssvalue == 3)
    { 
      // tag has been fully transmitted       
      call_extract_tag = true; // extract tag at the end of the function call
    }
    if (buffer_index >= BUFFER_SIZE)
    { 
      // checking for a buffer overflow (It's very unlikely that an buffer overflow comes up!)
      Serial.println("Error: Buffer overflow detected!");
      return;
    }
    
    buffer[buffer_index++] = ssvalue; // everything is alright => copy current value to buffer
    if (call_extract_tag == true)
    {
      if (buffer_index == BUFFER_SIZE)
      {
        long tag = extract_tag();

        if (client.connected())
        {       
          lastConnectionTime = millis();
          client.println("Card["+String(tag)+"]");
          Serial.println("Sent to Server >> Card["+String(tag)+"]"); 
        }    
        else
        {
          Serial.println("Server not Connected ......."); 
        }

  
      }
      else
      {
        // something is wrong... start again looking for preamble (value: 2)
        buffer_index = 0;
        return;
      }
    }    
  } 

 String content= "";
 bool dataFromServer=false;
 
  while (client.available())
  {
    dataFromServer=true;
    char c = client.read();
    content.concat(c);    
    lastConnectionTime = millis();
  }
  if(dataFromServer)
  {
    Serial.println("Read From Server>> "+content);
  }

  if(content=="RedOn")
  {
    Serial.println("red Light On");
    digitalWrite(5,HIGH);
    delay(1000);

  }
  if(content=="RedOff")
  {
    Serial.println("red Light Off");
    digitalWrite(5,LOW);
    delay(1000);
  }
  if(content=="GreenOn")
  {
    Serial.println("Green Light On");
    digitalWrite(6,HIGH);
    delay(1000);
  }
  if(content=="GreenOff")
  {
    Serial.println("Green Light off");
    digitalWrite(6,LOW);
    delay(1000);
  }
  if(content=="BuzerGood")
  {
    Serial.println("Buzzer On");
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(300);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(50);
    digitalWrite(7,LOW);
    delay(50);
    
    
  }
  if(content=="BuzerWarning")
  {

    digitalWrite(7,HIGH);
    delay(200);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(200);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(200);
    digitalWrite(7,LOW);
    delay(50);
    digitalWrite(7,HIGH);
    delay(200);
    digitalWrite(7,LOW);
    delay(50);
  }
  if(content=="OffAll")
  {
    
    digitalWrite(5,LOW);
    digitalWrite(6,LOW);
  }


  
  if (millis() - lastConnectionTime > postingInterval)
  {
    httpRequest();
  }  
}




  
  

    
void httpRequest()
{
  client.stop();
  if (client.connect(server, 2020))
  {
    Serial.println("connecting to Server...");
    client.println();
    lastConnectionTime = millis();
  }
  else
  {
    Serial.println("Server connection failed");
  }
}





long extract_tag()
{
    uint8_t msg_head = buffer[0];
    uint8_t *msg_data = buffer + 1; // 10 byte => data contains 2byte version + 8byte tag
    uint8_t *msg_data_version = msg_data;
    uint8_t *msg_data_tag = msg_data + 2;
    uint8_t *msg_checksum = buffer + 11; // 2 byte
    uint8_t msg_tail = buffer[13];
    // print message that was sent from RDM630/RDM6300
//    Serial.println("--------");
 //   Serial.print("Message-Head: ");
//    Serial.println(msg_head);
//    Serial.println("Message-Data (HEX): ");
 //   for (int i = 0; i < DATA_VERSION_SIZE; ++i) {
 //     Serial.print(char(msg_data_version[i]));
 //   }
 //   Serial.println(" (version)");
 //   for (int i = 0; i < DATA_TAG_SIZE; ++i) {
 //     Serial.print(char(msg_data_tag[i]));
 //   }
  //  Serial.println(" (tag)");
  //  Serial.print("Message-Checksum (HEX): ");
  //  for (int i = 0; i < CHECKSUM_SIZE; ++i) {
  //    Serial.print(char(msg_checksum[i]));
  //  }
  //  Serial.println("");
 //   Serial.print("Message-Tail: ");
 //   Serial.println(msg_tail);
  //  Serial.println("--");
    long tag = hexstr_to_value(msg_data_tag, DATA_TAG_SIZE);
 //   Serial.print("Extracted Tag: ");
 //   Serial.println(tag);
    long checksum = 0;
    for (int i = 0; i < DATA_SIZE; i+= CHECKSUM_SIZE) {
      long val = hexstr_to_value(msg_data + i, CHECKSUM_SIZE);
      checksum ^= val;
    }
  //  Serial.print("Extracted Checksum (HEX): ");
  //  Serial.print(checksum, HEX);
 //   if (checksum == hexstr_to_value(msg_checksum, CHECKSUM_SIZE)) { // compare calculated checksum to retrieved checksum
  //    Serial.print(" (OK)"); // calculated checksum corresponds to transmitted checksum!
  //  } else {
  //    Serial.print(" (NOT OK)"); // checksums do not match
  //  }
   // Serial.println("");
   // Serial.println("--------");
    return tag;
}



long hexstr_to_value(char *str, unsigned int length)
{ 
  // converts a hexadecimal value (encoded as ASCII string) to a numeric value
  char* copy = malloc((sizeof(char) * length) + 1); 
  memcpy(copy, str, sizeof(char) * length);
  copy[length] = '\0'; 
  // the variable "copy" is a copy of the parameter "str". "copy" has an additional '\0' element to make sure that "str" is null-terminated.
  long value = strtol(copy, NULL, 16);  // strtol converts a null-terminated string to a long value
  free(copy); // clean up 
  return value;
}
