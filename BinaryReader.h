#pragma once
#include <fstream>

class BinaryReader
{
public:
	BinaryReader(std::ifstream& file);

	uint32_t ReadUInt32();
	void Seek(size_t offset);
	size_t Tell();

private:
	std::ifstream& m_file;
};

