#ifndef CHECKER_H_
#define CHECKER_H_

#include "message.h"
#include <vector>

class CheckerMessage{
public:
	// it is just a message that tansfer from one function to another
    const std::string res;
	const bool ok;
    CheckerMessage(bool ok, const std::string& str) : ok(ok), res(str) {}
	~CheckerMessage() {}
};

class AnswerChecker{
public:
	// check if the out.totpair is correct
	bool pairs_check(const message &out);

	// check if the out.totlength is correct
	bool length_check(const message &out);

	// check if the step is legal
	CheckerMessage legal_step_check(const std::vector<std::vector<int> > &block,
				 	 				const message &out);
public:
	// check if the given out is legal
	CheckerMessage legal_check(const std::vector<std::vector<int> > &block,
				 	 		   const message &out);
	// check if the answer of ans is better than out
	// we assume ans is a solution that is approximately best
	// if out is worse than ans, out is not the best solution
	CheckerMessage cmp_check(const message &out, const message &ans);
};

#endif //CHECKER_H_