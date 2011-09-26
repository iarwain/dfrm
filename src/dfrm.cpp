//! Includes
#define __SCROLL_IMPL__
#include "dfrm.h"
#undef __SCROLL_IMPL__


//! Constants
static  const orxSTRING szConfigSectionGame         = "Game";
static  const orxSTRING szConfigSectionShader       = "Shader";
static  const orxSTRING szConfigSectionMainViewport = "MainViewport";
static  const orxSTRING szConfigSectionSplash       = "Splash";

static  const orxSTRING szInputAction               = "Action";
static  const orxSTRING szInputQuit                 = "Quit";

static  const orxSTRING szInputLeft                 = "Left";
static  const orxSTRING szInputRight                = "Right";
static  const orxSTRING szInputForward              = "Forward";
static  const orxSTRING szInputBackward             = "Backward";

static  const orxSTRING szInputHeading              = "Heading";
static  const orxSTRING szInputPitch                = "Pitch";

static  const orxSTRING szInputDistort              = "Distort";
static  const orxSTRING szInputBump                 = "Bump";
static  const orxSTRING szInputRepeat               = "Repeat";


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
        if(!orxString_Compare(pstPayload->zParamName, "time"))
        {
          // Updates its value
          pstPayload->fValue = DFRM::GetInstance().GetTime();
        }
        // Position?
        else if(!orxString_Compare(pstPayload->zParamName, "position"))
        {
          // Updates its value
          orxVector_Copy(&pstPayload->vValue, &DFRM::GetInstance().GetPosition());
        }
        // Heading?
        else if(!orxString_Compare(pstPayload->zParamName, "heading"))
        {
          orxVECTOR vTemp;

          // Gets direction
          vTemp.fX = DFRM::GetInstance().GetDirection().fZ;
          vTemp.fY = DFRM::GetInstance().GetDirection().fX;
          vTemp.fZ = DFRM::GetInstance().GetDirection().fY;
          orxVector_FromCartesianToSpherical(&vTemp, &vTemp);

          // Updates its value
          pstPayload->fValue = vTemp.fTheta;
        }
        // Pitch?
        else if(!orxString_Compare(pstPayload->zParamName, "pitch"))
        {
          orxVECTOR vTemp;

          // Gets direction
          vTemp.fX = DFRM::GetInstance().GetDirection().fZ;
          vTemp.fY = DFRM::GetInstance().GetDirection().fX;
          vTemp.fZ = DFRM::GetInstance().GetDirection().fY;
          orxVector_FromCartesianToSpherical(&vTemp, &vTemp);

          // Updates its value
          pstPayload->fValue = vTemp.fPhi - orxMATH_KF_PI_BY_2;
        }
        // Distort?
        else if(!orxString_Compare(pstPayload->zParamName, "distort"))
        {
          // Updates its value
          pstPayload->fValue = DFRM::GetInstance().Distort() ? orxFLOAT_1 : orxFLOAT_0;
        }
        // Bump?
        else if(!orxString_Compare(pstPayload->zParamName, "bump"))
        {
          // Updates its value
          pstPayload->fValue = DFRM::GetInstance().Bump() ? orxFLOAT_1 : orxFLOAT_0;
        }
        // Repeat?
        else if(!orxString_Compare(pstPayload->zParamName, "repeat"))
        {
          // Updates its value
          pstPayload->fValue = DFRM::GetInstance().Repeat() ? orxFLOAT_1 : orxFLOAT_0;
        }
      }

      break;
    }

    case orxEVENT_TYPE_SYSTEM:
    {
      // Focus gained?
      if(_pstEvent->eID == orxSYSTEM_EVENT_FOCUS_GAINED)
      {
        DFRM::GetInstance().SetFocus(orxTRUE);
      }
      // Focus lost?
      else if(_pstEvent->eID == orxSYSTEM_EVENT_FOCUS_LOST)
      {
        DFRM::GetInstance().SetFocus(orxFALSE);
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
      orxVECTOR vTemp;
      static orxBOOL sbFirstTime = orxTRUE;

      if(orxInput_IsActive(szInputLeft))
      {
        orxVector_Set(&vTemp, -mvDir.fZ, orxFLOAT_0, mvDir.fX);
        orxVector_Add(&mvPos, &mvPos, orxVector_Mulf(&vTemp, &vTemp, mvSpeed.fX * _rstInfo.fDT));
      }
      if(orxInput_IsActive(szInputRight))
      {
        orxVector_Set(&vTemp, mvDir.fZ, orxFLOAT_0, -mvDir.fX);
        orxVector_Add(&mvPos, &mvPos, orxVector_Mulf(&vTemp, &vTemp, mvSpeed.fX * _rstInfo.fDT));
      }
      if(orxInput_IsActive(szInputForward))
      {
        orxVector_Copy(&vTemp, &mvDir);
        orxVector_Add(&mvPos, &mvPos, orxVector_Mulf(&vTemp, &vTemp, mvSpeed.fY * _rstInfo.fDT));
      }
      if(orxInput_IsActive(szInputBackward))
      {
        orxVector_Neg(&vTemp, &mvDir);
        orxVector_Add(&mvPos, &mvPos, orxVector_Mulf(&vTemp, &vTemp, mvSpeed.fY * _rstInfo.fDT));
      }
      if(!sbFirstTime)
      {
        if(orxInput_IsActive(szInputHeading))
        {
          vTemp.fX = mvDir.fZ;
          vTemp.fY = mvDir.fX;
          vTemp.fZ = mvDir.fY;
          orxVector_FromCartesianToSpherical(&vTemp, &vTemp);
          vTemp.fTheta += mfIntensity * orxMATH_KF_DEG_TO_RAD * orxInput_GetValue(szInputHeading);
          orxVector_FromSphericalToCartesian(&vTemp, &vTemp);
          mvDir.fX = vTemp.fY;
          mvDir.fY = vTemp.fZ;
          mvDir.fZ = vTemp.fX;
        }
        if(orxInput_IsActive(szInputPitch))
        {
          vTemp.fX = mvDir.fZ;
          vTemp.fY = mvDir.fX;
          vTemp.fZ = mvDir.fY;
          orxVector_FromCartesianToSpherical(&vTemp, &vTemp);
          vTemp.fPhi += mfIntensity * -orxMATH_KF_DEG_TO_RAD * orxInput_GetValue(szInputPitch);
          vTemp.fPhi = orxMAX(orx2F(0.01f), orxMIN(vTemp.fPhi, orxMATH_KF_PI - orx2F(0.01f)));
          orxVector_FromSphericalToCartesian(&vTemp, &vTemp);
          mvDir.fX = vTemp.fY;
          mvDir.fY = vTemp.fZ;
          mvDir.fZ = vTemp.fX;
        }
      }
      else
      {
        sbFirstTime = orxFALSE;
      }

      if(orxInput_IsActive(szInputDistort) && orxInput_HasNewStatus(szInputDistort))
      {
        mbDistort = !mbDistort;
      }
      if(orxInput_IsActive(szInputBump) && orxInput_HasNewStatus(szInputBump))
      {
        mbBump = !mbBump;
      }
      if(orxInput_IsActive(szInputRepeat) && orxInput_HasNewStatus(szInputRepeat))
      {
        mbRepeat = !mbRepeat;
      }

      // Captures the mouse cursor
      if(mbFocus)
      {
        orxDisplay_GetScreenSize(&vTemp.fX, &vTemp.fY);
        orxMouse_SetPosition(orxVector_Mulf(&vTemp, &vTemp, orx2F(0.5f)));
      }

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
  orxSTRING zCode;
  orxS32    s32FileSize;
  orxFILE  *pstFile;

  // Pushes game section
  orxConfig_PushSection(szConfigSectionGame);

  // Init values
  orxConfig_GetVector("Position", &mvPos);
  orxConfig_GetVector("Direction", &mvDir);
  meGameState       = GameStateSplash;
  mpstSplashObject  = orxNULL;
  mfTime            = orxFLOAT_0;
  mbFocus           = orxFALSE;
  mbDistort         = orxFALSE;
  mbBump            = orxFALSE;
  mbRepeat          = orxFALSE;
  mfIntensity       = orxConfig_GetFloat("Intensity");
  orxConfig_GetVector("Speed", &mvSpeed);

  // Loads config file
  pstFile     = orxFile_Open(orxConfig_GetString(szConfigSectionShader), orxFILE_KU32_FLAG_OPEN_READ);
  s32FileSize = orxFile_GetSize(pstFile);

  // Allocates code buffer
  zCode = (orxSTRING)orxMemory_Allocate((s32FileSize + 1) * sizeof(orxCHAR), orxMEMORY_TYPE_TEMP);

  // Gets its content
  zCode[orxFile_Read(zCode, sizeof(orxCHAR), s32FileSize, pstFile)] = orxCHAR_NULL;

  // Closes file
  orxFile_Close(pstFile);

  // Pops config section
  orxConfig_PopSection();

  // Pushes shader section
  orxConfig_PushSection(szConfigSectionShader);

  // Loads shader code
  orxConfig_SetString("Code", zCode);

  // Pops config section
  orxConfig_PopSection();

  // Frees code buffer
  orxMemory_Free(zCode);

  // Creates viewport
  orxViewport_CreateFromConfig(szConfigSectionMainViewport);

  // Creates scene
  mpoScene = CreateObject("Scene");

  // Adds event handler
  orxEvent_AddHandler(orxEVENT_TYPE_SYSTEM, EventHandler);
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
  orxEvent_RemoveHandler(orxEVENT_TYPE_SYSTEM, EventHandler);

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
