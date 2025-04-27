#include "LocationConfig.hpp"

// Constructor definition
LocationConfig::LocationConfig(
	const std::string& pattern,
	const std::string& root,
	const std::vector<std::string>& allowMethod,
	bool autoindex,
	std::vector<std::pair<int, std::string> > redirect,
	const std::vector<std::string>& index,
	const std::string& cgiInterpreter,
	const std::string& path // Ensure const references
)
	: pattern(pattern),
	  root(root),
	  allowMethod(allowMethod),
	  autoindex(autoindex),
	  redirect(redirect),
	  index(index),
	  cgiInterpreter(cgiInterpreter),
	  path(path) {}

bool LocationConfig::isAutoindex() const {
    return autoindex;
}

std::string LocationConfig::getPattern() const {
    return pattern;
}

std::string LocationConfig::getRoot() const {
    return root;
}

std::string LocationConfig::getCgiInterpreter() const {
    return cgiInterpreter;
}

const std::vector<std::string>& LocationConfig::getIndex() const {
    return index; // Return the index vector
}

std::string LocationConfig::getPath() const {
    return path;
}

bool LocationConfig::isAutoindexEnabled() const {
	return autoindex;
}

bool LocationConfig::isValidMethod(const std::string& method) const {
	if (allowMethod.size() == 0)
		return true;
	for (size_t i = 0; i < allowMethod.size(); i++) {
		if (method == allowMethod[i]) {
			return true;
		}
	}
	return false;
}

bool LocationConfig::isRedirectEnabled() const {
	return !redirect.empty();
}

std::pair<int, std::string> LocationConfig::getRedirect() const {
	if (redirect.empty()) {
		throw std::runtime_error("Error: No redirect configuration found.");
	}
	return redirect[0];
}