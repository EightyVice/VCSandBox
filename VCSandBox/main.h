#include "plugin.h"
using namespace plugin;

#include "vendor/detours/detours.h"
#include "hooklibrary.h"
using namespace Hook;

//psdk
#include "CVector.h"
#include "CEntity.h"
#include "CStreaming.h"
#include "CModelInfo.h"
#include "CHeli.h"
#include "CPlane.h"
#include "CBoat.h"
#include "CWorld.h"
#include "CTheScripts.h"
#include "CTimer.h"
#include "CPlayerPed.h"
#include "CFileLoader.h"
#include "CWorld.h"
#include "CColModel.h"
#include "CMenuManager.h"
#include "CPopulation.h"
#include "CCamera.h"
#include "CWeaponInfo.h"
#include "CPed.h"
#include "CCivilianPed.h"
#include "CBike.h"
#include "CPathFind.h"
#include "CCarCtrl.h"
#include "CTimer.h"
#include "CTheScripts.h"
#include "CHud.h"
#include "CPickups.h"
#include "ePickupType.h"
#include "CObject.h"
#include "common.h"
#include "extensions/ScriptCommands.h"


//sandbox
#include "CEntityManager.h"
#include "CModelManager.h"
#include "CVehicleManager.h"
#include "CPedManager.h"
#include "CPlayerPedManager.h"
#include "CPopulationManager.h"
#include "CGamePatches.h"
#include "CWeaponManager.h"
#include "CRadarManager.h"
#include "CPickupManager.h"
#include "CObjectManager.h"

//externals
extern CModelManager		*gModelManager;
extern CVehicleManager		*gVehicleManager;
extern CPedManager			*gPedManager;
extern CPlayerPedManager	*gPlayerPedManager;
extern CPopulationManager	*gPopulationManager;
extern CRadarManager		*gRadarManager;
extern CPickupManager		*gPickupManager;
extern CObjectManager		*gObjectManager;

// that enum is from SA maybe right

enum PLUGIN_API eCamMode : unsigned short {
	MODE_NONE = 0x0,
	MODE_TOPDOWN = 0x1,
	MODE_GTACLASSIC = 0x2,
	MODE_BEHINDCAR = 0x3,
	MODE_FOLLOWPED = 0x4,
	MODE_AIMING = 0x5,
	MODE_DEBUG = 0x6,
	MODE_SNIPER = 0x7,
	MODE_ROCKETLAUNCHER = 0x8,
	MODE_MODELVIEW = 0x9,
	MODE_BILL = 0xA,
	MODE_SYPHON = 0xB,
	MODE_CIRCLE = 0xC,
	MODE_CHEESYZOOM = 0xD,
	MODE_WHEELCAM = 0xE,
	MODE_FIXED = 0xF,
	MODE_1STPERSON = 0x10,
	MODE_FLYBY = 0x11,
	MODE_CAM_ON_A_STRING = 0x12,
	MODE_REACTION = 0x13,
	MODE_FOLLOW_PED_WITH_BIND = 0x14,
	MODE_CHRIS = 0x15,
	MODE_BEHINDBOAT = 0x16,
	MODE_PLAYER_FALLEN_WATER = 0x17,
	MODE_CAM_ON_TRAIN_ROOF = 0x18,
	MODE_CAM_RUNNING_SIDE_TRAIN = 0x19,
	MODE_BLOOD_ON_THE_TRACKS = 0x1A,
	MODE_IM_THE_PASSENGER_WOOWOO = 0x1B,
	MODE_SYPHON_CRIM_IN_FRONT = 0x1C,
	MODE_PED_DEAD_BABY = 0x1D,
	MODE_PILLOWS_PAPS = 0x1E,
	MODE_LOOK_AT_CARS = 0x1F,
	MODE_ARRESTCAM_ONE = 0x20,
	MODE_ARRESTCAM_TWO = 0x21,
	MODE_M16_1STPERSON = 0x22,
	MODE_SPECIAL_FIXED_FOR_SYPHON = 0x23,
	MODE_FIGHT_CAM = 0x24,
	MODE_TOP_DOWN_PED = 0x25,
	MODE_LIGHTHOUSE = 0x26,
	MODE_SNIPER_RUNABOUT = 0x27,
	MODE_ROCKETLAUNCHER_RUNABOUT = 0x28,
	MODE_1STPERSON_RUNABOUT = 0x29,
	MODE_M16_1STPERSON_RUNABOUT = 0x2A,
	MODE_FIGHT_CAM_RUNABOUT = 0x2B,
	MODE_EDITOR = 0x2C,
	MODE_HELICANNON_1STPERSON = 0x2D,
	MODE_CAMERA = 0x2E,
	MODE_ATTACHCAM = 0x2F,
	MODE_TWOPLAYER = 0x30,
	MODE_TWOPLAYER_IN_CAR_AND_SHOOTING = 0x31,
	MODE_TWOPLAYER_SEPARATE_CARS = 0x32,
	MODE_ROCKETLAUNCHER_HS = 0x33,
	MODE_ROCKETLAUNCHER_RUNABOUT_HS = 0x34,
	MODE_AIMWEAPON = 0x35,
	MODE_TWOPLAYER_SEPARATE_CARS_TOPDOWN = 0x36,
	MODE_AIMWEAPON_FROMCAR = 0x37,
	MODE_DW_HELI_CHASE = 0x38,
	MODE_DW_CAM_MAN = 0x39,
	MODE_DW_BIRDY = 0x3A,
	MODE_DW_PLANE_SPOTTER = 0x3B,
	MODE_DW_DOG_FIGHT = 0x3C,
	MODE_DW_FISH = 0x3D,
	MODE_DW_PLANECAM1 = 0x3E,
	MODE_DW_PLANECAM2 = 0x3F,
	MODE_DW_PLANECAM3 = 0x40,
	MODE_AIMWEAPON_ATTACHED = 0x41
};
typedef struct _CAMERA_AIM
{
	CVector LookFront;
	CVector Source;
	CVector SourceBeforeLookBehind;
	CVector LookUp;

} CAMERA_AIM;

// Old sources
#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))
#define SET_BIT(var,pos) ((var) |= (1<<(pos)))
#define CLEAR_BIT(var,pos) ((var) &= ~(1<<(pos)))

#define SAFE_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = 0; } }
#define SAFE_DELETE(a) if( (a) != NULL ) delete (a); (a) = NULL;
#define SAFE_FREE(a) if( (a) != NULL ) free (a); (a) = NULL;
#define ARRAY_SIZE(a) (sizeof((a))/sizeof(*(a)))

#define INTEGER_AT(addr) *(int *)(addr)
#define FLOAT_AT(addr) *(float *)(addr)

#define RAD2DEG 57.2957763671875
#define SCREEN_WIDTH_(a) (((float)RsGlobal.maximumWidth / 640.0f)*a)
#define SCREEN_HEIGHT_(a) (((float)RsGlobal.maximumHeight / 448.0f)*a)
#define SCREEN(a) (((float)RsGlobal.maximumHeight / 900.0f)*a)
