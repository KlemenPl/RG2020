//
// Created by klemen on 07/01/2021.
//

#ifndef TOWERDEFENSE_GUI_H
#define TOWERDEFENSE_GUI_H


#include <vec4.hpp>
#include "../../engine/graphics/Color.h"
#include "../../engine/Core.h"
#include "../../engine/graphics/BitmapFont.h"
#include "../../engine/rendering/Renderer2D.h"

class GUI
{
private:
    Renderer2D *renderer2D;
    Ref<Texture2D> whitePixel;

    glm::vec2 mousePos{};
    bool mouseClicked = false;

public:
    explicit GUI(Renderer2D *renderer2D);

    void initInput();

    void begin();
    void end();
    void Rect(const Color &color, const glm::vec4 &pos);
    void TextLabel(Ref<BitmapFont> font, const std::string &text, const glm::vec4 &pos,
                   float scale, const Color &fontColor = Colors::WHITE);
    std::pair<bool, bool>
    TextButton(Ref<BitmapFont> font, const std::string &text, const glm::vec4 &pos, float scale = 1.0f,
               const Color &normalColor = Colors::WHITE, const Color &hoveredColor = Colors::GRAY);
    std::pair<bool, bool> ImageButton(Ref<Texture2D> image, const glm::vec4 &pos,
                                      Ref<BitmapFont> font, const std::string &text,
                                      float scale,
                                      const Color &normalColor = Colors::WHITE,
                                      const Color &hoveredColor = Colors::GRAY);
};


#endif //TOWERDEFENSE_GUI_H
