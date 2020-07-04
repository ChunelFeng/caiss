//
// Created by Chunel on 2020/7/4.
// 计算两个字符串的编译距离，用于去重
//

#ifndef CAISS_EDITDISTANCEPROC_H
#define CAISS_EDITDISTANCEPROC_H

#include <vector>
#include "../UtilsProc.h"
using namespace std;


class EditDistanceProc : public UtilsProc {

public:
    /**
     * 是否超过编辑距离
     * @param fst
     * @param snd
     * @param dist
     * @return
     */
    static bool BeyondEditDistance(const string &fst, const string &snd, const unsigned int dist) {
        return (EditDistanceProc::calc(fst, snd) > dist);
    }

    /**
     * 计算两个数据之间的编辑距离
     * @param fst
     * @param snd
     * @return
     */
    static unsigned int calc(const string &fst, const string &snd) {

        string longStr = std::move(fst);
        string shortStr = std::move(snd);
        if (longStr.length() < shortStr.length()) {
            swap(longStr, shortStr);    // 确保长的在第一个
        }

        int longLen = longStr.size() + 1;    // 用于初始化的值，均加一操作
        int shortLen = shortStr.size() + 1;

        vector<int> vecBefore(shortLen, 0);
        vector<int> vecCur(shortLen, 0);

        for (int i = 0; i < longLen; i++) {
            for (int j = 0; j < shortLen; j++) {
                if (i == 0) {
                    vecCur[j] = j;
                } else if (j == 0) {
                    vecCur[j] = i;
                } else {
                    if (longStr[i-1] == shortStr[j-1]) {
                        vecCur[j] = vecBefore[j-1];
                    } else {
                        vecCur[j] = std::min(std::min(vecBefore[j], vecCur[j-1]), vecBefore[j-1]) + 1;
                    }
                }
            }
            vecBefore = vecCur;    // 记录上一条信息
        }

        return (unsigned int)vecCur.back();    // 返回最后一个数据
    }


private:
    explicit EditDistanceProc() {
        return;
    }

    ~EditDistanceProc() {
        return;
    }
};

#endif //CAISS_EDITDISTANCEPROC_H
