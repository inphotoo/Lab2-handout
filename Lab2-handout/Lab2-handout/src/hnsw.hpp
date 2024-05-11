#pragma once
#include <iostream>
#include "base.hpp"
#include "../util/util.hpp"
#include <vector>
#include <cstring>

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <random>
#include <cassert>

namespace HNSWLab {

    
    class HNSW : public AlgorithmInterface {
    private:
        //每一个结点
        class hnswNode{
        public:
            int label; //数据标签（id)
            hnswNode* child = nullptr;//下一层
            const int *item = nullptr; // 特征向量
            std::vector<hnswNode*> neighbors; //邻居们
            hnswNode(){}
            hnswNode(const int *item , int label):item(item),label(label){}
        };
        //每一层
        class hnswLayer{
        public:
            std::vector<hnswNode*> nodes ; // 这一层结点们
            int level; //当前的层数
        };
        std::vector<hnswLayer *> layers; // 所有层
        hnswNode * enterPoint = nullptr; // 进入的节点
        int maxL = 0;// ep对应的层
    public:
        // you can add more parameter to initialize HNSW
        HNSW() {}

        void insert(const int *item, int label);

        std::vector<int> query(const int *query, int k);

        std::vector<hnswNode* > search_layer(const int *item , hnswNode * ep , int ef );

        std::vector<hnswNode* > select_neighbors(const int *item , int M , std::vector<hnswNode* > W);

        ~HNSW() {}
    };

    /**
     * input:
     * item: the vector to be inserted
     * label: the label(id) of the vector
    */
    void HNSW::insert(const int *item, int label) {

        std::vector<hnswNode*> W; //现在发现的最近邻元素集合
        int L = get_random_level(); //决定q从哪一层开始插入
        hnswNode *ep = enterPoint; // 把进入点设置为enterPoint
        for(int lc = maxL ; lc >= L + 1 ; lc --)
        {
            //从每一层中，找到一个最近的节点
            W = search_layer(item , ep , 1 );
            ep = W[0];
        }

        //有Bug,每一层的节点应该不同
        //后续要么二维数组，要么多几个Node
        hnswNode *q = new hnswNode(item,label);
        while(L >= layers.size())
            layers.push_back(new hnswLayer());
        hnswNode *insertNode = q;
        layers[0]->nodes.push_back(insertNode);
        for(int i = 0 ; i < L ; i++)
        {
            hnswNode* newNode = new hnswNode(item,label);
            newNode->child = insertNode;
            insertNode = newNode;
            layers[i+1]->nodes.push_back(newNode);
            //q对应的是最高层节点，所以没有到最高，就往上走
            if(i < maxL)
                q = insertNode;
        }

        // ⾃L层向第0层逐层搜索
        for( int lc = (maxL > L ?  L : maxL) ;  lc > 0 ;  lc--)
        {
            //找到每一层中，最近的ef_construction个节点
            W = search_layer(item , ep , ef_construction );
            //对于每一个节点，相互添加邻居,新节点至多添加M个邻居
            std::vector<hnswNode*> neighbors = select_neighbors(item , M, W);
            //对于每一个neighbor，都只能有至多M个neighbrs
            //如果超出，那么只选近的
            //感觉这里可优化，添加一个是否满了的变量，减少时间
            for(auto neighbor : neighbors)
            {
                if(neighbor->neighbors.size() > M )
                {
                    //选取对于neighbor最近的M个邻居
                    neighbor->neighbors = select_neighbors(  neighbor->item , M , neighbor->neighbors);
                }
            }
            //最近的，作为下一层入口
            ep = neighbors[0];
            ep = ep->child;
        }
        if(L > maxL)
        {
            enterPoint = layers[L]->nodes[0];
        }
        //TODO
    }

    /**
     * @param query: the vector to be queried
     * @param k: the number of nearest neighbors to be returned
     *
     * @return a vector of labels of the k nearest neighbors
    */
    std::vector<int> HNSW::query(const int *query, int k) {
        std::vector<int> res;
        //TODO 
        return res;
    }



