#include "EndGame.h"

EndWindow::EndWindow()
{
    mWindow = NULL;
    mRenderer = NULL;
    mWindowID = 0;
    mWidth = 0;
    mHeight = 0;
    mMouseFocus = false;
    mKeyboardFocus = false;
    mFullScreen = false;
    mMinimized = false;
    mShown;
}

EndWindow::~EndWindow()
{

}

bool EndWindow::init()
{
    mWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH/2, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if(mWindow != NULL)
    {
        mMouseFocus = true;
        mKeyboardFocus = true;
        mWidth = SCREEN_WIDTH/2;
        mHeight = SCREEN_HEIGHT;

        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (mRenderer == NULL)
        {
            SDL_DestroyWindow(mWindow);
            mWindow = NULL;
        }
        else
        {
            SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

            mWindowID = SDL_GetWindowID(mWindow);

            mShown = true;
        }
    }
    else return false;

    return mWindow != NULL && mRenderer != NULL;
}

void EndWindow::handleEvent(SDL_Event& e)
{
    if(e.type == SDL_WINDOWEVENT && e.window.windowID == mWindowID)
    {
        bool updateCaption = false;

        switch(e.window.event)
        {
        case SDL_WINDOWEVENT_SHOWN:
            mShown = true;
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            mShown = false;
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            mWidth = e.window.data1;
            mHeight = e.window.data2;
            SDL_RenderPresent(mRenderer);
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            SDL_RenderPresent(mRenderer);
            break;
        case SDL_WINDOWEVENT_ENTER:
            mMouseFocus = true;
            updateCaption = true;
            break;
        case SDL_WINDOWEVENT_LEAVE:
            mMouseFocus = false;
            updateCaption = true;
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            mKeyboardFocus = true;
            updateCaption = true;
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            mKeyboardFocus = false;
            updateCaption = true;
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            mMinimized = true;
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            mMinimized = false;
            break;
        case SDL_WINDOWEVENT_RESTORED:
            mMinimized = false;
            break;
        case SDL_WINDOWEVENT_CLOSE:
            SDL_HideWindow(mWindow);
            break;
        }

        if(updateCaption)
        {
            std::stringstream caption;
            caption << "SDL Tutorial - ID: " << mWindowID << "MouseFocus: " << ( ( mMouseFocus ) ? "On" : "Off" ) << " KeyboardFocus:" << ( ( mKeyboardFocus ) ? "On" : "Off" );
            SDL_SetWindowTitle(mWindow, caption.str().c_str());
        }
    }
}

void EndWindow::focus()
{
    if (!mShown)
    {
        SDL_ShowWindow(mWindow);
    }

    SDL_RaiseWindow(mWindow);
}

void EndWindow::render()
{
    if(!mMinimized)
    {
        SDL_SetRenderDrawColor(mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(mRenderer);

        SDL_RenderPresent(mRenderer);
    }
}

bool EndWindow::isShown()
{
    if (mShown == true) return true;
    else return false;
}
