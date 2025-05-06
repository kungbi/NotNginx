#include "Router.hpp"
#include <sys/stat.h>

Router::Router(const ServerConfig& serverConfig) {
	const std::vector<LocationConfig>& locations = serverConfig.getLocations();
	for (size_t i = 0; i < locations.size(); i++) {
		sortedLocations_.push_back(std::make_pair(locations[i].getPattern(), locations[i]));
	}
	sortLocations();
}

Router::~Router() {}

void Router::sortLocations() {
	std::sort(sortedLocations_.begin(), sortedLocations_.end(), compareLength);
}

bool Router::compareLength(const LocationPair& a, const LocationPair& b) {
	return a.first.size() > b.first.size();
}

bool Router::matchPattern(const std::string& target, const std::string& pattern) const {
    if (pattern == "/")
        return true; // "/"는 무조건 다 매칭

    if (target.find(pattern) != 0)
        return false;

    if (target.size() == pattern.size() || target[pattern.size()] == '/')
        return true;

    return false;
}


std::string Router::getRelativePath(const std::string& target, const std::string& pattern) const {
	std::string relativePath = target.substr(pattern.size());
	if (!relativePath.empty() && relativePath[0] == '/') {
		relativePath = relativePath.substr(1);
	}
	return relativePath;
}

std::string Router::getFilePath(const LocationConfig& location, const std::string& relativePath, const std::string& fileName) const {
	std::string filePath = location.getRoot();
	if (!relativePath.empty()) {
		filePath += "/" + relativePath;
	}
	if (!fileName.empty()) {
		filePath += "/" + fileName;
	}
	return filePath;
}

RouteResult Router::createResult(const LocationConfig& location, const std::string& filePath, const std::string& fileExtension) const {
	if (!location.getCgiInterpreter().empty()) {
		return RouteResult(CGI_RESOURCE, filePath, location.getCgiInterpreter(), fileExtension, 200);
	}
	return RouteResult(STATIC_RESOURCE, filePath, "", fileExtension, 200);
}

std::string Router::getIndex(const LocationConfig& location) const {
	if (!location.getIndex().empty()) {
		return location.getIndex()[0];
	}
	throw std::runtime_error("Error: No index file found in location with autoindex enabled.");
}

bool Router::exists(const std::string& path) const {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

std::string Router::getExtension(const std::string& fileName) const {
	size_t dotPos = fileName.rfind('.');
	if (dotPos != std::string::npos) {
		return fileName.substr(dotPos + 1);
	}
	return "";
}

RouteResult Router::routeRequest(const Request& request) const {
	const std::string& target = request.getTarget();
	const std::string& fileName = request.getFilename();

	for (size_t i = 0; i < sortedLocations_.size(); i++) {
		const std::string& pattern = sortedLocations_[i].first;
		const LocationConfig& location = sortedLocations_[i].second;

		if (!matchPattern(target, pattern)) {
			continue;
		}

		if (!isValidMethod(request.getRequestType(), location)) {
			throw MethodNotAllowedError("Error: Method not allowed for this location.");
		}

		if (location.isRedirectEnabled()) {
			std::pair<int, std::string> redirect = location.getRedirect();
			return RouteResult(REDIRECT_RESOURCE, redirect.second, "", "", redirect.first);
		}

		std::string relativePath = getRelativePath(target, pattern);
		if (!fileName.empty() && relativePath.size() >= fileName.size() &&
			relativePath.substr(relativePath.size() - fileName.size()) == fileName) {
			relativePath = relativePath.substr(0, relativePath.size() - fileName.size());
			if (!relativePath.empty() && relativePath.back() == '/') {
				relativePath.pop_back();
			}
		}

		if (!exists(getFilePath(location, relativePath, ""))) {
			throw NotFoundError("Error: File not found at " + getFilePath(location, relativePath, ""));
		}

		std::cout << "File path: " << getFilePath(location, relativePath, fileName) << " " << fileName.empty() << std::endl;
		if (fileName.empty() && !isDirectory(getFilePath(location, relativePath, fileName))) {
			return createResult(location, getFilePath(location, relativePath, ""), "nothing");
		}

		std::string resolvedFileName = fileName;
		std::string fileExtension;
		if (fileName.empty()) {
			const std::vector<std::string>& indices = location.getIndex();
			for (size_t j = 0; j < indices.size(); j++) {
				std::string potentialFilePath = getFilePath(location, relativePath, indices[j]);
				if (exists(potentialFilePath)) {
					resolvedFileName = indices[j];
					fileExtension = getExtension(resolvedFileName);
					break;
				}
			}
			if (resolvedFileName.empty() && !location.isAutoindexEnabled()) {
				throw NotFoundError("Error: No index file found and autoindex is disabled.");
			}
		} else {
			fileExtension = getExtension(fileName);
		}

		std::string filePath = getFilePath(location, relativePath, resolvedFileName);

		if (!resolvedFileName.empty()) {
			return createResult(location, filePath, fileExtension);
		}

		if (!exists(filePath)) {
			filePath = location.getRoot() + "/" + relativePath;
		}

		return createResult(location, filePath, fileExtension);
	}

	throw NotFoundError("Error: No matching location found for the request.");
}

bool Router::isValidMethod(const RequestType& method, const LocationConfig& location) const {
	if (method == GET)
		return location.isValidMethod("GET");
	if (method == POST)
		return location.isValidMethod("POST");
	if (method == PUT)
		return location.isValidMethod("PUT");
	if (method == PATCH)
		return location.isValidMethod("PATCH");
	if (method == DELETE)
		return location.isValidMethod("DELETE");
	return false;
}

bool Router::isDirectory(const std::string& path) const {
    struct stat pathStat;
    if (stat(path.c_str(), &pathStat) != 0) {
        return false; // 존재하지 않거나 에러
    }
    return S_ISDIR(pathStat.st_mode);
}