#ifndef __GLIP_EXCEPTION__
#define __GLIP_EXCEPTION__

    // Includes
        #include <iostream>
        #include <sstream>

    namespace Glip
    {
        // Object
            class Exception : public std::exception
            {
                private :
                    // Data
                        std::string  msg;
                        std::string  filename;
                        unsigned int line;
                        std::string  completeMsg;
                public :
                    // Tools
                        Exception(const std::string& m, std::string f="", unsigned int l=0);
                        Exception(const Exception& e);
                        ~Exception(void) throw();

                        const char*  what(void) const throw();
                        const char*  message(void) const throw();
                        const char*  file(void) const throw();
                        unsigned int lineNumber(void) const throw();
                        const Exception& operator+(const std::exception& e);
            };

        // Tools
            template<typename TYPE>
            bool from_string(const std::string & Str, TYPE& Dest)
            {
                std::istringstream iss( Str );
                return iss >> Dest != 0;
            }

            template<typename TYPE>
            std::string to_string(const TYPE& Value )
            {
                std::ostringstream oss;
                oss << Value;
                return oss.str();
            }
    }

#endif
