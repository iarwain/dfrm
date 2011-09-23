//! Includes
#define __SCROLL_IMPL__
#include "dfrm.h"
#undef __SCROLL_IMPL__


//! Constants
static  const orxSTRING szConfigSectionGame         = "Game";
static  const orxSTRING szConfigSectionMainViewport = "MainViewport";
static  const orxSTRING szConfigSectionSplash       = "Splash";

static  const orxSTRING szInputAction               = "Action";
static  const orxSTRING szInputQuit                 = "Quit";


//! Code
orxSTATUS orxFASTCALL EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Depending on event type
  switch(_pstEvent->eType)
  {
    case orxEVENT_TYPE_SHADER:
    {
      // Set param?
      if(_pstEvent->eID == orxSHADER_EVENT_SET_PARAM)
      {
        orxSHADER_EVENT_PAYLOAD *pstPayload;

        // Gets its payload
        pstPayload = (orxSHADER_EVENT_PAYLOAD *)_pstEvent->pstPayload;

        // Time?
        if(!orxString_Compare(pstPayload->zParamName, "Time"))
        {
          // Updates its value
          pstPayload->fValue = DFRM::GetInstance().GetTime();
        }
      }

      break;
    }

    case orxEVENT_TYPE_OBJECT:
    {
      // Deleted?
      if(_pstEvent->eID == orxOBJECT_EVENT_DELETE)
      {
        orxOBJECT *pstObject;

        // Gets game instance
        DFRM &roGame = DFRM::GetInstance();

        // Gets object
        pstObject = orxOBJECT(_pstEvent->hSender);

        // Is splash screen?
        if(pstObject == roGame.GetSplashObject())
        {
          // Clears splash pointer
          roGame.ClearSplashObject();

          // Loads menu
          roGame.LoadMenu();
        }
      }

      break;
    }

    default:
    {
      break;
    }
  }

  // Done!
  return eResult;
}

void DFRM::LoadMenu()
{
  //! TODO
  // Starts
  Start();
}

orxSTATUS DFRM::Start()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Inits variables
  mfTime = orxFLOAT_0;

  // Updates game state
  meGameState = GameStateRun;

  // Pops config section
  orxConfig_PopSection();

  // Done!
  return eResult;
}

void DFRM::InitSplash()
{
  // Not in editor mode?
  if(!IsEditorMode())
  {
    // Creates splash screen
    mpstSplashObject = orxObject_CreateFromConfig(szConfigSectionSplash);

    // Not created?
    if(!mpstSplashObject)
    {
      // Loads menu
      LoadMenu();
    }
  }
  else
  {
    // Loads menu
    LoadMenu();
  }
}

void DFRM::Update(const orxCLOCK_INFO &_rstInfo)
{
  // Updates in game time
  mfTime += _rstInfo.fDT;

  // Depending on game state
  switch(meGameState)
  {
    case GameStateSplash:
    {
      // Action?
      if(orxInput_IsActive(szInputAction))
      {
        // Deletes splash screen
        orxObject_Delete(mpstSplashObject);
      }

      break;
    }

    case GameStateMenu:
    {
      break;
    }

    case GameStateRun:
    {
      break;
    }

    case GameStatePause:
    {
      break;
    }

    case GameStateEnd:
    {
      break;
    }
  }
}

orxSTATUS DFRM::Init()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Creates viewport
  orxViewport_CreateFromConfig(szConfigSectionMainViewport);

  // Init values
  meGameState       = GameStateSplash;
  mpstSplashObject  = orxNULL;
  mfTime            = orxFLOAT_0;

  // Creates scene
  mpoScene = CreateObject("Scene");

  // Pops config section
  orxConfig_PopSection();

  // Adds event handler
  orxEvent_AddHandler(orxEVENT_TYPE_OBJECT, EventHandler);
  orxEvent_AddHandler(orxEVENT_TYPE_SHADER, EventHandler);

  // Inits splash
  InitSplash();

  // Done!
  return eResult;
}

orxSTATUS DFRM::Run()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Quitting?
  if(orxInput_IsActive(szInputQuit))
  {
    // Updates result
    eResult = orxSTATUS_FAILURE;
  }

  // Done!
  return eResult;
}

void DFRM::Exit()
{
  // Removes event handler
  orxEvent_RemoveHandler(orxEVENT_TYPE_SHADER, EventHandler);
  orxEvent_RemoveHandler(orxEVENT_TYPE_OBJECT, EventHandler);

  // Deletes scene
  DeleteObject(mpoScene);
}

void DFRM::BindObjects()
{
}

int main(int argc, char **argv)
{
  // Executes game
  DFRM::GetInstance().Execute(argc, argv);

  // Done!
  return EXIT_SUCCESS;
}

#ifdef __orxWINDOWS__

#include "windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  // Executes game
  DFRM::GetInstance().Execute();

  // Done!
  return EXIT_SUCCESS;
}

#endif // __orxWINDOWS__
