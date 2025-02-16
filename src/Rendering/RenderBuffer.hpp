//
//  RenderBuffer.hpp
//  AsteroidsTestBed
//
//  Created by Chris Greening on 08/01/2021.
//

#ifndef RenderBuffer_hpp
#define RenderBuffer_hpp

#include <vector>
#include "box2d/box2d.h"

class Font;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t hold;
    bool laser;
} DrawInstruction_t;

class Game;

class RenderBuffer
{
private:
    int16_t _minX;
    int16_t _maxX;
    int16_t _minY;
    int16_t _maxY;
    int16_t _centerX;
    int16_t _centerY;
    float _scale;
    Font *_font;

    inline int16_t calc_x(float x)
    {
        return int16_t(std::max(std::min(_maxX, int16_t(_centerX + _scale * x)), _minX));
    }
    inline int16_t calc_y(float y)
    {
        return int16_t(std::max(std::min(_maxY, int16_t(_centerY + _scale * y)), _minY));
    }
    void renderSegment(bool laser, b2Vec2 start, const b2Vec2 &end, int min_hold = 1);
    b2Vec2 draw_text(b2Vec2 start, float x, float y, const char *text, bool measure, int only1char=-1);

public:
    RenderBuffer(int minX, int maxX, int minY, int maxY, int centerX, int centerY, float scale, Font *font);
    std::vector<DrawInstruction_t> *display_frame = NULL;
    std::vector<DrawInstruction_t> *drawing_frame = NULL;
    bool needs_render = false;
    void render_if_needed(Game *game);
    bool swapBuffers();
};

#endif /* RenderBuffer_hpp */
