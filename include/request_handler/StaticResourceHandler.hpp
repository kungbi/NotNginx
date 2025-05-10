#ifndef STATIC_RESOURCE_HANDLER_HPP
# define STATIC_RESOURCE_HANDLER_HPP

# include <string> 
# include <fstream>
# include "Response.hpp"
# include "StaticResourceResponse.hpp"
# include "MimeTypes.hpp"
# include "HttpExceptions.hpp"
# include "ResponseReason.hpp"
# include <unistd.h>
# include <sys/stat.h> 

class StaticResourceHandler {
private:
	StaticResourceHandler(void);

	
public:
	static bool hasReadPermission(const std::string& path);
	static Response* execute(std::string path, std::string fileExtension, int statusCode);
};

#endif