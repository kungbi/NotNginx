#pragma once

# include "IConfigContext.hpp"

class MainContext : public IConfigContext
{
	private:
		ContextType type_ = MAIN;
		unsigned int	worker_processes_;

	public:
		MainContext(unsigned int worker_processes);
		ContextType GetType() const;
		void PrintType(std::ostream &os) const;
		bool IsValid() const;

};
