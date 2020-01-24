#include "LJMUFPSCamera.h"

#include "PCH.h"
#include "EventManager.h"
#include "EvtKeyDown.h"
#include "EvtKeyUp.h"
#include "EvtFrameStart.h"
#include "EvtMouseMove.h"
#include "EvtMouseLeave.h"
#include "ViewportDX11.h"

using namespace LJMUDX;

//--------------CONSTANT DEFINITION---------------------------------------------------------------------
namespace LJMUDX
{
	static const int MOUSE_INVALID = -99999;

	static const float	PI = 3.14159f;
	static const float	TWO_PI = PI * 2.0f;
	static const float	PI_OVER_TWO = PI / 2.0f;
}

//--------------CONSTRUCTORS/DESTRUCTORS----------------------------------------------------------------

///////////////////////////////////////////////
// Constructor for our Camera Class - Initalise
// the key states and register the events that 
// we are handling via this camera. 
///////////////////////////////////////////////
LJMUFPSCamera::LJMUFPSCamera(int phalfwidth, int phalfheight) :
_mouse_delta_x(0),
_mouse_delta_y(0),
_rot_x(0),
_rot_y(0),
_mouse_prev_x(MOUSE_INVALID),
_mouse_prev_y(MOUSE_INVALID)
{
	//Initialise our key states to false.
	for (UINT i = 0; i < FPS_KEY_COUNT; ++i)
	{
		this->_key_states[i] = false;
	}
	
	//Register for event handling of Keyboard and Mouse
	//Actions. 
	this->RequestEvent(Glyph3::SYSTEM_KEYBOARD_KEYUP);
	this->RequestEvent(Glyph3::SYSTEM_KEYBOARD_KEYDOWN);
	this->RequestEvent(Glyph3::SYSTEM_MOUSE_MOVE);
	this->RequestEvent(Glyph3::SYSTEM_MOUSE_LEAVE);
	this->RequestEvent(Glyph3::RENDER_FRAME_START);

	this->_centre_x = phalfwidth;
	this->_centre_y = phalfheight;
}

///////////////////////////////////////////////
// Destructor for our Camera Class. 
///////////////////////////////////////////////
LJMUFPSCamera::~LJMUFPSCamera()
{
	//Nothing to clean up. 
}

//----------------INPUT MAPPING------------------------------------------------------------------------------

///////////////////////////////////////////////
// Construct our Camera Mapping
// (Indices correspond to Enum Values)
///////////////////////////////////////////////
const UINT LJMUFPSCamera::KeyBindings[FPS_KEY_COUNT] =
{
	'W', 'S', 'A', 'D', 'Q', 'Z', VK_SPACE
};

//-----------------EVENT HANDLING INTERFACE------------------------------------------------------------------

