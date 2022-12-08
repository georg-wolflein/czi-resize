#include <iostream>
#include "libCZI/libCZI.h"

int main()
{
    // auto stream = libCZI::CreateStreamFromFile(LR"(W:\Data\TestData\DCV_30MB.czi)");
    // auto cziReader = libCZI::CreateCZIReader();
    // cziReader->Open(stream);
    // cziReader->EnumerateSubBlocks(
    //     [](int idx, const libCZI::SubBlockInfo &info)
    //     {
    //         std::cout << "Index " << idx << ": " << libCZI::Utils::DimCoordinateToString(&info.coordinate) << " Rect=" << info.logicalRect << std::endl;
    //         return true;
    //     });
    std::cout << "Done";
    return 0;
}