#include <SPI.h>

const float refFreq = 25000000.0;           // On-board crystal reference frequency
#define SPI_CLOCK_SPEED      12000000

const int SINE = 0x2003;                    // Define AD9833's waveform register value.
const int SQUARE = 0x2020;                  // When we update the frequency, we need to
const int TRIANGLE = 0x2002;                // define the waveform when we end writing.    

const int FSYNC = 10;                       // Standard SPI pins for the AD9833 waveform generator.
const int CLK = 13;                         // CLK and DATA pins are shared with the TFT display.
const int DATA = 11;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5000);
  Serial.println("Send a value");
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE2);  
  delay(50);
  
  pinMode(FSYNC, OUTPUT);
  
  AD9833reset(); 
  delay(1000);
  AD9833setFrequency(32000, SINE);
  AD9833setFrequency(32000, SINE);

}
long last_ping = 0;

void loop() {
  long f = Serial.parseInt();

  if(f) {
    //AD9833reset(); 
    AD9833setFrequency(f, SINE);
    delay(50);
    Serial.print("F set to ");
    Serial.print(f);
    Serial.println("Hz");
  }

  if(millis() - last_ping > 1000) {
    Serial.println("Ping!");
    last_ping = millis();
  }

//long f = 32000.0 + 30000.0 * sin(millis() / 1000.0);
//AD9833setFrequency(f, SINE);
//delay(10);

}


// AD9833 documentation advises a 'Reset' on first applying power.
void AD9833reset() {
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE2));
  SPI.transfer16(0x0100);   // Write '1' to AD9833 Control register bit D8.
  delay(10);
  SPI.endTransaction();
}

// Set the frequency and waveform registers in the AD9833.
void AD9833setFrequency(long frequency, int Waveform) {
  
  long FreqWord = (frequency * pow(2, 28)) / refFreq;

  int MSB = (int)((FreqWord & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data
  int LSB = (int)(FreqWord & 0x3FFF);
  
  //Set control bits 15 and 14 to 0 and 1, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000; 
  
  digitalWrite(FSYNC, LOW);           // Set FSYNC low before writing to AD9833 registers
  delayMicroseconds(10);              // Give AD9833 time to get ready to receive data.
  
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE2));
  SPI.transfer16(0x2100);   
  SPI.transfer16(LSB);                  // Write lower 16 bits to AD9833 registers
  SPI.transfer16(MSB);                  // Write upper 16 bits to AD9833 registers.
  SPI.transfer16(0xC000);               // Phase register
  SPI.transfer16(Waveform);             // Exit & Reset to SINE, SQUARE or TRIANGLE

  digitalWrite(FSYNC, HIGH);          //Write done. Set FSYNC high
  SPI.endTransaction();
}
