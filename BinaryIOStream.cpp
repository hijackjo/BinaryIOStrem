#include "BinaryIOStream.h"

//=================class BinaryStreamWriter implementation============//
const char* BinaryStreamReader::GetData() const{
    return _ptr;
}
size_t BinaryStreamReader::GetSize() const{
    return _len;
}
bool BinaryStreamReader::IsEmpty() const{
    return _len <= 4;
}
bool BinaryStreamReader::IsEnd() const{
    assert(_cur <= _ptr + _len);
    return _cur == _ptr + _len;
}
bool BinaryStreamReader::ReadLength(size_t& len){
    size_t headlen;
    if (!ReadLengthWithoutOffset(headlen, len)) {
        return false;
    }

    //cur += BINARY_PACKLEN_LEN_2;
    _cur += headlen;
    return true;
}

//将一个1~5个字节的字符数组值还原成4字节的整型值
void BinaryStreamReader::read7BitEncoded(const char* buf, uint32_t len, uint32_t& value)
{
    char c;
    value = 0;
    int bitCount = 0;
    int index = 0;
    do
    {
        c = buf[index];
        uint32_t x = (c & 0x7F);
        x <<= bitCount;
        value += x;
        bitCount += 7;
        ++index;
    } while (c & 0x80);
}

//将一个1~10个字节的值还原成4字节的整型值
void BinaryStreamReader::read7BitEncoded(const char* buf, uint32_t len, uint64_t& value)
{
    char c;
    value = 0;
    int bitCount = 0;
    int index = 0;
    do
    {
        c = buf[index];
        uint64_t x = (c & 0x7F);
        x <<= bitCount;
        value += x;
        bitCount += 7;
        ++index;
    } while (c & 0x80);
}

//Constructor
BinaryStreamReader::BinaryStreamReader(const char * ptr, size_t len)
:_ptr(ptr)
,_len(len)
,_cur(ptr){
    _cur += 6; //skip the header
};//end of constructor

//通过CString的包头获取字符串的长度outlen
bool BinaryStreamReader::ReadLengthWithoutOffset(size_t& headlen, size_t& outlen)
{
    headlen = 0;
    const char* temp = _cur;
    char buf[5];
    //拷贝字符串长度压缩值到buf里
    for (size_t i = 0; i < sizeof(buf); i++)
    {
        memcpy(buf + i, temp, sizeof(char));
        temp++;
        headlen++;

        //if ((buf[i] >> 7 | 0x0) == 0x0)
        if ((buf[i] & 0x80) == 0x00)//到达压缩值末尾
            break;
    }
    //if (cur + headlen > ptr + len)
    //    return false;

    unsigned int value;
    read7BitEncoded(buf, headlen, value);//对字符串长度值解压
    outlen = value;

    return true;
}

//ReadString
BinaryStreamReader & operator>>(BinaryStreamReader & is, std::string & str){
    size_t headlen;//字符串长度压缩值有多少个字节
    size_t fieldlen;//字符串长度值
    if (!is.ReadLengthWithoutOffset(headlen, fieldlen)) {//获取字符串长度压缩值字节数和字符串长度值
        //return false;
         
    }
    // 偏移到数据的位置
    is._cur += headlen;

    if (is._cur + fieldlen > is._ptr + is._len)
    {
        //outlen = 0;
        //return false;
    }
    str.assign(is._cur, fieldlen);//把数据写入到字符串里
    //outlen = fieldlen;
    //is.cur += outlen;
    is._cur += fieldlen;//偏移到数据末尾
    return is;
}//end of ReadString

//ReadCString
BinaryStreamReader & operator>>(BinaryStreamReader & is, char* str){
    size_t fieldlen;
    size_t headlen;//
    size_t maxlen = sizeof(str);
    if (!is.ReadLengthWithoutOffset(headlen, fieldlen)) {
        //return false;
        return is;
    }

    // user buffer is not enough
    if (fieldlen > maxlen) {
        //return false;
        return is;
    }

    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;	
    is._cur += headlen;
    //if (is.cur + fieldlen > ptr + len)
    //{
    //    outlen = 0;
    //    return false;
    //}
    memcpy(str, is._cur, fieldlen);
    //outlen = fieldlen;
    //cur += outlen;
    is._cur += fieldlen;
    return is;

}//end of ReadCString

//ReadCCstring
BinaryStreamReader & operator>>(BinaryStreamReader & is, const char** str){
    size_t headlen;
    size_t fieldlen;
    size_t maxlen = sizeof(*str);
    if (!is.ReadLengthWithoutOffset(headlen, fieldlen)) {
        //return false;
    }
    // user buffer is not enough
    if (maxlen != 0 && fieldlen > maxlen) {
        //return false;
    }

    // 偏移到数据的位置
    //cur += BINARY_PACKLEN_LEN_2;
    is._cur += headlen;

    //memcpy(str, cur, fieldlen);
    if (is._cur + fieldlen > is._ptr + is._len)
    {
        //outlen = 0;
        //return false;
    }
    *str = is._cur;
    //outlen = fieldlen;
    is._cur += fieldlen;
    return is;
}