///////////////////////////////////////////////
// Handle our Mouse, Keyboard and Frame Events.
///////////////////////////////////////////////
bool LJMUFPSCamera::HandleEvent(Glyph3::EventPtr pevt)
{
	using namespace Glyph3;
	//Get the Event Type
	eEVENT e = pevt->GetEventType();

	bool tmove = false;
	if (e == SYSTEM_KEYBOARD_KEYDOWN)
	{
		EvtKeyDownPtr tkeydown = std::static_pointer_cast<EvtKeyDown>(pevt);
		UINT tkey = tkeydown->GetCharacterCode();
		for (UINT i = 0; i < FPS_KEY_COUNT; ++i)
		{
			if (tkey == KeyBindings[i])
			{
				this->_key_states[i] = true;
				tmove = true;
			}
		}
	}
	else if (e == SYSTEM_KEYBOARD_KEYUP)
	{
		EvtKeyUpPtr tkeyup	= std::static_pointer_cast<EvtKeyUp>(pevt);
		UINT		tkey	= tkeyup->GetCharacterCode();
		for (UINT i = 0; i < FPS_KEY_COUNT; ++i)
		{
			if (tkey == KeyBindings[i])
			{
				this->_key_states[i] = false;
				tmove = true;
			}
		}
	}
	else if (e == SYSTEM_MOUSE_MOVE)
	{
		EvtMouseMovePtr tmm = std::static_pointer_cast<EvtMouseMove>(pevt);
				
		if (this->_mouse_prev_x == MOUSE_INVALID && this->_mouse_prev_y == MOUSE_INVALID)
		{
			SetCursorPos(this->_centre_x, this->_centre_y);
			this->_mouse_prev_x = this->_centre_x;
			this->_mouse_prev_y = this->_centre_y;
			this->_mouse_delta_x = 0;
			this->_mouse_delta_y = 0;
		}
		else if (this->_mouse_prev_x != MOUSE_INVALID && this->_mouse_prev_y != MOUSE_INVALID)
		{
			int tcurrx = tmm->GetX();
			int tcurry = tmm->GetY();
			this->_mouse_delta_x = tcurrx - this->_mouse_prev_x;
			this->_mouse_delta_y = tcurry - this->_mouse_prev_y;
			this->_mouse_prev_x = tcurrx;
			this->_mouse_prev_y = tcurry;
			//SetCursorPos(this->_centre_x, this->_centre_y);		
			tmove = true;
		}
	}
	else if (e == SYSTEM_MOUSE_LEAVE)
	{
		this->_mouse_prev_x = MOUSE_INVALID;
		this->_mouse_prev_y = MOUSE_INVALID;
	}
	else if (e == RENDER_FRAME_START)
	{
		this->update();
	}
	return tmove;
}

//-----------------CAMERA HELPERS--------------------------------------------------------------------------

///////////////////////////////////////////////
// Update the State of the Camera
///////////////////////////////////////////////
void LJMUFPSCamera::update()
{
	this->_timer.Update();
	float ttpf = this->_timer.Elapsed();

	float tmovespeed		= 100.0f * ttpf;
	const float trotspeed	= PI * ttpf;

    if (this->_key_states[FPS_MOD])
        tmovespeed *= 3.0f;

    if (this->_key_states[FPS_RIGHT])
        this->Spatial().MoveRight(tmovespeed);
    if (this->_key_states[FPS_LEFT])
		this->Spatial().MoveLeft(tmovespeed);
    if (this->_key_states[FPS_UP])
		this->Spatial().MoveUp(tmovespeed);
    if (this->_key_states[FPS_DOWN])
		this->Spatial().MoveDown(tmovespeed);
    if (this->_key_states[FPS_FWD])
		this->Spatial().MoveForward(tmovespeed);
	if (this->_key_states[FPS_BACK])
		this->Spatial().MoveBackward(tmovespeed);
		
	this->_rot_x += this->_mouse_delta_y * trotspeed;
	this->_rot_y += this->_mouse_delta_x * trotspeed;
	
	if (this->_rot_x < -PI_OVER_TWO)
		this->_rot_x = -PI_OVER_TWO;
	else if (this->_rot_x > PI_OVER_TWO)
		this->_rot_x = PI_OVER_TWO;
	this->_rot_y = this->wrapAngle(this->_rot_y);
		
	this->Spatial().RotateBy(Glyph3::Vector3f(this->_rot_x, this->_rot_y, 0.0f));

	this->_mouse_delta_x = 0;
	this->_mouse_delta_y = 0;

	this->_rot_x = 0.0f;
	this->_rot_y = 0.0f;
}

///////////////////////////////////////////////
// Convert Angle to Range -PI to PI
///////////////////////////////////////////////
float LJMUFPSCamera::wrapAngle(float pangle)
{
	float twrap;
	pangle = pangle + PI;
	twrap = fabsf(pangle);
	twrap = twrap - (TWO_PI * (float)((int)(twrap / TWO_PI)));
	twrap = twrap - PI;
	if (pangle < 0.0f)
		twrap = -twrap;
	return twrap;
}

//-------------COMPONENT NAME IMPLEMENTATION----------------------------------------------------------------

///////////////////////////////////////////////
// Get the Name of this Camera Object
///////////////////////////////////////////////
std::wstring LJMUFPSCamera::GetName()
{
	return L"LJMUFPSCamera";
}
