# gr-limesdr

**NOTE: this branch is pre-release and should not be considered stable.**

Package includes GNU Radio blocks for various LimeSDR boards.

## Documentation

* [MyriadRF Wiki page](https://wiki.myriadrf.org/Gr-limesdr_Plugin_for_GNURadio)

## Dependencies
 
* GNU Radio (versions 3.9, 3.10 or 3.11)
* libfmt
* pybind11
* LimeSuite

## Installation process

#### Linux

* Building from source
<pre>
git clone https://github.com/myriadrf/gr-limesdr.git
cd gr-limesdr
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
