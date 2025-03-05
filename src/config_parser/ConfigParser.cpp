#include "ConfigParser.hpp"

void ConfigParser::tokenize(std::string config_data)
{
	std::stringstream configStream(config_data);
	std::string currentToken;

	while (configStream >> currentToken)
	{
		configTokens_.push_back(currentToken);
	}
}

IConfigContext* ConfigParser::parseConfig()
{
    IConfigContext* root = new IConfigContext(nullptr, 0);
    IConfigContext* rootContext = nullptr; // 최종적으로 반환할 root.

    try
    {
        parseConfigRecursive(configTokens_, root);
        // fix: root의 첫 번째 자식(`http`)을 진짜 루트로 설정
        if (!root->getChild().empty())
		{
            rootContext = root->getChild()[0];
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        deleteTree(root);
        return nullptr;
    }

    return rootContext;
}

void ConfigParser::parseConfigRecursive(std::vector<std::string> configTokens, IConfigContext* parentContext)
{
	// iterator 설정.
	std::vector<std::string>::iterator currentIter = configTokens.begin();
	std::vector<std::string>::iterator lastIter = configTokens.end();
	lastIter--;
	
	// 예외 처리.
	if (currentIter == lastIter) throw (ConfigParser::ConfigSyntaxError());
	
	// 파싱
	while (currentIter != configTokens.end())
	{
		// 현재 토큰이 Context / Directive 인지 확인.
		int contextType = isContext(*currentIter);
		int directiveType = isDirective(*currentIter);
		
		// Context
		if (contextType != -1 && directiveType == -1)
		{
			if (currentIter == lastIter) throw ConfigParser::ConfigSyntaxError(); // 다음 토큰이 없는 경우 예외 처리.
			IConfigContext* newContextNode = new IConfigContext(parentContext, contextType); // 현재 새로운 Context -> 새로운 IConfigContext 노드 생성.
			++currentIter; // 다음 토큰으로 이동. ('/~~~' or '{' 를 가리키게 됨.)
			
			
			
			// 옵션 추가.
			while ((currentIter != configTokens.end()) && (*currentIter != "{"))
			{
				newContextNode->addOptions(*currentIter);
				++currentIter;
			}
			if (currentIter == configTokens.end()) throw ConfigParser::ConfigSyntaxError();
			
			
			
			
			// cuurentIter 이후 중괄호 내부 남아있는 토큰 innerBracketTokens에 저장.
			int BracketCount = 1;
			std::vector<std::string> innerBracketTokens;
			while (currentIter != configTokens.end())
			{
				++currentIter;
				if (*currentIter == "{") BracketCount++;
				if (*currentIter == "}") BracketCount--;
				if (BracketCount == 0) break;
				innerBracketTokens.push_back(*currentIter);
			}
			if (BracketCount != 0) throw ConfigParser::ConfigSyntaxError();
			
			
			
			
			// 재귀
			try
			{
				parseConfigRecursive(innerBracketTokens, newContextNode);
			}
			catch(...)
			{
				throw ConfigParser::ConfigSyntaxError();
			}




			// 재귀 호출이 끝난 후 `}`을 한 번 더 증가시켜서 남아있는 `}`를 처리
			if (currentIter != configTokens.end() && *currentIter == "}")
			{
				++currentIter;
			}
		}
		// Directive
		else if (directiveType != -1 && contextType == -1)
		{
			if (currentIter == lastIter) throw ConfigParser::ConfigSyntaxError(); // 예외 처리.
			std::string DirectiveString = *currentIter; // Directive 이름 저장. ex) "listen"
			IConfigDirective *directive = new IConfigDirective(parentContext, directiveType); // 새로운 IConfigDirective 생성
			++currentIter; // ex) "80;"
			


			// Directive 값 추가.
			while ((currentIter != configTokens.end()) && (*currentIter != ";"))
			{
				std::string::iterator TokenIterEnd = currentIter->end();
				--TokenIterEnd;
				if (*TokenIterEnd == ';')
				{
					directive->addValue(currentIter->substr(0, currentIter->size() - 1)); // ';' 제거하고 추가.
					++currentIter;
					break;
				}
				directive->addValue(*currentIter);
				++currentIter;
			}

			parentContext->addDirectives(directive); // 부모의 directives에 추가.
		}
		// 그 외의 경우
		else if (directiveType == -1 && contextType == -1) 
		{
			throw ConfigParser::ConfigSyntaxError();
		}
	}
}

const char* ConfigParser::ConfigSyntaxError::what() const throw()
{
	return ("Error: Config file has Syntax Error");
}
