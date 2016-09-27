#include <SPI.h>

const float refFreq = 25000000.0;           // On-board crystal reference frequency
#define SPI_CLOCK_SPEED      12000000

const int SINE = 0x2003;                    // Define AD9833's waveform register value.
const int SQUARE = 0x2020;                  // When we update the frequency, we need to
const int TRIANGLE = 0x2002;                // define the waveform when we end writing.    

const int FSYNC = 10;                       // Standard SPI pins for the AD9833 waveform generator.
const int CLK = 13;                         // CLK and DATA pins are shared with the TFT display.
const int DATA = 11;

const int POT = A7; // the pin that a potentiometer is connected to.
const int PHASE = A5;
const int AMP = A6;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5000);
  
  SPI.begin();
  SPI.setDataMode(SPI_MODE2);  
  delay(50);
  
  pinMode(FSYNC, OUTPUT);

  pinMode(POT, INPUT);
  pinMode(PHASE, INPUT);
  pinMode(AMP, INPUT);
  
  AD9833reset(); 
  delay(1000);

  float start_f = 32740;
  float end_f = 32770;
  int steps = 50;

  for(int i = 0; i < steps; i++) {
    float f = start_f + (float(i)/float(steps)) * (end_f - start_f);
    long FreqWord = (f * pow(2, 28)) / refFreq;
    AD9833setFrequency(f, SINE);
    delay(2000);
    int phase = analogRead(PHASE);
    int amp = analogRead(AMP);
    Serial.print(f); Serial.print(", "); Serial.print(amp); Serial.print(", "); Serial.println(phase);
  }
}

void loop() {
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
  AD9833setFrequencyRegister(FreqWord, Waveform);
}

// Set the 28 bit frequency and waveform registers directly.
void AD9833setFrequencyRegister(long FreqWord, int Waveform) {

  int MSB = (int)((FreqWord & 0xFFFC000) >> 14);    //Only lower 14 bits are used for data
  int LSB = (int)(FreqWord & 0x3FFF);
  
  //Set control bits 15 and 14 to 0 and 1, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000; 
  
  digitalWrite(FSYNC, LOW);           // Set FSYNC low before writing to AD9833 registers
  delayMicroseconds(20);              // Give AD9833 time to get ready to receive data.
  
  SPI.beginTransaction(SPISettings(SPI_CLOCK_SPEED, MSBFIRST, SPI_MODE2));
  SPI.transfer16(0x2100);   
  SPI.transfer16(LSB);                  // Write lower 16 bits to AD9833 registers
  SPI.transfer16(MSB);                  // Write upper 16 bits to AD9833 registers.
  SPI.transfer16(0xC000);               // Phase register
  SPI.transfer16(Waveform);             // Exit & Reset to SINE, SQUARE or TRIANGLE

  digitalWrite(FSYNC, HIGH);          //Write done. Set FSYNC high
  SPI.endTransaction();
}
