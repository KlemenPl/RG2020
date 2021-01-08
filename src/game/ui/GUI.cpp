//
// Created by klemen on 07/01/2021.
//

#include "GUI.h"
#include "../../engine/ResourceManager.h"
#include "../../engine/input/Input.h"
#include "../Game.h"

GUI::GUI(Renderer2D *renderer2D) : renderer2D(renderer2D)
{
    whitePixel = ResourceManager::getTexture2D("sshape");
}

void GUI::initInput()
{
    std::function<bool(float, float, float, float)> mouseMove =
            [this](float mouseX, float mouseY, float deltaX, float deltaY) -> bool {
                mousePos.x = mouseX;
                mousePos.y = Game::height - mouseY;
                return false;
            };

    std::function<bool(float, float)> mouseClick =
            [this](float mouseX, float mouseY) -> bool {
                mousePos.x = mouseX;
                mousePos.y = Game::height - mouseY;
                mouseClicked = true;
                return false;
            };
    Input::addMouseMoveEvent(MouseMoveEvent(mouseMove));
    Input::addMouseButtonEvent(ButtonEvent(MOUSE_BUTTON_1, PRESS, mouseClick));
}

void GUI::begin()
{
    renderer2D->begin();
}
void GUI::end()
{
    renderer2D->end();
    mouseClicked = false;
}

bool contains(const glm::vec4 &pos, const glm::vec2 &point)
{
    return point.x > pos.x && point.x < pos.x + pos.z &&
           point.y > pos.y && point.y < pos.y + pos.w;
}

void GUI::Rect(const Color &color, const glm::vec4 &pos)
{
    renderer2D->draw(whitePixel._get_ref(), glm::vec2(pos.x, pos.y), glm::vec2(pos.z, pos.w),
                     0, 0, 1, 1, color);
}
void GUI::TextLabel(Ref<BitmapFont> font, const std::string &text, const glm::vec4 &pos,
                    float scale, const Color &fontColor)
{
    renderer2D->draw(font._get_ref(), text, glm::vec2(pos.x, pos.y), scale, scale, fontColor);
}
std::pair<bool, bool> GUI::TextButton(Ref<BitmapFont> font, const std::string &text, const glm::vec4 &pos,
                                      float scale, const Color &normalColor, const Color &hoveredColor)
{
    bool hovered = contains(pos, mousePos);

    const Color &useColor = hovered ? hoveredColor : normalColor;
    TextLabel(font, text, pos, scale, useColor);

    return {mouseClicked && hovered, hovered};
}

std::pair<bool, bool> GUI::ImageButton(Ref<Texture2D> image, const glm::vec4 &pos,
                                       Ref<BitmapFont> font, const std::string &text,
                                       float scale, const Color &normalColor, const Color &hoveredColor)
{
    bool hovered = contains(pos, mousePos);

    const Color &useColor = hovered ? hoveredColor : normalColor;
    renderer2D->draw(*image,glm::vec2(pos.x,pos.y),glm::vec2(pos.z,pos.w));
    TextLabel(font, text, pos, scale, useColor);

    return {mouseClicked && hovered, hovered};
}
