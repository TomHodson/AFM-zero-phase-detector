# AFM-zero-phase-detector
schematics and notes for a quartz resonator based atomic force microscope.

#System components

##Frequency generator 

AD9833 arduino code
http://www.vwlowen.co.uk/arduino/AD9833-waveform-generator/AD9833-waveform-generator.htm

ad9833 datasheet
http://www.analog.com/media/en/technical-documentation/data-sheets/AD9833.pdf

AD9833 module with a 25MHz reference clock. It works by setting an ADC from a sine wave lookup table once per clock cycle. Gives reasonably clean waveforms up to about 1MHz.

controlled through SPI interface, pins are CLK and DATA along with a chip select pin called FSYNC that needs to be brought low when sending data and high to tell the chip you’re finished.

SPI mode 2 (data on falling edge of clock), MSB first

the frequency register is 28 bits long, if you set it to x then the generated frequency will be 

output frequency = x * reference frequency / (2^28)

If you read the data sheet you’ll see that you need to send the value 0x2000 to put the IC in sine wave mode. If you read the included Arduino code you’ll see it’s actually sends 0x2003 which has the last two bits set, one that should put it in triangle wave mode and one bit that’s reserved and should always be 0. I cannot explain this but it seems to generate a sine wave, 0x2000 does not.

The sine and triangle wave outputs are around 0.6v pk-pk coming from the module.

The module is currently wired up to an arduino nano. There's variable gain opamp stage that you can control from a potentiometer and another pot wired up to A7 that you can use as input to arduino sketches.


##Phase detector

Measures the phase difference between the signal coming from the frequency generator and the current through the tuning fork AFM. Does zero crossing detection on both signals, using one to start a linear ramp (555) then uses the other to trigger a sample and hold IC, outputs this value. 

sample and hold LF198 data sheet
http://www.ti.com/lit/ds/symlink/lf398-n.pdf

Note: The output of the Sample and hold IC is steady when in hold mode but shows a transient when the IC is sampling, two can be chained to negate this issue. 


Freq Generator —> tuning fork —> preamp —> zero crossing detector —> | Ramp 

Freq Generator —> zero crossing detector —————————————-> |



Signal Ranger (Interface, DACs and ADCs)

+- 10v DAC outputs to drive piezos, ADC inputs to measure phase and amplitude information.

Need some mechanism to control the 

Manufacturer page and manual
http://www.softdb.com/dsp-products-SR-MK2.php
http://www.softdb.com/_files/_dsp_division/SignalRanger_mk2_UsersManual.pdf

Needs firmware.

Driven by GXSM.

##Power supply
https://octopart.com/tmp+15515c-tracopower-8602141

Tracopower TMP 15515C
5V @ 380mA
+- 15V @ +- 150mA

May need to be loaded at 10-20% rated current to get best stability.

V1 negative appears to the 5v ground
V1 positive is 5v
V2 is 15v
V3 is -15v
Com 2/3 is GND for the +-15V rails

The two grounds appear connected and both are connected with a wire in the box to mains earth return.

##Useful links

Initial 0PM-AFM paper
https://drive.google.com/file/d/0B9IbJ1fnFICZQWVrSVRpWlJHZWFQMFR3cmFBQ0w3NExQVEFZ/view

12: describes tuning fork based AFM
http://dx.doi.org/10.1063/1.113340

9: explanation of dissipation’s effect on resonant frequency and phase
http://dx.doi.org/10.1063/1.2355437

15: description of phase detector:
http://dx.doi.org/10.1063/1.1149968

AD9833 arduino code
http://www.vwlowen.co.uk/arduino/AD9833-waveform-generator/AD9833-waveform-generator.htm

ad9833 datasheet
http://www.analog.com/media/en/technical-documentation/data-sheets/AD9833.pdf

sample and hold lf198 data sheet
http://www.ti.com/lit/ds/symlink/lf398-n.pdf

signal ranger page
http://www.softdb.com/dsp-products-SR-MK2.php
http://www.softdb.com/_files/_dsp_division/SignalRanger_mk2_UsersManual.pdf

A paper on the properties of quartz tuning forks
http://laplace.ucv.cl/Cursos/FisicaExperimental_2/Old/AJP000415.pdf



