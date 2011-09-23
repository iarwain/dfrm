#ifndef __VOX_H_
#define __VOX_H_

//! Includes
#define __NO_SCROLLED__ // Uncomment this define to prevent the embedded editor (ScrollEd) from being compiled
#include "Scroll.h"

#include "VoxObject.h"
#include "VoxCursor.h"
#include "VoxSoundSurface.h"
#include "VoxOctaveSurface.h"


//! Vox class
class Vox : public Scroll<Vox>
{
public:

  enum GameState
  {
    GameStateSplash = 0,
    GameStateMenu,
    GameStateRun,
    GameStatePause,
    GameStateEnd,

    GameStateNumber,

    GameStateNone = orxENUM_NONE

  };

                orxFLOAT        GetTime() const         {return mfTime;}
                orxOBJECT *     GetCameraObject() const {return mpstCameraObject;}
                GameState       GetGameState() const    {return meGameState;}
          const orxOBJECT *     GetSplashObject()       {return mpstSplashObject;}
                void            ClearSplashObject()     {mpstSplashObject = orxNULL;}

                void            LoadMenu();

                void            SetSoundSurface(VoxSoundSurface *_poSoundSurface)     {mpoSoundSurface = _poSoundSurface;}
                VoxSoundSurface *GetSoundSurface() const                              {return mpoSoundSurface;}

                void            SetOctaveSurface(VoxOctaveSurface *_poOctaveSurface)  {mpoOctaveSurface = _poOctaveSurface;}
                VoxOctaveSurface *GetOctaveSurface() const                            {return mpoOctaveSurface;}


private:

                orxSTATUS       Start();

                void            InitUI();
                void            InitSplash();
                void            InitScene();

                void            ExitUI();
                void            ExitScene();

                void            UpdateGame(const orxCLOCK_INFO &_rstInfo);
                void            UpdateInput(const orxCLOCK_INFO &_rstInfo);
                void            UpdateUI(const orxCLOCK_INFO &_rstInfo);

                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();

                GameState       meGameState;
                orxOBJECT *     mpstCameraObject;
                orxOBJECT *     mpstSplashObject;
                orxOBJECT *     mpstPauseObject;
                VoxSoundSurface *mpoSoundSurface;
                VoxOctaveSurface *mpoOctaveSurface;
                ScrollObject *  mpoScene;
                orxFLOAT        mfTime;
};

#endif // __VOX_H_
