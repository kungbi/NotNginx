#include "ConfigData.hpp"

ConfigData::ConfigData() {}

ConfigData::ConfigData(IConfigContext *contextRoot)
{
	if (!contextRoot) throw (ConfigData::ConfigSyntaxError());
	dataRoot_ = contextRoot;
}

ConfigData::~ConfigData()
{
	// deleteTree(dataRoot_);
}

IConfigContext* ConfigData::getRoot()
{
	return (dataRoot_);
}

void ConfigData::printConfigData(IConfigContext *parent)
{
    if (!parent)
    {
        std::cerr << "Error: Null context provided to PrintData." << std::endl;
        return;
    }

    std::cout << "========================================\n";
    std::cout << "Context Type: " << parent->getType() << "\n";

    // 옵션 출력
    std::vector<std::string> options = parent->getOptions();
    if (!options.empty())
    {
        std::cout << "Options: ";
        for (size_t i = 0; i < options.size(); ++i)
        {
            std::cout << options[i] << " ";
        }
        std::cout << "\n";
    }

    // 디렉티브 출력
    std::vector<IConfigDirective *> directives = parent->getDirectives();
    std::cout << "Directives (" << directives.size() << "):\n";
    for (size_t i = 0; i < directives.size(); ++i)
    {
        std::cout << "  [" << i + 1 << "] Directive Type: " << directives[i]->getType() << "\n";
        std::vector<std::string> values = directives[i]->getValues();
        std::cout << "      Values: ";
        for (size_t j = 0; j < values.size(); ++j)
        {
            std::cout << values[j] << " ";
        }
        std::cout << "\n";
    }

    // 자식 컨텍스트 출력 (재귀 호출)
    std::vector<IConfigContext *> children = parent->getChild();
    std::cout << "Sub-Contexts (" << children.size() << "):\n";
    for (size_t i = 0; i < children.size(); ++i)
    {
        printConfigData(children[i]);  // 재귀적으로 하위 컨텍스트 출력
    }

    std::cout << "========================================\n";
}


const char* ConfigData::ConfigSyntaxError::what() const throw()
{
	return ("Syntax Error in Config File");
}
