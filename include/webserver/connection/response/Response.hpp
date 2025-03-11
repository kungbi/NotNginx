#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <ctime>
# include <sstream>

class Response {
public:
	virtual std::string getResponse(void) = 0;
};

#endif // RESPONSE_HPP