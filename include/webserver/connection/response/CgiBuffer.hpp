#ifndef CGI_BUFFER_HPP
# define CGI_BUFFER_HPP

# include <map>

class CgiBuffer {
private:
	std::map<int, std::string> buffer_;

public:
	void appendBuffer(int fd, const std::string& data);
	std::string popBuffer(int fd);
	bool hasBuffer(int fd);
};

#endif