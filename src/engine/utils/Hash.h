//
// Created by klemen on 05/01/2021.
//

#ifndef TOWERDEFENSE_HASH_H
#define TOWERDEFENSE_HASH_H
namespace Utils {
    template<typename T>
    inline void hash_combine(std::size_t &seed, const T &v)
    {
        std::hash <T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}
#endif //TOWERDEFENSE_HASH_H
