#include "Request.hpp"


Request::Request(
	RequestType requestType,
	const std::string& protocolVersion,
	const std::string& host,
	const std::string& target,
	const std::string& query,
	const std::string& fileName,
	const std::string& extension,
	const std::string& path,
	int port,
	const std::string& connection,
	size_t contentLength,
	const std::string& accept
) :
	requestType_(requestType),
	protocolVersion_(protocolVersion),
	host_(host),
	target_(target),
	query_(query),
	fileName_(fileName),
	extension_(extension),
	path_(path),
	port_(port),
	connection_(connection),
	contentLength_(contentLength),
	accept_(accept) {}