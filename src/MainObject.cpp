#include <iostream>
#include "MainObject.h"

MainObject::MainObject() {
    frame_ = 0;
    x_pos_ = 0;
    y_pos_ = 0;
    x_val_ = 0;
    y_val_ = 0;
    width_frame_ = 0;
    height_frame_ = 0;
    status_ = WALK_NONE;
    input_type_.left_ = 0;
    input_type_.right_ = 0;
    input_type_.down_ = 0;
    input_type_.up_ = 0;
    input_type_.jump_ = 0;
    on_ground_ = false;
    map_x_ = 0;
    map_y_ = 0;
    come_back_time_ = 0;
    money_count = 0;
    heart_count = 0;
    time_ = 0;
    is_die_ = false;
    win_ = 0;
}

MainObject::~MainObject() {

}

bool MainObject::LoadImg(std::string path, SDL_Renderer* screen) {
    bool ret = BaseObject::LoadImg(path, screen);

    if (ret == true) {
        if (rect_.w/8 < 30) width_frame_ = rect_.w;
        else width_frame_ = rect_.w/8;
        height_frame_ = rect_.h;
    }

    return ret;
}

SDL_Rect MainObject::GetRectFrame()
{
    SDL_Rect rect;
    rect.x = rect_.x;
    rect.y = rect_.y;
    rect.w = width_frame_;
    rect.h = height_frame_;

    return rect;
}

void MainObject::set_clips() {
    if (width_frame_ > 0 && height_frame_ > 0) {
        for (int i = 0; i < 8; i++) {
            frame_clip_[i].x = i*width_frame_;
            frame_clip_[i].y = 0;
            frame_clip_[i].w = width_frame_;
            frame_clip_[i].h = height_frame_;
        }

    }
}

void MainObject::Show(SDL_Renderer* des) {
    UpdateImagePlayer(des);

    if(input_type_.right_ == 1 && on_ground_ == true) {
        frame_++;
    }
    else {
        frame_ = 0;
    }
    if (come_back_time_ == 0) {
        rect_.x = x_pos_ - map_x_;
        rect_.y = y_pos_ - map_y_;
        frame_++;

        SDL_Rect* current_clip = &frame_clip_[frame_];

        SDL_Rect renderQuad = {rect_.x, rect_.y, width_frame_, height_frame_};

        SDL_RenderCopy(des, p_object_, current_clip, &renderQuad);
    }

}

void MainObject::MandelInputAction(SDL_Event events, SDL_Renderer* screen) {
    if(events.type == SDL_KEYDOWN) {
        switch (events.key.keysym.sym) {
            case SDLK_RIGHT:
                {
                    status_ = WALK_RIGHT;
                    input_type_.right_ = 1;
                    input_type_.left_ = 0;
                    UpdateImagePlayer(screen);
                }
                break;
            case SDLK_UP:
                {
                    input_type_.jump_ = 1;
                }
                break;
            default:
                break;
        }
    }
    else if (events.type == SDL_KEYUP) {
        switch (events.key.keysym.sym) {
            case SDLK_RIGHT:
                {
                    input_type_.right_ = 0;
                }
                break;
            case SDLK_LEFT:
                {
                    input_type_.left_ = 0;
                }
                break;
        }
    }

}


void MainObject::DoPlayer(Map& map_data, int win_over, SDL_Renderer* screen) {
    if (come_back_time_ == 0) {
        x_val_ = 0;
        y_val_ += 0.8;

        if (y_val_ >= MAX_FALL_SPEED)
        {
            y_val_ = MAX_FALL_SPEED;
        }

        if (win_over == 1)
        {
            GameOver(screen);
        }
        else
        {
            x_val_ += (PLAYER_SPEED + PlAYER_ACCE*time_);
            time_+=0.3;

            if (input_type_.right_ == 1) { //&& on_ground_ == true) {
                x_val_ += PLAYER_SPEED;
                y_val_ = 0;
            }

            if (input_type_.jump_ == 1)
            {
                Mix_PlayChannel(-1, g_sound_jump, 0);
                if (on_ground_ == true)
                {
                    y_val_ = PLAYER_JUMP_VALUE;
                }
                input_type_.jump_ = 0;
                on_ground_ = false;
            }

            CheckToMap(map_data);
            CenterEntityOnMap(map_data);
        }
    }

    if (come_back_time_ > 0) {
        come_back_time_--;
        if (come_back_time_ == 0) {
            on_ground_ = false;
            x_pos_ = 0;
            y_pos_ = 0;
            x_val_ = 0;
            y_val_ = 0;
        }
    }
}

