//
// Created by klemen on 08/01/2021.
//

#ifndef TOWERDEFENSE_PREVIEWTURRET_H
#define TOWERDEFENSE_PREVIEWTURRET_H

#include "../entity/Turret.h"
#include "../../engine/graphics/Color.h"
#include "../../engine/graphics/BitmapFont.h"
#include <string>

class PreviewTurret
{
private:
    Color backgroundColour;
    Turret turret;
    std::string text;
    Ref<BitmapFont> font;





};


#endif //TOWERDEFENSE_PREVIEWTURRET_H
