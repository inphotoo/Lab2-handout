#pragma once
#include <cmath>

namespace HNSWLab {
    int M = 30; //每一个节点进来需要连接的数量
    int M_max = 30;  // 每一个节点最大可连接数，不等于M的原因是先插如的结点可能被后续不断地添加连接，增大。
    int ef_construction = 100;// 当查找节点的时候，候选结点的数量
    int ef_search = 100;
    double mult_ = 1 / log(1.0 * M);
}