//ReadInt32
BinaryStreamReader & operator>>(BinaryStreamReader & is, int32_t& i){
    const int VALUE_SIZE = sizeof(int32_t);
    memcpy(&i, is._cur, VALUE_SIZE);
    if(is._isNULL == false){
        i = ntohl(i);
    }
    is._cur += VALUE_SIZE;
    return is;
}//end of ReadInt32

//ReadInt64
BinaryStreamReader & operator>>(BinaryStreamReader & is, int64_t& i){
    char int64str[128];
    operator>>(is,int64str);
    i = atoll(int64str);
    return is;
}//end of ReadInt64

//ReadShort
BinaryStreamReader & operator>>(BinaryStreamReader & is, short& i){
    const int VALUE_SIZE = sizeof(short);

    if (is._cur + VALUE_SIZE > is._ptr + is._len) {
        //return false;
    }

    memcpy(&i, is._cur, VALUE_SIZE);
    i = ntohs(i);

    is._cur += VALUE_SIZE;

    return is;
}//end of ReadShort

//ReadChar
BinaryStreamReader & operator>>(BinaryStreamReader & is, char& c){
    const int VALUE_SIZE = sizeof(char);

    if (is._cur + VALUE_SIZE > is._ptr + is._len) {
        //return false;
    }

    memcpy(&c, is._cur, VALUE_SIZE);
    is._cur += VALUE_SIZE;

    return is;
}//ReadChar

//ReadAll
size_t  BinaryStreamReader::ReadAll(char* szBuffer, size_t iLen){
    size_t iRealLen = std::min(iLen, _len);
    memcpy(szBuffer, _ptr, iRealLen);
    return iRealLen;
}


//=================class BinaryStreamWriter implementation============//

const char* BinaryStreamWriter::GetData() const{
    return m_data->data();
}

size_t BinaryStreamWriter::GetSize() const{
    return m_data->length();
}

//将一个4字节的整型压缩成1-5个字节
void BinaryStreamWriter::write7BitEncoded(uint32_t value, std::string& buf)
{
    do
    {
        //取出第一个字节 0x7F 111 1111
        unsigned char c = (unsigned char)(value & 0x7F);
        value >>= 7;//左移七位
        if (value)
            c |= 0x80;

        buf.append(1, c);//每次计算一个字符
    } while (value);
}

//将一个8字节的整型值编码成1~10个字节
void BinaryStreamWriter::write7BitEncoded(uint64_t value, std::string& buf)
{
    do
    {
        unsigned char c = (unsigned char)(value & 0x7F);
        value >>= 7;
        if (value)
            c |= 0x80;

        buf.append(1, c);
    } while (value);
}

BinaryStreamWriter::BinaryStreamWriter(std::string* data)
:m_data(data){
    m_data->clear();
    char buf[6] = {0};
    m_data->append(buf, 6);

}//end of constructor

//WriteCstring 
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, const char* str){
    uint32_t len = ::strlen(str);
    std::string buf;
    os.write7BitEncoded(len, buf);
    os.m_data->append(buf);//buf.size() = 1Byte
    os.m_data->append(str, len);
    return os;
}

//WriteString
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, const std::string& str){
    return operator<<(os, str.c_str());
}

//WriteDouble
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, double value){
    char   doublestr[128];
    if ( os._isNULL == false)
    {
        sprintf(doublestr, "%f", value);
        return operator<<(os, doublestr);
    }
    else
        return operator<<(os, doublestr);
}

//WriteInt64
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, int64_t value){
    char int64str[128];
    if (os._isNULL == false)
    {
#ifndef _WIN32
        sprintf(int64str, "%ld", value);
#else
        sprintf(int64str, "%lld", value);
#endif
        return operator<<(os, int64str);
    }
    else
        return operator<<(os, int64str);
}

//WriteInt32
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, int32_t i){
    if(os._isNULL == false){
        i = htonl(i);
    } 
    os.m_data->append((char*) &i, sizeof(i));
    return os;
}

//WriteShort
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, short i){
     short i2 = 0;
     if (os._isNULL == false)
         i2 = htons(i);
     os.m_data->append((char*)& i2, sizeof(i2));
     return os;
}//end of WriteShort

//WriteChar
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, char c){
    char c2 = 0;
    if (os._isNULL == false)
        c2 = c;
    (*os.m_data) += c2;
    return os;
}//end of WriteChar

//Flush
BinaryStreamWriter & operator<<(BinaryStreamWriter & os, Flush flush){
    char * ptr = &(*os.m_data)[0];
    unsigned int ulen = htonl(os.m_data->length());
    memcpy(ptr, &ulen, sizeof(ulen));
    return os;
}//end of Flush

//Clear
void BinaryStreamWriter::Clear(){
    m_data->clear();
    char str[4 + 2];
    m_data->append(str, sizeof(str));
}
