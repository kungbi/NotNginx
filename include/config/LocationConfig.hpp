#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <vector>
# include <map>

class LocationConfig {
private:
	std::string pattern;
	std::string root;
	std::vector<std::string> allowMethod;
	bool autoindex;
	std::vector< std::pair<int, std::string> > redirect;
	std::vector<std::string> index;
	std::string cgiInterpreter;
	std::string path;

public:
	LocationConfig(
		const std::string& pattern,
		const std::string& root,
		const std::vector<std::string>& allowMethod,
		bool autoindex,
		std::vector<std::pair<int, std::string> > redirect,
		const std::vector<std::string>& index,
		const std::string& cgiInterpreter,
		const std::string& path // Ensure const references
	);

	bool isValidMethod(const std::string& method) const;

	bool isAutoindex() const;
	std::string getPattern() const;
	std::string getRoot() const;
	std::string getCgiInterpreter() const;
	const std::vector<std::string>& getIndex() const;
	std::string getPath() const;
	bool isAutoindexEnabled() const;

	bool isRedirectEnabled() const;
	std::pair<int, std::string> getRedirect() const;
};

#endif // LOCATIONCONFIG_HPP
