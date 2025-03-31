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
	std::map<int, std::string> redirect;
	std::vector<std::string> index;
	std::string cgiInterpreter;
	std::string path;

public:
	LocationConfig(
		const std::string& pattern,
		const std::string& root,
		const std::vector<std::string>& allowMethod,
		bool autoindex,
		const std::map<int, std::string>& redirect,
		const std::vector<std::string>& index,
		const std::string& cgiInterpreter,
		const std::string& path // Ensure const references
	);

	bool isAutoindex() const;
	std::string getPattern() const;
	std::string getRoot() const;
	std::string getCgiInterpreter() const;
	const std::vector<std::string>& getIndex() const;
	std::string getPath() const;
	bool isAutoindexEnabled() const;
};

#endif // LOCATIONCONFIG_HPP
