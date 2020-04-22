#ifndef exception_hpp
#define exception_hpp
#include <iostream>

class Exception: std::exception {
    std::string error;
    
public:
    Exception(std::string);
    
    const char * what() const noexcept override;
};

#endif
