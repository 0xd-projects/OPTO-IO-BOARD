# OPTO-IO-BOARD

This board provides 16 independent voltage detection channels.  The channels can be configured to detect Open/Ground signals or Open/5-40V signals.  Each channel has a LED that indicates if the channel is ON or OFF, a currently limiting Resistor and an Opto-Isolator.  Each channel is then imputed into the MCP23017 IC that is setup as an Open/Ground detect.  The MCP23017 has an internal 100K internal pullup to 3.3V that is grounded by the Opto-Isolator when the Input is Active.  The MCP23017 samples it's inputs and reports their state over the I2C bus.  The Adafruit HUZZAH32 - ESP32 Feather board is used to poll the MCP23017 and send the current state of the channels over the network.  The HUZZAH32 Feather can be used over Wifi or with the optional Ethernet FeatherWing.

The board was designed with a large input voltage range as the input circuit is a series of two diodes so any input that provides enough current to activate the diodes will trigger the input.  The input trigger point is approximately 33ua because the Opto-isolator only has to sink the 33ua provided by the MCP23017 100K pullup to 3.3V.  So 0.5V over the two diodes forward voltage is enough to do this, 5V is more than enough to trigger this.  

If you would like to change the voltage level trigger you can add a Zener diode in series to the input circuit.  The Zener diode will increase the input level trigger by it's value + approximately 5V.  So, if you want to trigger at say 20V you can put a 15V Zener diode in series on the input.

The input voltage is limited by 10K resistor at 2W and the max steady state current rating of the input diodes ( Typically 20ma ).  At 40V input there is approximately only 3.6 ma of current going through the input circuit this is far below the 20ma max at around 0.8W.  The input was rated at 40V as a very conservative limit not knowing what conditions the circuit would be used in.


Folder structure
- doc                : Contains PDFs of the board design and the BOM.
- hw                 : Contains the Gerber and Kiacad design files for the PCB.
- sw/opto            : Contains example source code for using the board with the optional Ethernet FeatherWing.
- sw/opto_wifi       : Contains example source code for using the board with wifi.
- sw/sample_receiver : Contains example source code to receive and print the channel status on change.


You can find an overview of the Adafruit HUZZAH32 - ESP32 board at the link below.
- https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/overview

You can find an overview of the Adafruit Ethernet FeatherWing at the link below.
- https://learn.adafruit.com/adafruit-wiz5500-wiznet-ethernet-featherwing/overview

You can find the MCP23017 data sheet at the link below.
- https://cdn-shop.adafruit.com/datasheets/mcp23017.pdf


To build the code you need to download the following SW :
- HUZZAH32: https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/using-with-arduino-ide
- Ethernet FeatherWing: https://learn.adafruit.com/adafruit-wiz5500-wiznet-ethernet-featherwing/usage
- MCP23017: https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library

To edit the PCB design you can download Kicad at the link below.
- http://kicad-pcb.org/download/


## License
The contents of the doc and hw folders are licensed under the [Creative Commons Attribution 3.0 license](http://creativecommons.org/licenses/by/3.0/us/deed.en_US).
The contents of the sw folder is licensed under the [MIT license](http://opensource.org/licenses/mit-license.php).

