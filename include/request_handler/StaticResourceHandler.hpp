#ifndef STATIC_RESOURCE_HANDLER_HPP
# define STATIC_RESOURCE_HANDLER_HPP

# include <string> 
# include <fstream>
# include "Response.hpp"
# include "StaticResourceResponse.hpp"
# include "MimeTypes.hpp"
# include "HttpExceptions.hpp"

class StaticResourceHandler {
private:
	StaticResourceHandler(void);

public:
	static Response* execute(std::string path, std::string fileExtension);
};

#endif