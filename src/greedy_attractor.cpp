#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <unordered_set>
#include "lcp_interval.hpp"
#include "greedy_attractor.hpp"
using namespace std;

namespace stool
{

void GreedyAttractorAlgorithm::addCount(LCPInterval &interval, int64_t addValue)
{
    vector<uint64_t> posArr;
    for (uint64_t i = interval.i; i <= interval.j; i++)
    {
        posArr.push_back(sa[i]);
    }
    sort(posArr.begin(), posArr.end());
    for (uint64_t x = 0; x < posArr.size(); x++)
    {
        uint64_t pos = posArr[x];
        uint64_t nextPos = x + 1 < posArr.size() ? posArr[x + 1] : UINT64_MAX;
        uint64_t endPos = pos + interval.lcp - 1 < nextPos ? pos + interval.lcp - 1 : nextPos - 1;
        for (uint64_t y = pos; y <= endPos; y++)
        {
            if (this->countVec[y] != UINT64_MAX)
            {
                this->countVec[y] += addValue;
                uint64_t blockPos = y / this->blockSize;
                this->changedVec[blockPos] = true;
            }
        }
    }
}
/*
void GreedyAttractorAlgorithm::updateMaxPosVec()
{
    for (uint64_t i = 0; i < this->blockNum; i++)
    {
        if (this->changedVec[i])
        {
            uint64_t beg = i * this->blockSize;
            uint64_t max = 0;
            uint64_t maxPos = UINT64_MAX;
            for (uint64_t x = 0; x < this->blockSize; x++)
            {
                if (this->countVec[beg + x] != UINT64_MAX && max < this->countVec[beg + x])
                {
                    max = this->countVec[beg + x];
                    maxPos = beg + x;
                }
            }
            this->maxPosVec[i] = maxPos;
            this->changedVec[i] = false;
        }
    }
}
*/
uint64_t GreedyAttractorAlgorithm::getMaxTPosition()
{
    uint64_t p = UINT64_MAX;
    uint64_t wholeMax = 0;

    for (uint64_t i = 0; i < this->blockNum; i++)
    {

        if (this->changedVec[i])
        {
            uint64_t beg = i * this->blockSize;
            uint64_t max = 0;
            uint64_t maxPos = UINT64_MAX;
            for (uint64_t x = 0; x < this->blockSize; x++)
            {
                if (this->countVec[beg + x] != UINT64_MAX && max < this->countVec[beg + x])
                {
                    max = this->countVec[beg + x];
                    maxPos = beg + x;
                }
            }
            this->maxPosVec[i] = maxPos;
            this->changedVec[i] = false;
        }
        uint64_t mPos = this->maxPosVec[i];

        if (mPos != UINT64_MAX && this->countVec[mPos] != UINT64_MAX && wholeMax < this->countVec[mPos])
        {
            p = mPos;
            wholeMax = this->countVec[mPos];
        }
    }
    return p;
}
void GreedyAttractorAlgorithm::getContainingIntervals(uint64_t pos, vector<LCPInterval> &outputIntervals)
{
    for (auto it = this->currentIntervals.begin(); it != this->currentIntervals.end(); ++it)
    {
        if (it->containsPosition(sa, pos))
        {
            outputIntervals.push_back(*it);
        }
    }
}
void GreedyAttractorAlgorithm::compute(vector<uint64_t> &sa, vector<LCPInterval> &intervals, uint64_t _blockSize, vector<uint64_t> &outputAttrs)
{
    GreedyAttractorAlgorithm algo(intervals, &sa, _blockSize);

    uint64_t prev = UINT64_MAX;
    uint64_t iter = 0;
    std::cout << "computing greedy attractors... : [" << algo.currentIntervals.size() << "]" << std::flush;

    while (algo.currentIntervals.size() > 0)
    {
        iter++;
        if (iter % 100 == 0)
            std::cout << "\r"
                      << "computing greedy attractors... : [" << algo.currentIntervals.size() << "]" << std::flush;
        uint64_t maxTPos = algo.getMaxTPosition();
        algo.addAttractor(maxTPos);
        outputAttrs.push_back(maxTPos);
    }
    std::cout << "[END]" << std::endl;
    sort(outputAttrs.begin(), outputAttrs.end());
}
void GreedyAttractorAlgorithm::addAttractor(uint64_t pos)
{
    vector<LCPInterval> maxPosIntervals;
    this->getContainingIntervals(pos, maxPosIntervals);
    for (auto &interval : maxPosIntervals)
    {
        this->addCount(interval, -1);
        this->currentIntervals.erase(interval);
    }
    this->countVec[pos] = UINT64_MAX;
    //this->updateMaxPosVec();
}
GreedyAttractorAlgorithm::GreedyAttractorAlgorithm(vector<LCPInterval> &intervals, vector<uint64_t> *_sa, uint64_t _blockSize) : sa(*_sa)
{
    std::cout << "Constructing Data Strutures..." << std::flush;

    this->blockSize = _blockSize;
    //constructISA(sa, isa);
    this->blockNum = (sa.size() / this->blockSize) + 1;
    this->changedVec.resize(this->blockNum, false);
    this->maxPosVec.resize(this->blockNum, UINT64_MAX);

    this->countVec.resize(this->blockNum * this->blockSize, 0);
    for (uint64_t i = 0; i < intervals.size(); i++)
    {
        addCount(intervals[i], 1);
        this->currentIntervals.insert(intervals[i]);

        if (i % 10000 == 0)
            std::cout << "\r"
                      << "Constructing Data Strutures... : [" << i << "/" << intervals.size() << "]" << std::flush;
    }
    intervals.resize(0);
    //this->updateMaxPosVec();
    std::cout << "[END]" << std::endl;

    /*
    for (auto it = this->currentIntervals.begin(); it != this->currentIntervals.end(); ++it)
    {
    }
    */

    /*
    uint64_t iter = 0;
    for (auto interval : intervals)
    {
    }
    */
}

} // namespace stool