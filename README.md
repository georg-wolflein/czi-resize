czi-resize
==========

A tool for compressing [CZI](https://www.zeiss.com/microscopy/en/products/software/zeiss-zen/czi-image-file-format.html) file size by storing data at a lower magnification using [libCZI](https://github.com/ZEISS/libczi).
For example, it can convert a CZI file scanned with 40x magnification to one with 20x magnification.
In my experiments, I could shrink a 6.5GB CZI at 40x magnification to 1.5GB at 20x magnification.

**Use at your own risk!**
This tool does not copy over metadata, so it will get lost if you delete the original file.
Also, there may be issues opening the converted files in some CZI viewers.

This tool will delete the sub-blocks at `zoom=1.0` (actually, it just removes the data payload of these subblocks). 
When reading the CZI file using a library like [pylibCZIrw](https://pypi.org/project/pylibCZIrw/), make sure you only access tiles at `zoom<=0.5`. 

## Installing
```
mkdir build
cd build
cmake ..
cmake --build .
```

## Running
```
./build/cziresize in_file.czi out_file.czi
```