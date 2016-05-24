#include "checker.h"
#include "message.h"

bool AnswerChecker::pairs_check(const message &out){
    int tot_pairs = 0;
    for (int i = 1; i <= out.d(); ++i){
        if (out.route[i].size() > 0)
            ++tot_pairs;
    }
    return tot_pairs == out.totpair();
}

bool AnswerChecker::length_check(const message &out){
    int tot_length = 0;
    for (int i = 1; i <= out.d(); ++i){
        tot_length += out.route[i].size();
    }
    return tot_length == out.totlength();
}

CheckerMessage AnswerChecker::legal_step_check(const std::vector<std::vector<int> > &block,
                                               const message &out){
    for (int d = 1; d <= out.d(); ++d){
        // check the droplet d
        
        int len = out.route[d].size();
        
        if (len == 0) continue;
        
        // check the origin
        int sx = out.route[d][0].first,
        sy = out.route[d][0].second;
        
        if (block[sx][sy] != d)
            return CheckerMessage(false, std::string("LegalError: origin error"));
        
        // check the terminal
        int tx = out.route[d][len - 1].first,
        ty = out.route[d][len - 1].second;
        if (block[tx][ty] != d)
            return CheckerMessage(false, std::string("LegalError: terminal error"));
        
        // check the connection
        for (int i = 1; i < len; ++i){
            int dx = out.route[d][i].first - out.route[d][i - 1].first,
            dy = out.route[d][i].second - out.route[d][i - 1].second;
            if (abs(dx) + abs(dy) != 1)
                return CheckerMessage(false, std::string("LegalError: connection error"));
        }
        
        // check the block
        for (int i = 1; i < len - 1; ++i){
            int x = out.route[d][i].first,
            y = out.route[d][i].second;
            if (block[x][y] != 0)
                return CheckerMessage(false, std::string("LegalError: cross an illegal grid"));
        }
    }
    
    return CheckerMessage(true, "OK");
}

CheckerMessage AnswerChecker::legal_check(const std::vector<std::vector<int> > &block,
                                          const message &out){
    if (!pairs_check(out))
        return CheckerMessage(false, std::string("LegalError: total pairs is not correct"));
    
    if (!length_check(out))
        return CheckerMessage(false, std::string("LegalError: total length is not correct"));
    
    return legal_step_check(block, out);
}

CheckerMessage AnswerChecker::cmp_check(const message &out, const message &ans){
    if (ans.d() == ans.totpair()){
        if (out.d() != out.totpair())
            return CheckerMessage(false, std::string("CmpError: out don't have correct tot_pairs"));
        if (out.totlength() > ans.totlength())
            return CheckerMessage(false, std::string("CmpError: out don't have correct tot_length"));
    }
    else{
        if (out.totpair() < ans.totpair())
            return CheckerMessage(false, std::string("CmpError: out don't have correct tot_pairs"));
        else return CheckerMessage(true, std::string("OK"));
    }
}
