# Arduino Volume Detector

This project is a volume detector component that gives the end user an indication of how loud an environment is. It is controlled by an Arduino Uno. Such a component can have many uses:
* Warn workers in a loud workplace site when sound levels are too lound, warranting the need for hearing protection
* Utility for the hearing impared, such as a baby monitor that gives a visual signal when a baby's volume reaches a certain threshold

## Hardware Decisions

### Processor

The following processors were evaluated:

| Processor family | Instruction set | Accumulate | Word size | Development board |
|------------------|-----------------|------------|-----------|-------------------|
| ARM Cortex-A7    | Thumb-2         | yes        | 32-bit    | Raspberry Pi B+   |
| ATmega328P       | AVR             | no         | 8-bit     | Arduino Uno       |
| ARM7TDMI         | Thumb           | yes        | 16-bit    |                   |
| Intel Atom       | i386            | yes        | 32-bit    | Intel Edison      |
| TI C55x DSP      | C55x            | yes        | 16-bit    | TMDX5515eZDSP     |

An important consideration for this project was ease of development. Hobby boards, such as the Raspberry Pi 2 B+ with a Cortex-A7, the Intel Edison with the Atom, and the Arduino Uno with the ATmega328P, provide a low barrier to entry. While it is important to ensure that your processor meets the requirements of the project, many of these processors I evaluated were plenty sufficient for my project. The overriding factor, therefore, was the board itself and the power usage. Both the Raspberry Pi and the Intel Edison have relatively powerful processors for embedded systems, and even have multiple cores for more bandwidth. 

The TI C55x, on the other hand, is highly specialized for digital signal processing. This might be really useful for implementing a filter, if you write optimized code for this processor. It's not meant to be a general puropse processor, however, so it was not chosen.

The Arduino Uno provided a plenty powerful processor that even had a built in A/D converter. The Uno is powered by an [ATmega328P](http://www.atmel.com/devices/ATMEGA328P.aspx) processor from Amtel. The board can easily be powered by a battery. A potential downside to this processor could be thst it does not have accumulator instructions. The Arduino ecosystem has an immensive number of free resources and code snippets, largely due to the platform being open source and very popular.

### DAC

The chosen DAC is the MCP4725, which has a 12-bit resolution. It uses the I2C interface, which makes connecting to it easier because it is a standard with a lot of documentation.

## Product Specification

The system shall meet the following requirements:
* A light whose brightness is proportional to the volume of the environment
* Another light that is green when the volume is below a certain level, and red when it is above that level
* A display that shows a number proportional to the volume

| Name        | Volume detector                                                                       |
|-------------|---------------------------------------------------------------------------------------|
| Purpose     | Provide visual display of volume levels of environment                                |
| Inputs      | Microphone                                                                            |
| Outputs     | Single color LED, multicolor LED, text display                                        |
| Functions   | LED's brightness, color of LED, and number on text display based on microphone signal |
| Performance | Can update output at every 100 milliseconds                                           |
| Cost        | $15                                                                                   |
| Power       | 4 x 1.5V battery                                                                      |
| Size/weight | 3"x5"x1", 8oz                                                                         |

## Software Requirements

The system works by taking 50 millisecond samples, and measuring the high and low peaks from the microphone input in each sample period. Because of the wave nature of sound, the raw output from the microphone turned out to be useless for this project. The following code implements this software requirement:

```
while (millis() - startMillis < sampleWindow) {
  sample = analogRead(0);

  if (sample < 1024)
  {
    if (sample > signalMax)
    {
      signalMax = sample;
    }
    else if (sample < signalMin)
    {
      signalMin = sample;
    }
  }
}

peakToPeak = signalMax - signalMin;
```

## Software Architecture

The system must listen to an microphone input loop, sample every 50 milliseconds and measure the peak to peak difference, then output the difference to various outputs. 

![Software flowchart](img/software-flowchart.png)

## Hardware-Software Interface

### Block Diagram

![Block diagram](img/block-diagram.png)

### Memory Map

The program includes two modules, one called LiquidCrystal.h for the text display. and the other Wire.h for communicating with the DAC.

![Memory map](img/memory-map.png)

### Bandwidth Estimate

My estimate is that, for each loop, there are 50 instructions. The analogRead(pin) instruction is a limiting factor. The Arduino can read at about 10 thousand times per second. That puts the program at 50 thousand instructions per second. With a clock speed of 16MHz (the frequency of the oscillator), that is approximately 16 million instructions per second. Dividing the program bandwidth by processor bandwidth gives 0.3% bandwidth utilization.

### Memory Estimate

According to the compiler, the program takes up 3,516 bytes of flash memory, and the global variables 229 bytes. The following local variables are in the loop, totaling 128 bits:

| Variable      | Data type | Size     |
|---------------|-----------|----------|
| startMillis   | long      | 32-bit   |
| peakToPeak    | int       | 16-bit   |
| signalMax     | int       | 16-bit   |
| signalMin     | int       | 16-bit   |
| while loop    | long      | 32-bit   |
| transmitValue | int       | 16-bit   |
|               | Total     | 128 bits |

The program will need at least 357 bits of SRAM, out of 2 kilobytes available.

### Constraints

Software can be constrained by the chosen hardware. For example, the number of times per second we can read the microphone signal depends on how long it takes to read each signal. This depends on, among other things, the clock speed. While it's not a practical contraint for this project, the 10 bits of resolution on the A/D converter could be a limiting factor in many projects that listen to sound. For example, standard audio CDs have a bit depth of 16 bits, and are sampled at 44,100 times per second. This A/D converter can read at about 10,000 times per second. 

Being a hobby board, the designers of the Arduino Uno made a great effort to make the board easy to program and debug. Furthermore, being open source there are very few intellectual property concerns.
