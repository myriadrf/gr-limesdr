# gr-limesdr 1.0.0-RC

Package includes GNU Radio blocks for LimeSDR-USB/LimeSDR-Mini
boards. Various settings are hidden or shown, depending on the
selected device type. 

## Documentation

* [MyriadRF Wiki page](https://wiki.myriadrf.org/Gr-limesdr_Plugin_for_GNURadio)

## Dependencies
 
* GNU Radio
* BOOST
* SWIG
* LimeSuite

## Installation process

* Linux

<pre>
cd gr-limesdr
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
</pre>

* Windows

If you have "GNU Radio", merge windows\GNU_Radio folders with folders located in

<pre>
C:\Program Files\GNURadio-3.7
</pre>

If you have "Pothos SDR dev environment", merge windows\Pothos_SDR folders with
folders located in

<pre>
C:\Program Files\PothosSDR
</pre>

## Known issues

Known issues are located in:
gr-limesdr/docs/known_issues.txt

## GNU Radio-Companion examples

GNU Radio-Companion examples are located in:
gr-limesdr/examples
