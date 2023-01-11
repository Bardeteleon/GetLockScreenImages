#include <fstream>

#include "TinyEXIF.h"

class EXIFStreamFile : public TinyEXIF::EXIFStream {
	private:
		std::ifstream file;
		std::vector<uint8_t> buffer;

	public:
		explicit EXIFStreamFile(const char* fileName)
			: file(fileName, std::ifstream::in | std::ifstream::binary) {
		}

		bool IsValid() const override {
			return file.is_open();
		}

		const uint8_t* GetBuffer(unsigned desiredLength) override {
			buffer.resize(desiredLength);
			if (!file.read((char*)buffer.data(), desiredLength))
				return NULL;
			return buffer.data();
		}

		bool SkipBuffer(unsigned desiredLength) override {
			return (bool)file.seekg(desiredLength, std::ios::cur);
		}
};