#include "src/hnsw.hpp"

using namespace HNSWLab;


int main() {
    HNSW hnsw;
    int *vec = new int[2,3,4];
    int *vec2 = new int[2,3,5];
    std::cout<<"hello";
    hnsw.insert(vec ,1);
    hnsw.insert(vec2 ,2);
    hnsw.query(vec , 1);
    return 0;
}