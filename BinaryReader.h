#pragma once
#include <fstream>
#include <vector>
#include <string>

class BinaryReader
{
public:
	BinaryReader(std::ifstream& file);

	uint32_t ReadUInt32();
	void Seek(size_t offset);
	size_t Tell();
	std::vector<uint8_t> ReadBytes(size_t count);
	std::string ReadString(size_t length);
	size_t GetFileSize();
private:
	std::ifstream& m_file;
};

