//
// Created by klemen on 11/12/2020.
//

#include <string>
#include "BitmapFont.h"

BitmapFont::BitmapFont()
{}

BitmapFont::~BitmapFont()
{}


float BitmapFont::getTextWidth(const std::string &text, float scaleX)
{
    float width = 0;
    for (char c:text)
    {
        const FontCharacter &fc = charData.at(c);

        width += fc.xAdvance*scaleX;

    }
    return width;
}
void BitmapFont::generate(float fontSize, int atlasWidth, int atlasHeight, int padding, int startChar, int numChars,
                          unsigned char *data, uint32_t dataSize)
{
    stbtt_packedchar tChardata[numChars];

    // allocating temporary bitmap
    unsigned char *tmp_bitmap = new unsigned char[atlasWidth * atlasHeight];

    unsigned int ftex;
    stbtt_pack_context pc;
    stbtt_PackBegin(&pc, tmp_bitmap, atlasWidth, atlasHeight, 0, padding, nullptr);
    // todo look into oversampling
    stbtt_PackFontRange(&pc, data, 0, fontSize, startChar, numChars, tChardata);
    stbtt_PackEnd(&pc);

    // generating texture
    glGenTextures(1, &ftex);
    glBindTexture(GL_TEXTURE_2D, ftex);

    // inserting tChardata in our own (normalized) struct
    for (int i = 0; i < numChars; i++)
    {
        stbtt_packedchar &stb_char = tChardata[i];
        FontCharacter character{
                Region2D(ftex, (float) stb_char.x0 / atlasWidth, (float) stb_char.y0 / atlasHeight,
                         (float) stb_char.x1 / atlasWidth, (float) stb_char.y1 / atlasHeight),
                stb_char.xoff, stb_char.yoff, stb_char.xadvance,
                stb_char.xoff2, stb_char.yoff2};
        //charData[(char) (startChar + i)] = character;
        charData.insert_or_assign((char) (startChar + i), character);
    }

    // https://stackoverflow.com/questions/8172648/gl-alpha8-was-removed-from-opengl-3-1-what-are-the-alternatives
    // swizzling to make all components look like RED (so we can colour text by multiplying in shader)
    GLint const swizzle[] = {GL_RED, GL_RED, GL_RED, GL_RED};
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, swizzle[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, swizzle[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, swizzle[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, swizzle[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 1024, 1024, 0, GL_RED, GL_UNSIGNED_BYTE, tmp_bitmap);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, tmp_bitmap);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    // deleting tmp_bitmap
    delete[] tmp_bitmap;

    bitmapTexture.ID = ftex;
    bitmapTexture.width = atlasWidth;
    bitmapTexture.width = atlasHeight;


}

void BitmapFont::dispose()
{
    charData.clear();
    bitmapTexture.dispose();
}