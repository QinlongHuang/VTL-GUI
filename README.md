# VocalTractLab-GUI

An X system version(Test on Ubuntu and MacOS) of [VocalTractLab](http://vocaltractlab.de "VocalTractLab").

Usage:
    Modify bool variable "with_GUI" in CMakeLists.txt to build GUI version or Python API version.

    cmake -S . -B build
    cmake --build build -j2

    or 

    pip install ./VTL-GUI
