#ifndef EXCEPTIONS_NOT_IMPLEMENTED_HPP
#define EXCEPTIONS_NOT_IMPLEMENTED_HPP

#include <stdexcept>
#include <string>

class not_implemented : public std::logic_error
{
private:
    std::string _message;

public:

    not_implemented(const char* message, const char* function) : std::logic_error("Not implemented")
    {
        _message = message;
        _message += " : ";
        _message += function;
    };

    not_implemented() : not_implemented("Not implemented", __FUNCTION__)
    {}

    not_implemented(const char* function) : not_implemented("Not implemented", function)
    {}

    virtual const char* what() const throw()
    {
        return _message.c_str();
    }

};

#endif // EXCEPTIONS_NOT_IMPLEMENTED_HPP
