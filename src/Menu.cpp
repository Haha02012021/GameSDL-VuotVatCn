#include "Menu.h"

Menu::Menu()
{

}

Menu::~Menu()
{

}

int Menu::ShowMenu(SDL_Renderer* screen, TTF_Font* font)
{
    SDL_Event m_event;

    BaseObject img_menu;
    bool ret_menu = img_menu.LoadImg("img//menu.png", screen);
    if (ret_menu == false) return 1;

    TextObject game_menu;
    game_menu.SetColor(TextObject::BLACK_TEXT);

    while (true)
    {
        //img_menu.Render(screen, NULL);

        while (SDL_PollEvent(&m_event))
        {
            switch (m_event.type)
            {
                case SDL_QUIT: return 1;
                break;
                case SDL_KEYDOWN:
                    switch(m_event.key.keysym.sym)
                    {
                        case SDLK_p: return 0;
                        break;
                        case SDLK_e: return 1;
                        break;
                    }
                    break;
            }
        }

        /*std::string str_time = "";
        Uint32 time_val = SDL_GetTicks()/1000;
        Uint32 val_time = 3 - time_val;
        std::string str_val = std::to_string(val_time);

        str_time += str_val;

        game_menu.SetText(str_time);
        game_menu.LoadFromRenderText(font, screen);
        game_menu.RenderText(screen, SCREEN_WIDTH - 200, 14);*/
    }

    SDL_RenderPresent(screen);
    return 1;
}
