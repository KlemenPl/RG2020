//
// Created by klemen on 18/12/2020.
//

#ifndef TOWERDEFENSE_RENDERER3D_H
#define TOWERDEFENSE_RENDERER3D_H


class Renderer3D
{
private:
public:
    Renderer3D();
    virtual ~Renderer3D();

    // should not be copied!!
    Renderer3D(const Renderer3D &) = delete;
    Renderer3D &operator=(const Renderer3D &) = delete;
};


#endif //TOWERDEFENSE_RENDERER3D_H
