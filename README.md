# gr-limesdr

Package includes GNU Radio blocks for various LimeSDR boards.

## Documentation

* [MyriadRF Wiki page](https://wiki.myriadrf.org/Gr-limesdr_Plugin_for_GNURadio)

## Dependencies
 
* GNU Radio (>=3.8)
* BOOST
* SWIG
* LimeSuite

## Installation process

* Installing GNURadio
To install GNURadio3.8 please follow this guide [Installing GNURadio](https://wiki.gnuradio.org/index.php/InstallingGR)

* Building from source
<pre>
git clone https://github.com/myriadrf/gr-limesdr.git
cd gr-limesdr
git checkout gr-3.8
mkdir build
cd build
cmake ..
make
sudo make install
sudo ldconfig
</pre>

## GNU Radio-Companion examples

GNU Radio-Companion examples are located in:
gr-limesdr/examples
