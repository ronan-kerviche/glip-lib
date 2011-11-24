#include "Exception.hpp"

using namespace Glip;

    Exception::Exception(const std::string& m, std::string f, unsigned int l)
        : msg(m), filename(f), line(l)
    {
        completeMsg = '[';

        if(filename!="")
        {
            completeMsg += filename;
            completeMsg += ", ";
        }


        if(line!=0)
        {
            completeMsg += to_string(line);
        }

        completeMsg += "]\t";
        completeMsg += msg;
    }

    Exception::Exception(const Exception& e)
        : msg(e.msg), filename(e.filename), line(e.line), completeMsg(e.completeMsg)
    { }

    Exception::~Exception(void) throw()
    { }

    const char* Exception::what(void) const throw()
    {
        return completeMsg.c_str();
    }

    const char* Exception::message(void) const throw()
    {
        return msg.c_str();
    }

    const char* Exception::file(void) const throw()
    {
        return filename.c_str();
    }

    unsigned int Exception::lineNumber(void) const throw()
    {
        return line;
    }

    const Exception& Exception::operator+(const std::exception& e)
    {
        std::string str(e.what());

        size_t beg = 0;

        while((beg = str.find('\n', beg))!=std::string::npos)
        {
            beg++;
            str.insert(beg, 1, '\t'); // insert one TAB after the END-OF-LINE
        }

        msg += "\n\t";
        msg += str;

        completeMsg += "\n\t";
        completeMsg += str;

        return *this;
    }
