
#include <iostream>
#include "CommonFunc.h"
#include "BaseObject.h"
#include "game-map.h"
#include "MainObject.h"
#include "Imptimer.h"
#include "TextObject.h"
#include "PlayPower.h"
#include "Geometric.h"
#include "ThreatObject.h"

BaseObject g_menu;
BaseObject g_background;
TTF_Font* font_time = NULL;
TTF_Font* font_menu = NULL;

int win = 0;

bool InitData() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    g_window = SDL_CreateWindow("B. Bean",
                                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                SCREEN_WIDTH, SCREEN_HEIGHT,
                                SDL_WINDOW_SHOWN);
    if (g_window == NULL) {
        success = false;
    }
    else {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if(g_screen == NULL) success = false;
        else {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags)) success = false;
        }

        if (TTF_Init() == -1) {
            success = false;
        }

        font_time = TTF_OpenFont("font//Pacifico.ttf", 20);
        if (font_time == NULL) {
            success = false;
        }

        font_menu = TTF_OpenFont("font//SEASRN__.ttf", 28);
        if (font_menu == NULL)
        {
            success = false;
        }

        if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
        {
            success = false;
        }

        g_sound_ex_main = Mix_LoadWAV("audio/FAIL #2.wav");

        if (g_sound_ex_main == NULL)
        {
            success = false;
        }

        g_background_music = Mix_LoadMUS("audio/Kevin MacLeod - Monkeys Spinning Monkeys.wav");

        if (g_background_music == NULL)
        {
            success = false;
        }

        g_sound_jump = Mix_LoadWAV("audio/Funny effect 21.wav");

        if (g_sound_jump == NULL)
        {
            success = false;
        }

        g_sound_win = Mix_LoadWAV("audio/HA HA (NELSON).wav");

        if (g_sound_win == NULL)
        {
            success = false;
        }
    }

    return success;
}

bool LoadBackground() {
    bool ret = g_background.LoadImg("img//background.png", g_screen);
    if (ret == false) return false;

    return true;
}

bool LoadMenu() {
    bool ret = g_menu.LoadImg("img//menu.png", g_screen);
    if (ret == false) return false;

    return true;
}

void close() {
    g_background.Free();
    g_menu.Free();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

    Mix_FreeChunk(g_sound_ex_main);
    g_sound_ex_main = NULL;

    Mix_FreeMusic(g_background_music);
    g_background_music = NULL;

    Mix_FreeChunk(g_sound_jump);
    g_sound_jump = NULL;

    Mix_FreeChunk(g_sound_win);
    g_sound_win = NULL;

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}

std::vector<ThreatsObject*> MakeThreatList()
{
    std::vector<ThreatsObject*> list_threats;

    ThreatsObject* threats_objs = new ThreatsObject[20];
    srand(time(NULL));
    for(int i = 0; i < 9; i++) {
        ThreatsObject* p_threat = threats_objs + i;
        p_threat->SetFly(0);
        if (p_threat != NULL) {
            int xp = rand() % (10000 - 500 + 1) + 500;
            p_threat->LoadThreats("img//threat_level.png", xp, 250, g_screen);

            list_threats.push_back(p_threat);
        }
    }

    ThreatsObject* threats_objs1 = new ThreatsObject[20];
    for(int i = 0; i < 9; i++) {
        ThreatsObject* p_threat = threats_objs1 + i;
        p_threat->SetFly(0);
        if (p_threat != NULL) {
            int xp = rand() % (18000 - 9800 + 1) + 9800;
            p_threat->LoadThreats("img//threat_level1.png", xp, 250, g_screen);

            list_threats.push_back(p_threat);
        }
    }

    ThreatsObject* threats_objs2 = new ThreatsObject[20];
    for(int i = 0; i < 14; i++) {
        ThreatsObject* p_threat = threats_objs2 + i;
        p_threat->SetFly(1);
        if (p_threat != NULL) {
            int xp = rand() % (25300 - 18000 + 1) + 18000;
            p_threat->LoadThreats("img//threat_level2.png", xp, 300 - 10*i, g_screen);

            list_threats.push_back(p_threat);
        }
    }

    return list_threats;
}


