#pragma once
#include <string>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>

class Flush{

};

class BinaryStreamReader final
{
public:
    BinaryStreamReader(const char * ptr, size_t len);
    ~BinaryStreamReader() = default;

    virtual const char* GetData() const;
    virtual size_t GetSize() const;
    bool IsEmpty() const;
    bool IsEnd() const;
    const char* GetCurrent() const { return _cur; }
    void read7BitEncoded(const char* buf, uint32_t len, uint32_t& value);
    void read7BitEncoded(const char* buf, uint32_t len, uint64_t& value);
    bool ReadLength(size_t& len);
    bool ReadLengthWithoutOffset(size_t& headlen, size_t& outlen);

    //ReadString
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, std::string& str);
    //ReadCString
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, char* str);
    //ReadCCstring
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, const char** str);
    //ReadInt32
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, int32_t& i);
    //ReadInt64
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, int64_t& i);
    //ReadShort
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, short& i);
    //ReadChar
    friend BinaryStreamReader & operator>>(BinaryStreamReader & is, char& c);
    //ReadAll
    size_t  ReadAll(char* szBuffer, size_t iLen);


private://禁止复制
    BinaryStreamReader(const BinaryStreamReader&) = delete;
    BinaryStreamReader& operator=(const BinaryStreamReader&) = delete;

private:
    const char* const _ptr;//接受buffer的首地址,不可修改
    const size_t      _len;//buffer的长度
    const char*       _cur;//当前访问buffer的位置
    bool _isNULL =   false;//是否是网络字节序
};

class BinaryStreamWriter final
{
public:
    BinaryStreamWriter(std::string* data);
    ~BinaryStreamWriter() = default;

    virtual const char* GetData() const;
    virtual size_t GetSize() const;
    //WriteCstring 
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, const char* str);
    //WriteString
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, const std::string& str);
    //WriteDouble
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, double value);
    //WriteInt64
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, int64_t value);
    //WriteInt32
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, int32_t i);
    //WriteShort
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, short i);
    //WriteChar
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, char c);
    //Flush
    friend BinaryStreamWriter & operator<<(BinaryStreamWriter & os, Flush flush);

    size_t GetCurrentPos() const { return m_data->length(); }
    void Clear();
    
    void write7BitEncoded(uint32_t value, std::string& buf);    
    void write7BitEncoded(uint64_t value, std::string& buf);

private://禁止复制
    BinaryStreamWriter(const BinaryStreamWriter&) = delete;
    BinaryStreamWriter& operator=(const BinaryStreamWriter&) = delete;

private:
    std::string* m_data;
    bool _isNULL = false;
};