void MainObject::CenterEntityOnMap(Map& map_data) {
    map_data.start_x_ = x_pos_ - (SCREEN_WIDTH/2);
    if(map_data.start_x_ < 0) {
        map_data.start_x_ = 0;
    }
    else if (map_data.start_x_ + SCREEN_WIDTH >= map_data.max_x) {
        map_data.start_x_ = map_data.max_x - SCREEN_WIDTH;
    }

    map_data.start_y_ = y_pos_ - (SCREEN_HEIGHT/2);
    if (map_data.start_y_ < 0) {
        map_data.start_y_ = 0;
    }
    else if (map_data.start_y_ + SCREEN_HEIGHT >= map_data.max_y) {
        map_data.start_y_ = map_data.max_y - SCREEN_HEIGHT;
    }
}

void MainObject::CheckToMap(Map& map_data) {
    int x1 = 0;
    int x2 = 0;

    int y1 = 0;
    int y2 = 0;


    //Check horizontal
    int height_min = height_frame_ < TILE_SIZE ? height_frame_ : TILE_SIZE;

    x1 = (x_pos_ + x_val_)/TILE_SIZE;
    x2 = (x_pos_ + x_val_ + width_frame_ - 1)/TILE_SIZE;

    y1 = (y_pos_)/TILE_SIZE;
    y2 = (y_pos_ + height_min - 1)/TILE_SIZE;

    if(x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y) {
        if (x_val_ > 0) //main object is moving to right
        {
            int val1 = map_data.tile[y1][x2];
            int val2 = map_data.tile[y2][x2];

            if (val1 == STATE_MONEY || val2 == STATE_MONEY) {
                map_data.tile[y1][x2] = 0;
                map_data.tile[y2][x2] = 0;
                IncreaseMoney();
            }
            else
                if (val1 == STATE_BLACKHEART || val2 == STATE_BLACKHEART) {
                    map_data.tile[y1][x2] = 0;
                    map_data.tile[y2][x2] = 0;
                    IncreaseHeart();
                }

            else
                if (val1 == STATE_REDHEART || val1 == 13 || val1 == 14 || val1 == 15 ||
                    val2 == STATE_REDHEART || val2 == 13 || val2 == 14 || val2 == 15)
                {
                    win_ = 1;
                }

            else {
                if (val1 != BLANK_TILE || val2 != BLANK_TILE)
                {
                    x_pos_ = x2*TILE_SIZE;
                    x_pos_ -= (width_frame_ + 1);
                    x_val_ = 0;
                }
            }
        }
        else if (x_val_ < 0) {
            int val1 = map_data.tile[y1][x1];
            int val2 = map_data.tile[y2][x1];

            if (val1 == STATE_MONEY || val2 == STATE_MONEY) {
                map_data.tile[y1][x1] = 0;
                map_data.tile[y2][x1] = 0;
                IncreaseMoney();
            }
            else
                if (val1 == STATE_BLACKHEART || val2 == STATE_BLACKHEART) {
                    map_data.tile[y1][x1] = 0;
                    map_data.tile[y2][x1] = 0;
                    IncreaseHeart();
                }

            else
                if (val1 == STATE_REDHEART || val1 == 13 || val1 == 14 || val1 == 15 ||
                    val2 == STATE_REDHEART || val2 == 13 || val2 == 14 || val2 == 15) {
                    win_ = 1;
                }

            else {
                if (val1 != BLANK_TILE || val2 != BLANK_TILE) {
                    x_pos_ = (x1 + 1)*TILE_SIZE;
                    x_val_ = 0;
                }
            }
        }
    }

    int width_min = width_frame_ < TILE_SIZE ? width_frame_ : TILE_SIZE;
    x1 = (x_pos_)/TILE_SIZE;
    x2 = (x_pos_ + width_min)/TILE_SIZE;

    y1 = (y_pos_ + y_val_)/TILE_SIZE;
    y2 = (y_pos_ + y_val_ + height_frame_ - 1)/TILE_SIZE;

    if (x1 >= 0 && x2 < MAX_MAP_X && y1 >= 0 && y2 < MAX_MAP_Y) {
        if (y_val_ > 0) {
            int val1 = map_data.tile[y2][x1];
            int val2 = map_data.tile[y2][x2];
            if (val1 == STATE_MONEY || val2 == STATE_MONEY) {
                map_data.tile[y2][x1] = 0;
                map_data.tile[y2][x2] = 0;
                IncreaseMoney();
            }
            else
                if (val1 == STATE_BLACKHEART || val2 == STATE_BLACKHEART) {
                    map_data.tile[y2][x1] = 0;
                    map_data.tile[y2][x2] = 0;
                    IncreaseHeart();
                }
            else
                if (val1 == STATE_REDHEART || val1 == 13 || val1 == 14 || val1 == 15 ||
                    val2 == STATE_REDHEART || val2 == 13 || val2 == 14 || val2 == 15) {
                    win_ = 1;
                }
            else {
                if ( val1!= BLANK_TILE ||  val2!= BLANK_TILE) {
                    y_pos_ = y2*TILE_SIZE;
                    y_pos_ -= (height_frame_ + 1);
                    y_val_ = 0;

                    on_ground_ = true;
                    if (status_ == WALK_NONE) {
                        status_ = WALK_RIGHT;
                    }
                }
            }
        }
        else if (y_val_ < 0) {
            int val1 = map_data.tile[y1][x1];
            int val2 = map_data.tile[y1][x2];
            if (val1 == STATE_MONEY || val2 == STATE_MONEY) {
                map_data.tile[y1][x1] = 0;
                map_data.tile[y1][x2] = 0;
                IncreaseMoney();
            }
            else
                if (val1 == STATE_BLACKHEART || val2 == STATE_BLACKHEART) {
                    map_data.tile[y1][x1] = 0;
                    map_data.tile[y1][x2] = 0;
                    IncreaseHeart();
                }
            else
                if (val1 == STATE_REDHEART || val1 == 13 || val1 == 14 || val1 == 15 ||
                    val2 == STATE_REDHEART || val2 == 13 || val2 == 14 || val2 == 15) {
                    win_ = 1;
                }
            else {
                if (val1 != BLANK_TILE || val2 != BLANK_TILE){
                    y_pos_ = (y1 + 1)*TILE_SIZE;
                    y_val_ = 0;

                }
            }
        }
    }

    x_pos_ += x_val_;
    y_pos_ += y_val_;

    if (x_pos_ < 0) {
        x_pos_ = 0;
    }
    else if (x_pos_ + width_frame_ > map_data.max_x) {
        x_pos_ = map_data.max_x - width_frame_ - 1;
    }

    if (y_pos_ > map_data.max_y) {
        come_back_time_ = 60;
    }
}

void MainObject::GameOver(SDL_Renderer* screen)
{
    x_val_ = 0;
    is_die_ = true;
}

void MainObject::IncreaseMoney() {
    money_count++;
}

void MainObject::IncreaseHeart() {
    heart_count++;
}

void MainObject::UpdateImagePlayer(SDL_Renderer* des) {
    LoadImg("img//player_right.png", des);
}

