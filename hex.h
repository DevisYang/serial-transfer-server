#pragma once
#include <string>

struct HexBuffer {
	uint8_t* data;
	int size;
	HexBuffer(int size) {
		data = new uint8_t[size];
		this->size = size;
	}
	HexBuffer(uint8_t* data, int size) {
		this->data = new uint8_t[size];
		memcpy(this->data, data, size);
		this->size = size;
	}
	HexBuffer(const HexBuffer& v) {
		this->data = new uint8_t[v.size];
		memcpy(this->data, v.data, v.size);
		this->size = v.size;
	}
	HexBuffer& operator=(const HexBuffer& v) {
		this->data = new uint8_t[v.size];
		memcpy(this->data, v.data, v.size);
		this->size = v.size;
		return *this;
	}
	~HexBuffer() {
		if (data != nullptr) {
			delete[] data;
			data = nullptr;
		}
	}
};

std::string BytesToHexString(const uint8_t* data, int size);

HexBuffer HexStringToBytes(const std::string& hex);