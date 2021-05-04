#ifndef PLAYER_POWER_H
#define PLAYER_POWER_H

#include "CommonFunc.h"
#include "BaseObject.h"
#include "MainObject.h"

class PlayerPower : public BaseObject
{
public:
    PlayerPower();
    ~PlayerPower();

    void SetNum(const int& num) {number_ = num;}
    void AddPos(const int& xPos);
    void Show(SDL_Renderer* screen);
    void Init(SDL_Renderer* screen);

    void InitCrease();
    void Decrease();

    int GetNumberDie() const {return number_;};

private:
    int number_;
    std::vector<int> pos_list_;

};

class PlayerMoney : public BaseObject
{
public:
    PlayerMoney();
    ~PlayerMoney();

    void InitMoney(std::string path, SDL_Renderer* screen);
    void Show(SDL_Renderer* screen);
    void SetPos(const int& x, const int& y) {x_pos_ = x; y_pos_ = y;};

private:
    int x_pos_;
    int y_pos_;
};
#endif // PLAYER_POWER_H