    /**
     *
     * @param item 插入的新元素的值（向量）
     * @param ep 入口元素
     * @param ef 需要返回的近邻数量
     * @param lc 层数
     * @return 该元素ef个近邻节点
     */
    std::vector<HNSW::hnswNode *> HNSW::search_layer(const int *item, HNSW::hnswNode *ep, int ef) {
        int vec_dim = sizeof (item)/ sizeof(item[0]);
        auto lessCmp = [item , vec_dim](hnswNode* node1 , hnswNode* node2)
        {
            return l2distance(node1->item , item , vec_dim) < l2distance(node2->item , item , vec_dim);
        };
        auto moreCmp = [item , vec_dim](hnswNode* node1 , hnswNode* node2)
        {
            return l2distance(node1->item , item , vec_dim) > l2distance(node2->item , item , vec_dim);
        };
        std::vector<HNSW::hnswNode *> v; //已近访问过的
        std::priority_queue<hnswNode* , std::vector<hnswNode*> , decltype(moreCmp)> minHeap_C(moreCmp);
        std::priority_queue<hnswNode* , std::vector<hnswNode*> , decltype(lessCmp)> maxHeap_W(lessCmp);
        v.push_back(ep);
        minHeap_C.push(ep);
        maxHeap_W.push(ep);

        while(!minHeap_C.empty())
        {
            // 取出C中q的最近邻c
            hnswNode* c = minHeap_C.top();
            minHeap_C.pop();
            // 取出W中q的最远点f
            hnswNode* f = maxHeap_W.top();
            // 如果c已经⽐最近邻所有节点离⽬标节点更远，则⽆需探索
            if(l2distance(c->item , item , vec_dim) > l2distance(f->item , item , vec_dim))
                break;
            for(auto e : c->neighbors)
            {
                //如果没有访问过
                auto it = std::find(v.begin() , v.end() , e);
                if(it == v.end())
                {
                    //not be found
                    //加入访问过的节点
                    v.push_back(e);
                    f = maxHeap_W.top();
                    //如果距离比已发现的所有节点小，或者已发现的个数不足
                    //增加候选结点，
                    //同时保证已发现的结点数大小稳定
                    if(l2distance(e->item , item , vec_dim) < l2distance(f->item , item , vec_dim ) || maxHeap_W.size() <= ef)
                    {
                        minHeap_C.push(e);
                        maxHeap_W.push(e);
                        if(maxHeap_W.size() > ef)
                            maxHeap_W.pop();
                    }
                }

            }
        }
        //最终返回剩下的节点
        std::vector<hnswNode*> vec;
        while (!maxHeap_W.empty()) {
            hnswNode* topNode = maxHeap_W.top();
            maxHeap_W.pop();
            vec.push_back(topNode);
        }
        return vec;
    }

    /**
     *
     * @param item 插入的新元素的值（向量）
     * @param M 返回的至多节点数
     * @param W 节点们
     * @return
     */
    std::vector<HNSW::hnswNode *> HNSW::select_neighbors(const int *item, int M, std::vector<hnswNode *> W) {
        if(W.size() <= M ) return W;
        else
        {
            auto cmp = [item](hnswNode* node1 , hnswNode* node2)
            {
                int vec_dim = sizeof (item)/ sizeof(item[0]);
                return l2distance(node1->item , item , vec_dim) < l2distance(node2->item , item , vec_dim);
            };
            //decltype(cmp) 用于获取 lambda 表达式 cmp 的类型
            //maxHeap大顶堆，用cmp作为函数比较对象
            std::priority_queue<hnswNode* , std::vector<hnswNode*> , decltype(cmp)> maxHeap(cmp);
            //遍历每一个节点
            int size = 0;
            for(auto node : W)
            {
                //插入节点，如果超出上限就再pop一个
                maxHeap.push(node);
                if(size > M)
                    maxHeap.pop();
                //超出目标个数，
                size++;
            }
            //最终返回剩下的节点
            std::vector<hnswNode*> vec;
            while (!maxHeap.empty()) {
                hnswNode* topNode = maxHeap.top();
                maxHeap.pop();
                vec.push_back(topNode);
            }
            return vec;
        }
    }

}