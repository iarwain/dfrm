//! Includes
#define __SCROLL_IMPL__
#include "Vox.h"
#undef __SCROLL_IMPL__


//! Constants
static  const orxSTRING szConfigSectionGame         = "Game";
static  const orxSTRING szConfigSectionMainViewport = "MainViewport";
static  const orxSTRING szConfigSectionSave         = "Save";
static  const orxSTRING szConfigSectionSplash       = "Splash";

static  const orxSTRING szInputAction               = "Action";
static  const orxSTRING szInputQuit                 = "Quit";


//! Code
static orxBOOL orxFASTCALL SaveCallback(const orxSTRING _zSectionName, const orxSTRING _zKeyName, const orxSTRING _zFileName, orxBOOL _bUseEncryption)
{
  // Done!
  return (!orxString_Compare(_zSectionName, szConfigSectionSave));
}

orxSTATUS orxFASTCALL EventHandler(const orxEVENT *_pstEvent)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Depending on event type
  switch(_pstEvent->eType)
  {
    case orxEVENT_TYPE_OBJECT:
    {
      // Deleted?
      if(_pstEvent->eID == orxOBJECT_EVENT_DELETE)
      {
        orxOBJECT *pstObject;

        // Gets game instance
        Vox &roGame = Vox::GetInstance();

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

void Vox::LoadMenu()
{
  //! TODO
  // Starts
  Start();
}

orxSTATUS Vox::Start()
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

void Vox::InitUI()
{
  //! TODO
}

void Vox::InitSplash()
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

void Vox::InitScene()
{
  orxVECTOR vPosition;

  // Creates scene
  mpoScene = CreateObject("Scene");

  // Updates listener position
  orxConfig_GetVector("ListenerPosition", &vPosition);
  orxSoundSystem_SetListenerPosition(&vPosition);
}

void Vox::ExitUI()
{
}

void Vox::ExitScene()
{
  // Deletes scene
  DeleteObject(mpoScene);
}

void Vox::UpdateGame(const orxCLOCK_INFO &_rstInfo)
{
  // Updates in game time
  mfTime += _rstInfo.fDT;

  // Updates input
  UpdateInput(_rstInfo);

  // Updates UI
  UpdateUI(_rstInfo);
}

void Vox::UpdateInput(const orxCLOCK_INFO &_rstInfo)
{
  orxVECTOR vMousePos;

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Gets world mouse position
  if(orxRender_GetWorldPosition(orxMouse_GetPosition(&vMousePos), &vMousePos) != orxNULL)
  {
    VoxObject *apoObjectList[] = {mpoOctaveSurface, mpoSoundSurface};

    // Updates pixking depth
    vMousePos.fZ = orxFLOAT_0;

    // For all surfaces
    for(orxS32 i = 0, iCounter = sizeof(apoObjectList) / sizeof(VoxObject *); i < iCounter; i++)
    {
      // Active or new status?
      if(orxInput_IsActive(szInputAction) || orxInput_HasNewStatus(szInputAction))
      {
        // Relays the input
        apoObjectList[i]->SetInput(vMousePos, orxInput_IsActive(szInputAction));
      }
      // Double action?
      else if(orxInput_IsActive("DoubleAction") || orxInput_HasNewStatus("DoubleAction"))
      {
        // Is octave surface?
        if(apoObjectList[i] == mpoOctaveSurface)
        {
          // Relays the special input
          apoObjectList[i]->SetInput(vMousePos, orxInput_IsActive("DoubleAction"), orxTRUE);
        }
        else
        {
          // Relays the double input
          vMousePos.fY = orxMAX(orxFLOAT_0, vMousePos.fY - orx2F(75.0f));
          vMousePos.fX = orxMAX(orx2F(112.0f), vMousePos.fX - orx2F(75.0f));
          apoObjectList[i]->SetInput(vMousePos, orxInput_IsActive("DoubleAction"));
          vMousePos.fY = orxMIN(orx2F(768.0f), vMousePos.fY + orx2F(75.0f));
          vMousePos.fX = orxMIN(orx2F(1024.0f), vMousePos.fX + orx2F(75.0f));
          apoObjectList[i]->SetInput(vMousePos, orxInput_IsActive("DoubleAction"));
        }
      }
    }

    //! TODO
  }

  // Pops config section
  orxConfig_PopSection();
}

void Vox::UpdateUI(const orxCLOCK_INFO &_rstInfo)
{
  //! TODO
}

void Vox::Update(const orxCLOCK_INFO &_rstInfo)
{
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
      // Updates in-game
      UpdateGame(_rstInfo);

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

orxSTATUS Vox::Init()
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Creates viewport
  orxViewport_CreateFromConfig(szConfigSectionMainViewport);

  // Init values
  meGameState       = GameStateSplash;
  mpstCameraObject  = orxNULL;
  mpstSplashObject  = orxNULL;
  mpstPauseObject   = orxNULL;
  mpoScene          = orxNULL;
  mfTime            = orxFLOAT_0;

  // Inits splash screen
  InitSplash();

  // Inits UI
  InitUI();

  // Inits scene
  InitScene();

  // Pops config section
  orxConfig_PopSection();

  // Adds event handler
  orxEvent_AddHandler(orxEVENT_TYPE_OBJECT, EventHandler);

  // Done!
  return eResult;
}

orxSTATUS Vox::Run()
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

void Vox::Exit()
{
  // Removes event handler
  orxEvent_RemoveHandler(orxEVENT_TYPE_OBJECT, EventHandler);

  // Exits from scene
  ExitScene();

  // Exits from UI
  ExitUI();
}

void Vox::BindObjects()
{
  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Binds objects
  ScrollBindObject<VoxCursor>("Cursor");
  ScrollBindObject<VoxOctaveSurface>("OctaveSurface");
  ScrollBindObject<VoxSoundSurface>("SoundSurface");

  // Pops section
  orxConfig_PopSection();
}

int main(int argc, char **argv)
{
  // Executes game
  Vox::GetInstance().Execute(argc, argv);

  // Done!
  return EXIT_SUCCESS;
}

#ifdef __orxWINDOWS__

#include "windows.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  // Executes game
  Vox::GetInstance().Execute();

  // Done!
  return EXIT_SUCCESS;
}

#endif // __orxWINDOWS__
