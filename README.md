# gr-limesdr

Package includes GNU Radio blocks for various LimeSDR boards.

## Documentation

* [MyriadRF Wiki page](https://wiki.myriadrf.org/Gr-limesdr_Plugin_for_GNURadio)

## Dependencies
 
* GNU Radio(3.9)
* BOOST
* pybind11
* LimeSuite

## Installation process

#### Linux

* Building from source
<pre>
git clone https://github.com/chrisjohgorman/gr-limesdr.git
cd gr-limesdr
mkdir build
cd build
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
