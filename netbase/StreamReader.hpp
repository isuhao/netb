/*
 * Copyright (C) 2010, Maoxu Li. Email: maoxu@lebula.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NET_BASE_STREAM_READER_HPP
#define NET_BASE_STREAM_READER_HPP

#include "StreamBuffer.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

NET_BASE_BEGIN

//
// StreamReader and StreamWriter are designed as tool classes to read from and 
// write to a StreamBuffer. The reader and writer work on a buffer object but 
// not own the buffer. 
// Reader read data from the buffer sequentially, and writer write data to the 
// buffer sequentially. In terms of protocol message packaging, these operations 
// are kind of serialization.
// Serialization read data from buffer and assign the value to variables in 
// protocol message packet, while write the value of variables into buffer. 
// Reader and Writer support serialization of same types of variables, and 
// particularly, the operations of reading and writing have compatable names, 
// i.e., SerializeXXX(). These may let protocol message packet using a some function 
// to read from buffer or write to buffer, which determined by using StreamReader 
// or StreamWriter.  
//
class StreamReader
{
private:
    StreamReader(const StreamReader*);
    StreamReader(const StreamReader&);
    void operator=(const StreamReader&);

public:
    // If not assign a StreamBuffer at init, 
    // should attach later before serializing operations
    StreamReader();
    StreamReader(StreamBuffer* buf);
    StreamReader(StreamBuffer& buf);
    ~StreamReader();

    StreamReader& Attach(StreamBuffer* buf);
    StreamReader& Attach(StreamBuffer& buf);

    // Read n bits from the low end
    bool SerializeBits(void* p, size_t n);

    // n bytes
    bool SerializeBytes(void* p, size_t n);

    // Bytes for integer, determined by integer type
    // Todo: endianess concerns for multi-bytes types 
    bool SerializeInteger(int8_t& v);
    bool SerializeInteger(uint8_t& v);
    bool SerializeInteger(int16_t& v);
    bool SerializeInteger(uint16_t& v);
    bool SerializeInteger(int32_t& v);
    bool SerializeInteger(uint32_t& v);
    bool SerializeInteger(int64_t& v);
    bool SerializeInteger(uint64_t& v);

    // Bytes for bool, float, and double
    // Todo: presentation formats for these types
    bool SerializeBool(bool& b);
    bool SerializeFloat(float& v);
    bool SerializeDouble(double& v);

    // Read a string that has n bytes length
    bool SerializeString(std::string& s, size_t n);

    // Read a string, with bytes before a delimit character
    bool SerializeString(std::string& s, const char delim);

    // Read a string, with bytes before a delimit string
    bool SerializeString(std::string& s, const std::string& delim);

private:
    StreamBuffer* mStream;
};

NET_BASE_END

#endif