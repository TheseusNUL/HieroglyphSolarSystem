#pragma once

#include "Application.h"

#include "Win32RenderWindow.h"
#include "RendererDX11.h"

#include "ViewPerspective.h"
//Hieroglyph Includes
#include "Camera.h"
#include "Scene.h"
#include "GeometryActor.h"
#include "PointLight.h"
#include "TextActor.h"

//STL Includes
#include <vector>

//LJMU Framework Includes
#include "LJMUTextOverlay.h"


#include "SkyboxActor.h"

using namespace Glyph3;

namespace LJMUDX
{
	//////////////////////////////////////
	//LJMULevelDemo.H
	//Class Application for a DirectX 11
	//Driven Application using the DirectX Toolkit
	//Hieroglyph 3 Rendering Engine and LUA.
	//
	//
	//AUTHORS:  DR Po Yang
	//			DR CHRIS CARTER
	//////////////////////////////////////
	class LJMULevelDemo : public Application //Inherit from the Hieroglyph Base Class
	{

	public:
		//------------CONSTRUCTORS------------------------------------------------
		LJMULevelDemo();	//Standard Empty Constructor which builds the object

	public:
		//------------INHERITED METHODS-------------------------------------------
		virtual void Initialize();					//Initialise the DirectX11 Scene
		virtual void Update();						//Update the DirectX Scene
		virtual void Shutdown();					//Shutdown the DirectX11 Scene

		virtual bool ConfigureEngineComponents();	//Initialise Hieroglyph and DirectX TK Modules
		virtual void ShutdownEngineComponents();	//Destroy Hieroglyph and DirectX TK Modules

		virtual void TakeScreenShot();				//Allow a screenshot to be generated

		virtual bool HandleEvent(EventPtr pEvent);	//Handle an I/O Event
		virtual std::wstring GetName();				//Get the Name of this App Instance

		//------------CUSTOM METHODS-----------------------------------------------
		void inputAssemblyStage();					//Stage to setup our VB and IB Info
		
		std::wstring outputFPSInfo();				//Convert the timer's Frames Per Second to a formatted string
		std::wstring OutputDebugInfo(Actor* actor);		
		std::wstring OutputDebugMatrix(const GeometryActor& actor);

		GeometryActor* LoadPlanet(const std::wstring& filename, float radius, Node3D* parent, const std::wstring& name);
		Node3D* CreateTiltNode(Vector3f angle, Vector3f position);

	    protected:
		//-------------CLASS MEMBERS-----------------------------------------------
		RendererDX11*			m_pRenderer11;	//Pointer to our DirectX 11 Device
		Win32RenderWindow*		m_pWindow;		//Pointer to our Windows-Based Window

		int						m_swapIndex;		//Index of our Swap Chain 
		ResourcePtr				m_targetRender;		//Pointer to the GPU Render Target for Colour
		ResourcePtr				m_targetDepth;			//Pointer to the GPU Render Target for Depth

		//--------------HIEROGLYPH OBJECTS-----------------------------------------
		ViewPerspective*		m_renderView;	//3D Output View - DirectX 11 Accelerated
		LJMUTextOverlay*        m_2DRenderText;	//2D Output View - DirectX 11 Accelerated
		Camera*					m_objCamera;	//Camera Object
		unsigned int			m_index;

		std::vector<TextActor*> m_textElements; //3D text

		SkyboxActor*			m_pSkyBox;

		std::vector<GeometryActor*>	m_objects; //List of objects within scene
		std::vector<std::wstring> m_planetNames;

		Node3D*					m_pPlanetNode; //Node that all the planets rotate around
		Node3D*					m_pTiltNode; //Node to tilt planets on angle
		Node3D*					m_pMoonNode; //Node for moon rotation around earth
	};

}
/*


*/