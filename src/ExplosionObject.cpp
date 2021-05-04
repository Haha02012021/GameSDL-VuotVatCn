#include <iostream>
#include "ExplosionObject.h"

ExplosionObject::ExplosionObject()
{
    frame_height_ = 0;
    frame_widht_ = 0;
    frame_ = 0;
}

ExplosionObject::~ExplosionObject()
{

}

bool ExplosionObject::LoadImg(std::string path, SDL_Renderer* screen)
{
    bool ret = BaseObject::LoadImg(path, screen);

    if(ret) {
        frame_widht_ = rect_.w/NUM_FRAME_EXP;
        frame_height_ = rect_.h;
    }

    return ret;
}

void ExplosionObject::set_clip()
{
    if(frame_widht_ > 0 && frame_height_ > 0) {
        for (int i = 0; i < 8 ; i++) {
            frame_clip_[i].x = i*frame_widht_;
            frame_clip_[i].y = 0;
            frame_clip_[i].w = frame_widht_;
            frame_clip_[i].h = frame_height_;
        }
    }
}

void ExplosionObject::Show(SDL_Renderer* screen)
{
    SDL_Rect* current_clip = &frame_clip_[frame_];
    SDL_Rect renderQuad = {rect_.x, rect_.y, frame_widht_, frame_height_};
    if (current_clip != NULL) {
        renderQuad.w = current_clip->w;
        renderQuad.h = current_clip->h;
    }

    SDL_RenderCopy(screen, p_object_, current_clip, &renderQuad);
}
