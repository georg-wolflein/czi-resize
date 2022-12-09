#include <iostream>
#include <string>
#include <codecvt>
#include <locale>
#include "libCZI/libCZI.h"

std::wstring stringToWstring(const std::string &t_str)
{
    typedef std::codecvt_utf8<wchar_t> convert_type;
    std::wstring_convert<convert_type, wchar_t> converter;
    return converter.from_bytes(t_str);
}

int process(const std::string &inFile, const std::string &outFile)
{
    auto readStream = libCZI::CreateStreamFromFile(stringToWstring(inFile).c_str());
    auto writeStream = libCZI::CreateOutputStreamForFile(stringToWstring(outFile).c_str(), true);
    auto reader = libCZI::CreateCZIReader();
    auto writer = libCZI::CreateCZIWriter();
    reader->Open(readStream);
    writer->Create(writeStream, nullptr);

    // // Read metadata
    // size_t readMetadataSize;
    // auto readMetadata = reader->ReadMetadataSegment();
    // auto readMetadataPtr = readMetadata->GetRawData(libCZI::IMetadataSegment::XmlMetadata, &readMetadataSize);
    // auto readMetadataXml = std::string(static_cast<const char *>(readMetadataPtr.get()), readMetadataSize);
    // // std::cout << "META: " << readMetadataXml << std::endl;

    int countSubBlock = 0,
        countSubBlocksWithData = 0,
        countSubBlocksWithMetadata = 0,
        countSubBlocksWithAttachment = 0;
    // int mIndex = 0;
    int numSubBlocks = reader->GetStatistics().subBlockCount;
    reader->EnumerateSubBlocks(
        [&](int idx, const libCZI::SubBlockInfo &info)
        {
            std::cout << int((double)idx / numSubBlocks * 100) << "%\r";
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
            // addSubBlockInfo.mIndex = mIndex++;
            addSubBlockInfo.x = info.logicalRect.x;
            addSubBlockInfo.y = info.logicalRect.y;
            addSubBlockInfo.logicalWidth = info.logicalRect.w;
            addSubBlockInfo.logicalHeight = info.logicalRect.h;
            addSubBlockInfo.physicalWidth = info.physicalSize.w;
            addSubBlockInfo.physicalHeight = info.physicalSize.h;
            addSubBlockInfo.PixelType = info.pixelType;
            if (info.GetZoom() != 1.)
            {
                addSubBlockInfo.ptrData = data.get();
                addSubBlockInfo.dataSize = sizeData;
            }
            // addSubBlockInfo.ptrSbBlkMetadata = metadata.get();
            // addSubBlockInfo.sbBlkMetadataSize = sizeMetadata;
            addSubBlockInfo.ptrSbBlkAttachment = attachment.get(); // for some reason we need to retain the attachments
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

    // // Write metadata
    // auto info = libCZI::PrepareMetadataInfo();
    // auto builder = writer->GetPreparedMetadata(info);
    // // builder->GetRootNode()->GetChildNode(L"Metadata")->GetChildNode(L"Information")->GetChildNode(L"Image")->;
    // std::cout << builder->GetXml() << std::endl;

    // libCZI::WriteMetadataInfo metaDataInfo;
    // metaDataInfo.szMetadata = readMetadataXml.c_str();
    // metaDataInfo.szMetadataSize = readMetadataXml.size() + 1;
    // metaDataInfo.ptrAttachment = nullptr;
    // metaDataInfo.attachmentSize = 0;
    // writer->SyncWriteMetadata(metaDataInfo);

    reader->Close();
    writer->Close();
    std::cout << "Done: " << inFile << " -> " << outFile << std::endl;

    std::cout << "  sub-blocks: " << countSubBlock << std::endl;
    std::cout << "  sub-blocks with data: " << countSubBlocksWithData << std::endl;
    std::cout << "  sub-blocks with metadata: " << countSubBlocksWithMetadata << std::endl;
    std::cout << "  sub-blocks with attachment: " << countSubBlocksWithAttachment << std::endl;
    return 0;
}

int main(int argc, char *argv[])
{
    std::string inFile = "";
    std::string outFile = "";
    if (argc == 3)
    {
        inFile = argv[1];
        outFile = argv[2];
    }
    else
    {
        std::cout << "Usage: ./cziresize input_file output_file" << std::endl;
        return 1;
    }
    process(inFile, outFile);
}