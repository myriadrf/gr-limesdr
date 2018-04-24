# gr-limesdr v0.9 beta

Package includes GNU Radio blocks for LimeSDR-USB/LimeSDR-Mini
boards. Various settings are hidden or shown, depending on the
selected device type. 

## Documentation

* [MyriadRF Wiki page](https://wiki.myriadrf.org/Gr-limesdr_Plugin_for_GNURadio)

## Currently under development

* Callback functions
* Windows OS support

## Dependencies
 
* GNU Radio
* BOOST
* SWIG
* LimeSuite

## Installation process

<pre>
cd gr-limesdr/build
cmake ..
make
sudo make install
sudo ldconfig
</pre>

## Known issues

Known issues are located in:
gr-limesdr/docs/known_issues.txt

## GNU Radio-Companion examples

GNU Radio-Companion examples are located in:
gr-limesdr/examples

Select .wav file path before running any of the TX examples.