#ifndef CGI_BUFFER_HPP
# define CGI_BUFFER_HPP

# include <map>

class CgiBuffer {
private:
	// pipe_fd -> buffer
	std::map<int, std::string> buffer_;

public:
	void appendBuffer(int pipeFd, const std::string& data);
	std::string popBuffer(int pipeFd);
	bool hasBuffer(int pipeFd);
};

#endif