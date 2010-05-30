/*!@file RTSPPacket.cpp
@brief

Copyright (c) 2009 HUST ITEC VoIP Lab.
All rights reserved.

@author  Wentao Tang
@date    2009.3.21
@version 1.0
*/
#include "RTSPPacket.h"
#include "util.h"
#include "libpcre++/pcre++.h"

using RTSP::SP;
using RTSP::CRLF;

RTSPPacket::RTSPPacket(void)
{
    Reset();
}

RTSPPacket::~RTSPPacket(void)
{
}

//TODO this func is not fully tested
void RTSPPacket::SetData( char *data, int len )
{
    ++setdatacout_;
    char *pfirst = data;
    int   start = 0;
    int   end = 0;
    int   line = 0;
    for ( int i = 0; i < len; ++i )
    {
        if ( '\r' == data[i] && (i+1) < len && '\n' == data[i+1] )
        {
            ++line;
            end = i;
            string str;
            int strlen = end - start;
            if ( end - start <= 0 )
            {
                if ( strlen == 0 ) //received \r\n\r\n
                {
                    string lenstr = header_[RTSP::HeaderField::Content_Length];
                    if ( 0 == lenstr.length() )
                    {
                        isCompletePacket_ = true;
                        header_.erase(RTSP::HeaderField::Content_Length);
                    }
                    else
                    {
                        contentLength_ = Utility::FromString<int>(lenstr);
                    }
                    //TODO to verify
                    if ( contentLength_ > 0 && len - end == contentLength_ ) // with msg body
                    {
                        isCompletePacket_ = true;
                        content_.assign(pfirst, contentLength_);
                    }
                }
                break;
            }
            str.assign(pfirst, strlen );
            pfirst += strlen + 2; // skeep \r\n
            //cout << "line: " << str << endl;

            if ( 1 == setdatacout_ && 1 == line)
            {
                ParserRequestLine(str);
            }
            else
            {
                ParserRequestHeader(str);
            }

            start = i + 2; // skeep \r\n
        }
    }
}

void RTSPPacket::AddAHeaderLine( string property_name, string property_value )
{
    header_[property_name] = property_value;
}

int  RTSPPacket::ParserRequestLine(string str)
{
    string requestheaderpattern = "^([A-Z]*)\\s*[RTSPrtsp]*:\\/\\/([\\w\\.\\/:-]*)\\s*([RTSPrtsp]*\\/(\\d).(\\d))";
    pcrepp::Pcre reg(requestheaderpattern, "i");
    if ( true == reg.search(str) )
    {
        for ( int pos = 0; pos < reg.matches(); ++pos )
        {
            //cout << pos << ":" << reg[pos] << '\n';
            if ( 0 == pos ) // request method
            {
                method_ = reg[pos];
            }
            if ( 1 == pos ) // request-URI without rtsp://
            {
                requestURI_ = reg[pos];
            }
            if ( 3 == pos ) // RTSP Version major
            {
                rtspVersion_.major = Utility::FromString<int>(reg[pos]);
            }
            if ( 4 == pos ) // RTSP Version minor
            {
                rtspVersion_.minor = Utility::FromString<int>(reg[pos]);
            }
        }
    }
    else
    {
        RTSPDEBUG("[Error]Failed match in ParserRequestLine");
    }

    return 0;
}

int  RTSPPacket::ParserRequestHeader(string str)
{
    string requestheaderpattern = "^([\\w-]*):\\s*([\\.\\(\\)\\;\\w\\/\\s\\d:=-]*)";
    pcrepp::Pcre reg(requestheaderpattern, "i");
    if ( true == reg.search(str) )
    {
        if ( 2 == reg.matches() )
        {
            AddAHeaderLine(reg[0], reg[1]);
        }
        else
        {
            RTSPDEBUG("[Error] Parser error in PareserRequestHeader");
            return -1;
        }
//        for ( int pos = 0; pos < reg.matches(); ++pos )
//        {
//            cout << pos << ":" << reg[pos] << '\n';
//
//        }
    }
    else
    {
        RTSPDEBUG("[Error]Failed match in ParserRequestHeader");
    }
    return 0;
}

//TODO to be modified to reduce transport
void RTSPPacket::AddDefaultHeader()
{
    //AddAHeaderLine(RTSP::HeaderField::Content_Length, string("0"));
}

void RTSPPacket::DumpPacket() const
{
    cout << "[Info]Dumping pakcet...\n";
    if ( RTSP::REQUEST_PKT == pktType_ )
    {
        cout << "Request Packet.\n";
        cout << "Method      : " << method_ << '\n';
        cout << "request-URI : " << requestURI_ << '\n';
        cout << "RTSP Version: " << rtspVersion_.major << "." <<rtspVersion_.minor << '\n';
    }
    else
    {
        cout << "Response Packet.\n";
        string reasion = GetStatusReasonByCode(statuscode_);
        cout << "RTSP/1.0 " << statuscode_ << " "<< reasion << "\n";
    }

    for ( const_headerItor itor = header_.begin(); itor != header_.end(); ++itor )
    {
        cout << itor->first << " : " << itor->second << '\n';
    }
    if ( 0 < content_.length() )
    {
        cout << content_;
    }
    cout << "[Info]dump finished\n";
}

const string RTSPPacket::GetData()
{
    data_ = "";
    if (RTSP::RESPONSE_PKT == pktType_)
    {
        data_ = "RTSP/1.0" + SP + statuscode_ + SP
        + GetStatusReasonByCode(statuscode_) + CRLF;
    }
    else
    {
        data_ = method_ + SP + "rtsp://" + requestURI_ + SP
        + "RTSP/1.0" + CRLF;
    }
    for ( const_headerItor itor = header_.begin(); itor != header_.end(); ++itor )
    {
        data_ += itor->first;
        data_ += ": ";
        data_ += itor->second;
        data_ += CRLF;
    }
    data_ += CRLF;
    if ( content_.length() )
    {
        data_ += content_;
        //data_ += CRLF;
    }
    len_ = data_.length();
    return data_;
}

void RTSPPacket::Reset()
{
    len_ = 0;
    isCompletePacket_ = false;
    method_ = "";
    requestURI_ = "";
    header_.empty();
    content_ = "";
    data_ = "";
    rtspVersion_.major = 0;
    rtspVersion_.minor = 0;
    setdatacout_       = 0;
    pktType_           = RTSP::REQUEST_PKT;
    contentLength_     = 0;
    AddDefaultHeader();
}

string RTSPPacket::GetStatusReasonByCode(string code) const
{
    for ( int i = 0; codeAndReason[i].code != "000"; ++i)
    {
        if ( code == codeAndReason[i].code )
        {
            return codeAndReason[i].reason;
        }
    }
    return string("");
}

