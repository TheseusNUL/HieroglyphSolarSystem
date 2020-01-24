// #pragma once
#include "../LJMU Framework/LJMUTwoDOverlay.h"
#include "../LJMU Framework/LJMUH3Util.h"

#include "RendererDX11.h"

#include <string>

//Hieroglyph Includes
#include "Camera.h"
#include "TextActor.h"
#include "GeometryActor.h"
#include "ResourceProxyDX11.h"
#include "SpriteFontDX11.h"
#include "Win32RenderWindow.h"

struct MenuSystem
{
	int active;
	int itemcount;

	MenuSystem()
	{
		active = 0;
		itemcount = 0;
	}
	
	void increment()
	{
		active++;
		if (active >= itemcount)
			active = 0;
	}

	void decrement()
	{
		active--;
		if (active < 0)
			active = itemcount - 1;
	}
};

enum MenuItems
{
	E_SHADING_MODE,
	E_ROT_PLANETS,
	E_SPIN_PLANETS,
	E_ROT_MOON,
	E_TRANS_SHIP,
	E_RESET_CAM
};

class OverlayWrapper
{
public:
	void Initalise(std::vector<Glyph3::GeometryActor*> &objs, float timer, Glyph3::Win32RenderWindow* window, Glyph3::RendererDX11& renderer, std::vector<std::wstring> names);
	void InputAssembly2D(std::vector<Glyph3::GeometryActor*> &objs, float timer, Glyph3::Win32RenderWindow* window, std::vector<std::wstring> names);		//Stage to set up 2D sprites and text
	void Update2D(float timer, std::vector<Glyph3::GeometryActor*> &objs, std::vector<std::wstring> names); //Transform updates
	void Update3D(std::vector<Glyph3::GeometryActor*> &objs, Glyph3::Camera* camera3D);


	void LoadUIElements(Glyph3::RendererDX11& renderer);
	void LoadUIPanel();

	std::wstring OutputFPSInfo(float timer);				//Convert the timer's Frames Per Second to a formatted string
	std::wstring OutputDebugInfoAll(std::vector<Glyph3::GeometryActor*> objs, std::vector<std::wstring> names);
	std::wstring OutputDebugMatrix(const Glyph3::GeometryActor& actor);
	std::wstring OutputDebugTransform(const Glyph3::Matrix4f& matrix, Glyph3::Vector3f point);
	std::wstring OutputDebugTransform(const Glyph3::GeometryActor& actor, Glyph3::Vector3f point);
	std::wstring OutputMatrixText(std::vector<Glyph3::GeometryActor*> objs, std::vector<std::wstring> names);

	int SetCurrentObject(int number);
	int GetCurrentObject();

	Glyph3::Camera* SetUpCamera(Glyph3::RendererDX11& renderer, Glyph3::ResourcePtr targetRender);
	Glyph3::Camera*	GetCamera();

	LJMUDX::LJMUTwoDOverlay* GetOverlay();

	void IncrementPanel();
	void DecrementPanel();

	int GetActiveIndex();

private:
	Glyph3::Camera*						m_p2DCamera;
	LJMUDX::LJMUTwoDOverlay*			m_p2DRenderer;

	Glyph3::ResourcePtr					m_spriteText;
	Glyph3::SpriteFontPtr				m_spriteFont;

	LJMUDX::LJMUResourceHelper			m_localContent;
	std::vector<LJMUDX::LJMUSpriteEntry>m_sprites;
	std::vector<LJMUDX::LJMUTextEntry>	m_text;
	std::vector<Glyph3::TextActor*>		m_pTextElements; //3D text

	unsigned int						m_currentObj; //current object selected on 2d overlay

	std::vector<std::wstring>			m_menuItems;
	MenuSystem							m_menuSystem;
};