int main(int argc, char* argv[]) {

    ImpTimer fps_timer;

    if(InitData() == false) return -1;

    if (LoadBackground() == false) return -1;

    bool is_quit = false;

    int ret_menu = SDLCommonFunc::ShowMenu(g_screen, font_menu, "Play Game", "Exit", "img//MENU.png");
    if (ret_menu == 1) is_quit = true;

again_label:

    Uint32 start_time = SDL_GetTicks();

    GameMap game_map;
    game_map.LoadMap("map/map01.dat");
    game_map.LoadTiles(g_screen);

    MainObject p_player;
    p_player.LoadImg("img//player_right.png", g_screen);
    p_player.set_clips();

    PlayerPower player_power;
    player_power.Init(g_screen);

    PlayerMoney player_money;
    player_money.InitMoney("img//money_img.png", g_screen);
    player_money.SetPos(SCREEN_WIDTH*0.4 - 70, 4);

    PlayerMoney player_heart;
    player_heart.InitMoney("img//heart_img.png", g_screen);
    player_heart.SetPos(SCREEN_WIDTH*0.4 - 170, 7);

    std::vector<ThreatsObject*> threats_list = MakeThreatList();

    int num_die = 0;
    int score = 10;
    Uint32 add_time = 0;

    TextObject time_game;
    time_game.SetColor(TextObject::BLACK_TEXT);

    TextObject money_game, b_heart_game;
    money_game.SetColor(TextObject::BLACK_TEXT);
    b_heart_game.SetColor(TextObject::BLACK_TEXT);

    while (!is_quit) {
        fps_timer.start();
                    //If there is no music playing
                    if( Mix_PlayingMusic() == 0 )
                    {
                        //Play the music
                        Mix_PlayMusic( g_background_music, -1 );
                    }
                    //If music is being played
                    else
                    {
                        //If the music is paused
                        if( Mix_PausedMusic() == 1 )
                        {
                            //Resume the music
                            Mix_ResumeMusic();
                        }
                        //If the music is playing
                        else
                        {
                            //Pause the music
                            Mix_PauseMusic();
                        }
                    }
        while (SDL_PollEvent(&g_event) != 0) {
            if (g_event.type == SDL_QUIT) {
                is_quit = true;
            }

            p_player.MandelInputAction(g_event, g_screen);
        }

        SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
        SDL_RenderClear(g_screen);

        g_background.Render(g_screen, NULL);

        Map map_data = game_map.getMap();

        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);
        p_player.DoPlayer(map_data, win, g_screen);
        p_player.Show(g_screen);

        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);

        //Draw Geometric
        GeometricFormat rectangle_size(0, 0, SCREEN_WIDTH, 40);
        ColorData color_data(255, 255, 255);
        Geometric::RenderRectangle(rectangle_size, color_data, g_screen);

        GeometricFormat outline_size (0, 1, SCREEN_WIDTH - 1, 38);
        ColorData color_data2(0, 0, 0);
        Geometric::RenderOutline(outline_size, color_data2, g_screen);

        if (p_player.GetHeartCount() == 15 && score == 10)
        {
            score = 0;
            player_power.InitCrease();
        }

        player_power.Show(g_screen);
        player_money.Show(g_screen);
        player_heart.Show(g_screen);

        for(int i = 0; i < threats_list.size(); i++) {
            ThreatsObject* p_threat = threats_list.at(i);
            if (p_threat != NULL) {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->DoPlayer(map_data);
                p_threat->Show(g_screen);

                SDL_Rect rect_player = p_player.GetRectFrame();

                SDL_Rect rect_threat = p_threat->GetRectFrame();

                bool bCol = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
                if (bCol) {
                    Mix_PauseMusic();

                    Mix_PlayChannel(-1, g_sound_ex_main, 0);

                    num_die ++;
                    if (num_die <= player_power.GetNumberDie()) {
                        p_player.SetRect(0, 0);
                        p_player.set_comeback_time(40);
                        SDL_Delay(1000);
                        player_power.Decrease();
                        player_power.Render(g_screen);
                        continue;
                    }
                    else
                    {
                        win = 1;
                    }
                }
            }
        }

        Uint32 end_time = SDL_GetTicks();


        //Show game time
        std::string str_time = "Time: ";
        Uint32 time_val = (end_time - start_time)/1000;
        Uint32 val_time = 84 - time_val + add_time;
        if (p_player.GetMoneyCount() == 20)
        {
            add_time = 10;
        }

        if (val_time <= 0) {
            win = 1;
        }
        else
        {
            std::string str_val = std::to_string(val_time);
            str_time += str_val;

            time_game.SetText(str_time);
            time_game.LoadFromRenderText(font_time, g_screen);
            time_game.RenderText(g_screen, SCREEN_WIDTH - 100, 1);
        }

        int money_count = p_player.GetMoneyCount();
        std::string money_str = std::to_string(money_count);
        money_game.SetText(money_str);
        money_game.LoadFromRenderText(font_time, g_screen);
        money_game.RenderText(g_screen, SCREEN_WIDTH*0.4 - 100, 1);

        int bheart_count = p_player.GetHeartCount();
        std::string bheart_str = std::to_string(bheart_count);
        b_heart_game.SetText(bheart_str);
        b_heart_game.LoadFromRenderText(font_time, g_screen);
        b_heart_game.RenderText(g_screen, SCREEN_WIDTH*0.4 - 200, 1);

        SDL_RenderPresent(g_screen);

        if (p_player.GetWin() == 1)
        {
            Mix_PlayChannel(-1, g_sound_win, 0);
            Sleep(500);
            int ret_menu = SDLCommonFunc::ShowMenu(g_screen, font_menu,
                                                   "Play Again", "Exit",
                                                    "img//menu_win.png");
            if (ret_menu == 1)
            {
                is_quit = true;
                continue;
            }
            else
            {
                win = 0;
                is_quit = false;
                goto again_label;
            }
        }

        // Make menu game over
        bool game_over = p_player.GetIsDie();
        if (game_over == true)
        {
            Mix_PlayChannel(-1, g_sound_ex_main, 0);
            Sleep(500);
            int ret_menu = SDLCommonFunc::ShowMenu(g_screen, font_menu,
                                                   "Play Again", "Exit",
                                                    "img//menu_gameover.png");
            if (ret_menu == 1)
            {
                is_quit = true;
                continue;
            }
            else
            {
                win = 0;
                is_quit = false;
                goto again_label;
            }
        }

        int real_imp_time = fps_timer.get_ticks();
        int time_one_frame = 1000/FRAME_PER_SECOND; //ms

        if(real_imp_time < time_one_frame) {
            int delay_time = time_one_frame - real_imp_time;
            if (delay_time >= 0) SDL_Delay(delay_time);
        }
    }
    //}

    close();
    return 0;
}



