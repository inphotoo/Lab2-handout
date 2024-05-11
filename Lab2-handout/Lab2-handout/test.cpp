#include "src/hnsw.hpp"

using namespace HNSWLab;


int main() {
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;

    // 插入元素
    minHeap.push(5);
    minHeap.push(2);
    minHeap.push(8);
    minHeap.push(1);
    minHeap.push(10);

    // 提取最小值
    while (!minHeap.empty()) {
        int minValue = minHeap.top();
        minHeap.pop();
        std::cout << minValue << " ";
    }
    std::cout << std::endl;
//    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
//    std::default_random_engine generator(seed);
//    std::uniform_int_distribution<int> distribution(1, 100);
//
//    // 初始化HNSW索引结构
//    HNSW hnsw;
//
//
//
//    for (int i = 0; i < 50; i++) {
//        // 生成随机数并插入HNSW索引结构中
//        int *vec = new int[3];
//        vec[0] = distribution(generator);
//        vec[1] = distribution(generator);
//        vec[2] = distribution(generator);
//        hnsw.insert(vec, i);
//
//        vec[0] += distribution(generator);
//        hnsw.insert(vec, 100 + i);
//
//        vec[1] += distribution(generator);
//        hnsw.insert(vec, 200 + i);
//
//        vec[2] += distribution(generator);
//    }
//    int *search_vec = new int[3];
//    search_vec[0] = 89;
//    search_vec[1] = 65;
//    search_vec[2] = 45;
//    auto vectors = hnsw.query( search_vec, 1);
//    for(auto vector : vectors)
//        std::cout << vector << std::endl;
//
//    return 0;
}