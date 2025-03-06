#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <stdexcept>
#include "IConfigDirective.hpp"

class IConfigDirective;

class IConfigContext
{
	private:
		IConfigContext *parent_;
		int contextType_;
		std::vector<std::string> options_;
		std::vector<IConfigContext *> child_;
		std::vector<IConfigDirective *> directives_;

		void addChild(IConfigContext *child);

	public:
		IConfigContext(IConfigContext *parent, int contextType);
		~IConfigContext();

		// Getters
		IConfigContext* getParent() const;
		int getType() const;
		std::vector<std::string> getOptions() const;
		std::vector<IConfigContext *> getChild() const;
		std::vector<IConfigDirective *> getDirectives() const;

		void addDirectives(IConfigDirective *directive);
		void addOptions(std::string token);
		void printType(std::ostream &os) const;
		bool isValid() const;
};

int isContext(std::string token);
void deleteTree(IConfigContext *root);