#ifndef __DFRM_H_
#define __DFRM_H_

//! Includes
#define __NO_SCROLLED__ // Uncomment this define to prevent the embedded editor (ScrollEd) from being compiled
#include "Scroll.h"

//! DFRM class
class DFRM : public Scroll<DFRM>
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

                orxFLOAT        GetTime() const           {return mfTime;}
          const orxVECTOR &     GetPosition() const       {return mvPos;}
          const orxVECTOR &     GetDirection() const      {return mvDir;}
                orxBOOL         Distort() const           {return mbDistort;}
                orxBOOL         Bump() const              {return mbBump;}
                orxBOOL         Repeat() const            {return mbRepeat;}
                void            SetFocus(orxBOOL _bFocus) {mbFocus = _bFocus;}
                GameState       GetGameState() const      {return meGameState;}
          const orxOBJECT *     GetSplashObject()         {return mpstSplashObject;}
                void            ClearSplashObject()       {mpstSplashObject = orxNULL;}

                void            LoadMenu();


private:

                orxSTATUS       Start();

                void            InitSplash();

                void            Update(const orxCLOCK_INFO &_rstInfo);

                orxSTATUS       Init();
                orxSTATUS       Run();
                void            Exit();
                void            BindObjects();

                orxVECTOR       mvPos, mvDir, mvSpeed;
                orxFLOAT        mfIntensity;
                orxBOOL         mbFocus, mbDistort, mbBump, mbRepeat;
                GameState       meGameState;
                orxOBJECT *     mpstSplashObject;
                ScrollObject *  mpoScene;
                orxFLOAT        mfTime;
};

#endif // __DFRM_H_
