#include <iostream>
#include <vector>
#include "libCZI/libCZI.h"

int main()
{
    auto stream = libCZI::CreateInputOutputStreamForFile(LR"(/scratch/TS_40X_HE_ICAIRD_2200.czi)");
    auto readerWriter = libCZI::CreateCZIReaderWriter();
    readerWriter->Create(stream);
    std::vector<int> ids;
    readerWriter->EnumerateSubBlocks(
        [&ids](int idx, const libCZI::SubBlockInfo &info)
        {
            if (info.GetZoom() == 1.)
            {
                ids.push_back(idx);
            }
            return true;
        });
    std::cout << "Removing " << ids.size() << " subblocks" << std::endl;
    int i = 0;
    for (auto id : ids)
    {
        readerWriter->RemoveSubBlock(id);
        i++;
        std::cout << "Removed " << i << " of " << ids.size() << std::endl;
    }
    std::cout << "Closing" << std::endl;
    readerWriter->Close();
    std::cout << "Done" << std::endl;
    return 0;
}