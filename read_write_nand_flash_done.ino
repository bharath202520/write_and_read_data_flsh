#include <SPI.h>
const int CS_PIN =53;
void setup() 
{
  Serial.begin(9600);
 delay(1020);
  SPI.begin();
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  reset();//0xFF
  readDeviceID();//0x9F
  const char* data = "Hello I am bharath kumar in from kurnool helloooooo";
  int len = strlen(data);
  char c1;
  //blockErase(0x123456);
  enableWrite();
 // writeDataToBuffer(0x0000,data, len);//0x02
 //blockErase(0x123456);//0xD8 or 0x52
  //delay(10);
writeDataToBuffer(0x0000,data, len);//0x02
blockErase(2);//0xD8 or 0x52
  programExecute(2);//0x10 

   char readData[len + 1];
  readPage(2, 0x0000,readData, len);//0x13,0x03
  char c;
  Serial.print("Read Data: ");
  for(int i=0;i<len;i++)
  {
    c=readData[i];
    Serial.print(c,HEX);
  }
  Serial.println();
  if(strcmp(data,readData)==0)
  {
    Serial.print("Both data is macheded so success");
  }
  else
  {
    Serial.print("Both data is macheded so Failure");
  }
}
void loop() {}
void reset() {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0xFF); // Reset command
  digitalWrite(CS_PIN, HIGH);
}

void readDeviceID() {
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x9F); // Read ID command
  byte id0 = SPI.transfer(0x00);
  byte id1 = SPI.transfer(0x00);
  byte id2 = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  Serial.print("Manufacturer ID: 0x");
  Serial.println(id1, HEX);
  Serial.print("Device ID: 0x");
  Serial.println(id2, HEX);
}

void enableWrite() 
{
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x06); // Write Enable command
  digitalWrite(CS_PIN, HIGH);

  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x0F);  // Get Feature
  SPI.transfer(0xC0);  // Status register
  uint8_t s= SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  if(s&0x02)
  {
    Serial.print("OIP (bit 0): ");
    Serial.println((s & 0x01) ? "Busy (1)" : "Ready (0)");
    Serial.print("WEL (bit 1): ");
    Serial.println((s & 0x02) ? "Write Enabled (1)" : "Not Enabled (0)");
    Serial.print("-----Write Enable Command Program success.-----");
    Serial.println();
  }
  else
  {
    Serial.print("OIP (bit 0): ");
    Serial.println((s & 0x01) ? "Busy (1)" : "Ready (0)");
    Serial.print("WEL (bit 1): ");
    Serial.println((s & 0x02) ? "Write Enabled (1)" : "Not Enabled (0)");
    Serial.print("-----Write Enable Command Failure-----");
    Serial.println();
  }
  }
//void writeDataToBuffer(uint16_t columnAddr, char* data, uint16_t length)
void writeDataToBuffer(uint16_t columnAddr, uint8_t* data, uint8_t length) 
{
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x02); // Program Load Random Data command mean we can give anywere give data not particular addresa or nor mandetary 0 address
  SPI.transfer((columnAddr >> 8) & 0xFF);
  SPI.transfer(columnAddr & 0xFF);
  // SPI.transfer((columnAddr >> 8) & 0xFF);
  for (int i = 0; i < length; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(CS_PIN, HIGH);
//read_sattus();
}
void programExecute(uint32_t pageAddr) 
{
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x10); // Program Execute command  meand to set the page address to set the address bits 
  SPI.transfer((pageAddr >> 16) & 0xFF);
  SPI.transfer((pageAddr >> 8) & 0xFF);
  SPI.transfer(pageAddr & 0xFF);
  waitUntilReady();
  digitalWrite(CS_PIN, HIGH);
  digitalWrite(CS_PIN, LOW);
    SPI.transfer(0x0F); // Get Feature command
    SPI.transfer(0xC0); // Status register address
   int  s = SPI.transfer(0x00);
    Serial.print("program ececute Status Register:------ 0x");
  Serial.println(s, HEX);
 if ((s & 0x01) == 0 && (s & 0x08) == 0) 
 {
    Serial.println(" Program executed successfully.");
 }
 else 
 {
    Serial.println("Program failed.");
 }
     Serial.print("P_FAIL (bit 3): ");
     Serial.println((s & 0x08) ? "Program Failure (1)" : "Program success (0)");
    //Serial.print("-----Write Enable Command Program success.-----");
}
void waitUntilReady() {
  uint8_t s;
  do {
    digitalWrite(CS_PIN, LOW);
    SPI.transfer(0x0F); // Get Feature command
    SPI.transfer(0xC0); // Status register address
    s = SPI.transfer(0x00);
    digitalWrite(CS_PIN, HIGH);
    delay(1);
  } while (s & 0x01); // Check busy bit
}
void blockErase(uint32_t pageAddr) 
{
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0xD8); // Block Erase command this  command i will cheked in chartgpt id erasr the 32kb usage 0x53 command and if you erase the 64kb usage command 0xD8
 // SPI.transfer((blockAddr >> 24) & 0xFF);
  SPI.transfer((pageAddr >> 16) & 0xFF);
  SPI.transfer((pageAddr >> 8) & 0xFF);
  SPI.transfer(pageAddr & 0xFF);
  waitUntilReady();
  digitalWrite(CS_PIN, HIGH);
  // // Check status
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x0F); // Get Feature
  SPI.transfer(0xC0); // Status register
  uint8_t s = SPI.transfer(0x00);
  digitalWrite(CS_PIN, HIGH);
  Serial.print("Erase Status Register:------ 0x");
  Serial.println(s, HEX);
  Serial.print("E_FAIL (bit 2): ");
  Serial.println((s & 0x04) ? "Erase Failure (1)" : "Erase success (0)");

  if (s & 0x04) {
    Serial.println("***** Erase Failure (E_FAIL bit 2 is set) *****");
  } else {
    Serial.println("----- Block erased successfully. -----");
  }
}
//void readPage(uint16_t pageAddr, uint16_t columnAddr, char* buffer, uint16_t length)
void readPage(uint32_t pageAddr, uint16_t columnAddr, uint8_t* buffer, uint8_t length) 
{
  //enableWrite();//0x06
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x13); // Page Read command the to set the this command  to read page address 
  SPI.transfer((pageAddr >> 16) & 0xFF);
  SPI.transfer((pageAddr >> 8) & 0xFF);
  SPI.transfer(pageAddr & 0xFF);
  waitUntilReady();  
  digitalWrite(CS_PIN, HIGH);
 // delay(1);
  // digitalWrite(CS_PIN, LOW);
  // SPI.transfer(0x0F); // Get Feature command
  // SPI.transfer(0xC0); // Status register address
  // int  s = SPI.transfer(0x00);
  // digitalWrite(CS_PIN, HIGH);
  // Serial.print("page read cmmand Status Register: 0x");
  // Serial.println(s, HEX);


  digitalWrite(CS_PIN, LOW);
  SPI.transfer(0x03); // Read From Cache command the command tonreaf the data frome in address the to variable 
  SPI.transfer((columnAddr >> 8) & 0xFF);
  SPI.transfer(columnAddr & 0xFF);
  SPI.transfer(0x00); // Dummy byte
  int i;
  for (i = 0; i < length; i++) {
    buffer[i] = SPI.transfer(0x00);
  }
  buffer[i]='\0';
  digitalWrite(CS_PIN, HIGH);
  char c;
  for (uint8_t i = 0; i < length; i++) {
    c=buffer[i];
    Serial.print(c);
  }
  Serial.println();
}
