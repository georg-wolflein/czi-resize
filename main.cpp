#include <iostream>
#include <vector>
#include "libCZI/libCZI.h"

int main()
{
    auto readStream = libCZI::CreateStreamFromFile(LR"(/scratch/TS_40X_HE_ICAIRD_2200.czi)");
    auto writeStream = libCZI::CreateOutputStreamForFile(LR"(/scratch/TS_40X_HE_ICAIRD_2200_out.czi)", true);
    auto reader = libCZI::CreateCZIReader();
    auto writer = libCZI::CreateCZIWriter();
    reader->Open(readStream);
    writer->Create(writeStream, nullptr); // TODO: add metadata

    int countSubBlock = 0,
        countSubBlocksWithData = 0,
        countSubBlocksWithMetadata = 0,
        countSubBlocksWithAttachment = 0;

    reader->EnumerateSubBlocks(
        [&](int idx, const libCZI::SubBlockInfo &info)
        {
            if (info.GetZoom() == 1.)
                return;
            std::cout << "Processing " << idx << std::endl;
            auto subBlock = reader->ReadSubBlock(idx);
            size_t sizeData, sizeMetadata, sizeAttachment;
            auto data = subBlock->GetRawData(libCZI::ISubBlock::Data, &sizeData);
            auto metadata = subBlock->GetRawData(libCZI::ISubBlock::Metadata, &sizeMetadata);
            auto attachment = subBlock->GetRawData(libCZI::ISubBlock::Attachment, &sizeAttachment);

            int c = 0, z = 0;
            info.coordinate.TryGetPosition(libCZI::DimensionIndex::C, &c);
            info.coordinate.TryGetPosition(libCZI::DimensionIndex::Z, &z);

            libCZI::AddSubBlockInfoMemPtr addSubBlockInfo;
            addSubBlockInfo.Clear();
            addSubBlockInfo.SetCompressionMode(info.GetCompressionMode());
            addSubBlockInfo.coordinate.Set(libCZI::DimensionIndex::C, c);
            addSubBlockInfo.coordinate.Set(libCZI::DimensionIndex::Z, z);
            addSubBlockInfo.mIndexValid = true;
            addSubBlockInfo.mIndex = info.mIndex; // TODO: check if this is correct
            addSubBlockInfo.x = info.logicalRect.x;
            addSubBlockInfo.y = info.logicalRect.y;
            addSubBlockInfo.logicalWidth = info.logicalRect.w;
            addSubBlockInfo.logicalHeight = info.logicalRect.h;
            addSubBlockInfo.physicalWidth = info.physicalSize.w;
            addSubBlockInfo.physicalHeight = info.physicalSize.h;
            addSubBlockInfo.PixelType = info.pixelType;
            addSubBlockInfo.ptrData = data.get();
            addSubBlockInfo.dataSize = sizeData;
            addSubBlockInfo.ptrSbBlkMetadata = metadata.get();
            addSubBlockInfo.sbBlkMetadataSize = sizeMetadata;
            addSubBlockInfo.ptrSbBlkAttachment = attachment.get();
            addSubBlockInfo.sbBlkAttachmentSize = sizeAttachment;

            writer->SyncAddSubBlock(addSubBlockInfo);

            countSubBlock++;
            if (sizeData > 0)
                countSubBlocksWithData++;
            if (sizeMetadata > 0)
                countSubBlocksWithMetadata++;
            if (sizeAttachment > 0)
                countSubBlocksWithAttachment++;
            return true;
        });
    std::cout << "Finishing..." << std::endl;

    // auto prepareMetadataInfo = libCZI::PrepareMetadataInfo();
    // auto metadata = writer->GetPreparedMetadata(prepareMetadataInfo);
    // writer->SyncWriteMetadata(metadata);

    reader->Close();
    writer->Close();
    std::cout << "Done." << std::endl;

    std::cout << "sub-blocks: " << countSubBlock << std::endl;
    std::cout << "sub-blocks with data: " << countSubBlocksWithData << std::endl;
    std::cout << "sub-blocks with metadata: " << countSubBlocksWithMetadata << std::endl;
    std::cout << "sub-blocks with attachment: " << countSubBlocksWithAttachment << std::endl;
    return 0;
}