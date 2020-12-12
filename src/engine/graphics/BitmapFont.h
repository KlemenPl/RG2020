//
// Created by klemen on 11/12/2020.
//

#ifndef TOWERDEFENSE_BITMAPFONT_H
#define TOWERDEFENSE_BITMAPFONT_H


#include <imstb_truetype.h>
#include <unordered_map>
#include <utility>
#include "Texture2D.h"
#include "Region2D.h"

// todo clean up a little bit
struct FontCharacter
{
    /* stb_truetype representation
    unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
    float xoff,yoff,xadvance;
    float xoff2,yoff2;
     */

    // already normalized texture coords
    Region2D region;

    float xOff, yOff, xAdvance;
    float xOff2, yOff2;

    FontCharacter(Region2D region, float xOff, float yOff, float xAdvance, float xOff2, float yOff2) :
            region(std::move(region)), xOff(xOff), yOff(yOff), xAdvance(xAdvance), xOff2(xOff2), yOff2(yOff2)
    {};

};

class BitmapFont
{
public:
    Texture2D bitmapTexture;
    std::unordered_map<char, FontCharacter> charData;
    unsigned int charDataSize;

    BitmapFont();

    void generate(float fontSize, int atlasWidth, int atlasHeight,
                  int padding, int startChar, int numChars,
                  unsigned char *data, uint32_t dataSize);


    float getTextWidth(const std::string& text, float scaleX);
    virtual ~BitmapFont();

    void dispose();

};


#endif //TOWERDEFENSE_BITMAPFONT_H
