/*
 *  Filename: BinaryWriter.cpp
 *  This code is part of the Astera core library
 *  Copyright 2025 Jake Rieger
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  In no event and under no legal theory, whether in tort (including negligence),
 *  contract, or otherwise, unless required by applicable law (such as deliberate
 *  and grossly negligent acts) or agreed to in writing, shall any Contributor be
 *  liable for any damages, including any direct, indirect, special, incidental,
 *  or consequential damages of any character arising as a result of this License or
 *  out of the use or inability to use the software (including but not limited to damages
 *  for loss of goodwill, work stoppage, computer failure or malfunction, or any and
 *  all other commercial damages or losses), even if such Contributor has been advised
 *  of the possibility of such damages.
 */

#include "BinaryWriter.hpp"
#include <fstream>

namespace Astera {
    BinaryWriter::BinaryWriter(size_t initialCapacity, Endian endian) : mEndian(endian) {
        mBuffer.reserve(initialCapacity);
    }

    void BinaryWriter::WriteInt8(i8 value) {
        WriteRaw(&value, sizeof(value));
    }

    void BinaryWriter::WriteUInt8(u8 value) {
        WriteRaw(&value, sizeof(value));
    }

    void BinaryWriter::WriteInt16(i16 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteUInt16(u16 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteInt32(i32 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteUInt32(u32 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteInt64(i64 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteUInt64(u64 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteFloat(f32 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteDouble(f64 value) {
        WriteWithEndian(value);
    }

    void BinaryWriter::WriteBool(bool value) {
        WriteUInt8(value ? 1 : 0);
    }

    void BinaryWriter::WriteCString(const char* str) {
        const size_t len = std::strlen(str) + 1;
        WriteRaw(str, len);
    }

    void BinaryWriter::WriteString(const std::string& str) {
        WriteUInt32(static_cast<u32>(str.size()));
        WriteRaw(str.data(), str.size());
    }

    void BinaryWriter::WriteFixedString(const std::string& str, size_t fixedLength) {
        const size_t writeLen = std::min(str.size(), fixedLength);
        WriteRaw(str.data(), writeLen);

        for (size_t i = writeLen; i < fixedLength; ++i) {
            WriteUInt8(0);
        }
    }

    void BinaryWriter::WriteBytes(const u8* data, size_t size) {
        WriteRaw(data, size);
    }

    void BinaryWriter::WriteBytes(const std::vector<u8>& data) {
        WriteRaw(data.data(), data.size());
    }

    void BinaryWriter::WritePadding(size_t count) {
        for (size_t i = 0; i < count; ++i) {
            WriteUInt8(0);
        }
    }

    void BinaryWriter::AlignTo(size_t alignment) {
        const size_t currentPos = mBuffer.size();
        const size_t padding    = (alignment - (currentPos % alignment)) % alignment;
        WritePadding(padding);
    }

    size_t BinaryWriter::Tell() const {
        return mWritePos;
    }

    void BinaryWriter::Seek(size_t position) {
        if (position > mBuffer.size()) {
            throw std::out_of_range("Seek position out of range");
        }
        mWritePos = position;
    }

    const std::vector<u8>& BinaryWriter::GetBuffer() const {
        return mBuffer;
    }

    const u8* BinaryWriter::Data() const {
        return mBuffer.data();
    }

    size_t BinaryWriter::Size() const {
        return mBuffer.size();
    }

    void BinaryWriter::Clear() {
        mBuffer.clear();
        mWritePos = 0;
    }

    bool BinaryWriter::SaveToFile(const fs::path& filename) const {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            return false;
        }
        file.write(RCAST<const char*>(mBuffer.data()), mBuffer.size());
        return file.good();
    }

    void BinaryWriter::SetEndian(Endian endian) {
        mEndian = endian;
    }

    BinaryWriter::Endian BinaryWriter::GetEndian() const {
        return mEndian;
    }

    void BinaryWriter::WriteRaw(const void* data, size_t size) {
        const auto byteData = CAST<const u8*>(data);

        if (mWritePos < mBuffer.size()) {
            // Overwriting existing data
            const size_t available = mBuffer.size() - mWritePos;
            const size_t toCopy    = std::min(size, available);
            std::memcpy(mBuffer.data() + mWritePos, byteData, toCopy);
            mWritePos += toCopy;
            if (toCopy < size) {
                // Append remaining data
                mBuffer.insert(mBuffer.end(), byteData + toCopy, byteData + size);
                mWritePos = mBuffer.size();
            }
        } else {
            // Appending new data
            mBuffer.insert(mBuffer.end(), byteData, byteData + size);
            mWritePos = mBuffer.size();
        }
    }

    bool BinaryWriter::IsLittleEndian() {
        u16 test = 0x0001;
        return *RCAST<u8*>(&test) == 0x01;
    }
}  // namespace Astera