#include "src/hnsw.hpp"

using namespace HNSWLab;


int main() {
    HNSW hnsw;
    int *vec = new int[3];
    vec[0] = 1;
    vec[1] = 65;
    vec[2] = 45;
    for(int i = 0 ; i < 100; i++)
    {
        hnsw.insert(vec ,i);
        vec[0]+= 2;
        hnsw.insert(vec ,100+ i);
        vec[1]+=1;
        hnsw.insert(vec ,200 + i);
        vec[2]+=1;
    }
    auto vectors = hnsw.query(vec , 1);
    for(auto vector : vectors)
        std::cout << vector << std::endl;

    return 0;
}