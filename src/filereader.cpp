/////////////////////////////////////////////////////////////////////////////
// Name:        filereader.cpp
// Author:      Laurent Pugin
// Created:     31/01/2024
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "filereader.h"

//----------------------------------------------------------------------------

#include <fstream>

//----------------------------------------------------------------------------

#include "vrv.h"

//----------------------------------------------------------------------------

#include "zip_file.hpp"

namespace vrv {

//----------------------------------------------------------------------------
// ZipFileReader
//----------------------------------------------------------------------------

ZipFileReader::ZipFileReader()
{
    m_file = NULL;

    this->Reset();
}

ZipFileReader::~ZipFileReader()
{
    this->Reset();
}

void ZipFileReader::Reset()
{
    if (m_file) {
        delete m_file;
        m_file = NULL;
    }
}

bool ZipFileReader::Load(const std::string &filename)
{
    std::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);
    if (!fin.is_open()) {
        return false;
    }

    fin.seekg(0, std::ios::end);
    std::streamsize fileSize = (std::streamsize)fin.tellg();
    fin.clear();
    fin.seekg(0, std::wios::beg);

    std::vector<unsigned char> bytes;
    bytes.reserve(fileSize + 1);

    unsigned char buffer;
    while (fin.read((char *)&buffer, sizeof(unsigned char))) {
        bytes.push_back(buffer);
    }
    return this->Load(bytes);
}

bool ZipFileReader::Load(const std::vector<unsigned char> &bytes)
{
    this->Reset();

    m_file = new miniz_cpp::zip_file(bytes);

    return true;
}

std::list<std::string> ZipFileReader::GetFileList()
{
    assert(m_file);

    std::list<std::string> list;
    for (miniz_cpp::zip_info &member : m_file->infolist()) {
        list.push_back(member.filename);
    }
    return list;
}

bool ZipFileReader::HasFile(const std::string &filename)
{
    assert(m_file);

    // Look for the file in the zip
    for (miniz_cpp::zip_info &member : m_file->infolist()) {
        if (member.filename == filename) {
            return true;
        }
    }

    return true;
}

std::string ZipFileReader::ReadTextFile(const std::string &filename)
{
    assert(m_file);

    // Look for the meta file in the zip
    for (miniz_cpp::zip_info &member : m_file->infolist()) {
        if (member.filename == filename) {
            return m_file->read(member.filename);
        }
    }

    LogError("No file '%s' to read found in the archive", filename.c_str());
    return "";
}

} // namespace vrv
