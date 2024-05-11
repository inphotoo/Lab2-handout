#include "src/hnsw.hpp"

using namespace HNSWLab;


int main() {
    HNSW hnsw;
    int *vec = new int[2,3,4];
    std::cout<<"hello";
    hnsw.insert(vec ,1);
    hnsw.query(vec , 1);
    return 0;
}