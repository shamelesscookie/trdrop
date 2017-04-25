# T(ea)RDrop
<img src="images/trdrop_logo_text.png" alt="Teardrop logo" width="200" height="330">

trdrop - a cross platform fps analyzer for raw video data 

## Description

This software is used for analyzing raw video data, calculating framedrops and visualizing tears.
The result can be exported with an overlay displaying the information. For a more detailed description, look up the documentation in [docs](docs/documentation.pdf).

The projects is split in 3 parts, the **trdrop\_lib** and **trdrop\_c**.

* trdrop_c is the command line utility to process videos
* trdrop_lib is the library that provides a nice interface to create new tasks to process the videos in a single pass

Linux development is done with `opencv` and `CMake`.
