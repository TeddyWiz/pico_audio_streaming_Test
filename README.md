# How to Test RTP streaming Example



## Step 1: Prepare software

The following serial terminal programs are required for Loopback example test, download and install from below links.

- [**Tera Term**][link-tera_term]
- [**Hercules**][link-hercules]
- [**WireShark**][link-wireshark]

## Step 2: Prepare hardware
1. prepare device
    - W5100S-EVB-Pico  
    ![pico_evb](https://docs.wiznet.io/assets/images/w5500_evb_pico_side-da676c5d9c41adedc0469b9f1810b81b.png)
    - Electret Microphone Amplifier - MAX9814  
    ![mic](https://cdn-shop.adafruit.com/970x728/1713-00.jpg)

2. pin connection
    - pico 3v3       - mic VDD
    - pico GND       - mic GND
    - pico GPIO26_A0 - mic OUT     
     ![pico mic](/picture/picoandmic.png "pico and mic")  <br/><br/>

3. Connect ethernet cable to W5100S-EVB-Pico ethernet port.

4. Connect Raspberry Pi Pico, W5100S-EVB-Pico to desktop or laptop using 5 pin micro USB cable.

## Step 3: Setup RTP streaming Example

To test the RTP streaming example, minor settings shall be done in code.

1. Setup SPI port and pin in 'w5x00_spi.h' in 'RP2040-HAT-C/port/ioLibrary_Driver/' directory.

Setup the SPI interface you use.

## Step 4: Build

## Step 5: Upload and Run


[link-tera_term]: https://osdn.net/projects/ttssh2/releases/
[link-hercules]: https://www.hw-group.com/software/hercules-setup-utility
[link-wireshark]: https://www.wireshark.org/#download