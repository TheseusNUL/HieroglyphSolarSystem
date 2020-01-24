#ifndef _LJMU_FPS_H_
#define _LJMU_FPS_H_

#include "Camera.h"
#include "Timer.h"

namespace LJMUDX
{
	////////////////////////////////
	// Camera Action Mapping
	////////////////////////////////
	enum LJMUFPSControls
	{
		FPS_FWD = 0,
		FPS_BACK,
		FPS_LEFT,
		FPS_RIGHT,
		FPS_UP,
		FPS_DOWN,
		FPS_MOD,
		FPS_KEY_COUNT
	};

	//////////////////////////////////
	// Class to represent our FPS
	// Camera.
	//////////////////////////////////
	class LJMUFPSCamera : public Glyph3::Camera
	{
		//--------CLASS CONSTANTS------------------------------------
		static const UINT KeyBindings[FPS_KEY_COUNT];

	public:
		//--------CONSTRUCTOR/DESTRUCTOR-----------------------------
		LJMUFPSCamera(int phalfwidth, int phalfheight);
		virtual ~LJMUFPSCamera();

		//--------INTERFACE IMPLEMENTATION---------------------------
		virtual bool HandleEvent(Glyph3::EventPtr pevt);
		virtual std::wstring GetName();

	protected:
		//--------HELPER METHODS-------------------------------------
		void update();
		float wrapAngle(float pangle);

	protected:
		//--------CLASS MEMBERS--------------------------------------
		bool			_key_states[FPS_KEY_COUNT];
		int				_mouse_prev_x;
		int				_mouse_prev_y;
		int				_mouse_delta_x;
		int				_mouse_delta_y;
		float			_rot_x;
		float			_rot_y;
		int				_centre_x;
		int				_centre_y;
		Glyph3::Timer	_timer;
	};
};
#endif