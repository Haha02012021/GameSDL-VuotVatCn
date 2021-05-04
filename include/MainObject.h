#ifndef MAIN_OBJECT_H
#define MAIN_OBJECT_H

#include "CommonFunc.h"
#include "BaseObject.h"

#define GRAVITY_SPEED 0.8
#define MAX_FALL_SPEED 10
#define PLAYER_SPEED 4
#define PLAYER_JUMP_VALUE -24
#define PlAYER_ACCE 0.01

class MainObject : public BaseObject {
public:
    MainObject();
    ~MainObject();

    enum WalkType {
        WALK_NONE = 0,
        WALK_RIGHT = 1,
        WALK_LEFT = 2,
    };
    bool LoadImg(std::string path, SDL_Renderer* screen);
    void Show(SDL_Renderer* des);
    void MandelInputAction(SDL_Event events, SDL_Renderer* screen);
    void set_clips();

    void DoPlayer(Map& map_data, int win_over, SDL_Renderer* screen);
    void CheckToMap(Map& map_data);
    void SetMapXY(const int map_x, const int map_y) {map_x_ = map_x; map_y_ = map_y;};
    void CenterEntityOnMap(Map& map_data);
    void UpdateImagePlayer(SDL_Renderer* des);
    void IncreaseMoney();
    void IncreaseHeart();
    SDL_Rect GetRectFrame();

    int get_frame_width() const {return width_frame_;};
    int get_frame_height() const {return height_frame_;};
    void set_comeback_time(const int& cb_time) {come_back_time_ = cb_time;};
    int GetMoneyCount() const {return money_count;};
    int GetHeartCount() const {return heart_count;};

    int GetWin() const {return win_;};
    bool GetIsDie() const {return is_die_;};
    void GameOver(SDL_Renderer* screen);

private:
    int money_count;
    int heart_count;

    int win_;
    bool is_die_;

    float time_;

    float x_val_;
    float y_val_;

    float x_pos_;
    float y_pos_;

    int width_frame_;
    int height_frame_;

    SDL_Rect frame_clip_[8];
    Input input_type_;
    int frame_;
    int status_;
    bool on_ground_;

    int map_x_;
    int map_y_;

    int come_back_time_;
};

#endif // MAIN_OBJECT_H





