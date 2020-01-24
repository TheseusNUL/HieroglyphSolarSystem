#pragma once

#include "Application.h"

#include "ViewPerspective.h"
//Hieroglyph Includes
#include "Scene.h"
#include "PointLight.h"

#include <array>

//LJMU Framework Includes
//#include "LJMUTextOverlay.h"
#include "LJMUTwoDOverlay.h"
#include "LJMUMateriaParams.h"
#include "LJMUH3Util.h"

#include "SkyboxActor.h"
#include "../Custom Files/WorldObject.h"
#include "../Custom Files/Planet.h"
#include "../Custom Files/OverlayWrapper.h"

#include "Win32RenderWindow.h"

using namespace Glyph3;

typedef std::shared_ptr<Glyph3::DrawExecutorDX11<Glyph3::BasicVertexDX11::Vertex>> BasicMeshPtr;

struct LJMUDefaultResources
{
	ResourcePtr		DefaultTexture;
	SpriteFontPtr	DefaultFont;
};

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
		void InputAssemblyStage();					//Stage to setup our VB and IB Info

		void SetupAllInitialState();
		void SetupInitialStates(GeometryActor* actor, Vector3f position);

		void LoadSharedResource(); //Setup content manager

		void CreatecCelestialObjects();
		void InitialiseCelestialObject(const std::wstring& filename, float radius, Node3D* parent, Vector3f tiltAngle, Vector3f tiltPos, const std::wstring& name);
		void CreateAllObjs();
		void CreateObj(std::wstring obj, std::wstring texture, Vector3f position, Vector3f scale, Actor* parent, const std::wstring& name);
		void CreateAsteroid(std::wstring obj, std::wstring texture, Vector3f position, Vector3f scale, Actor* parent);
		void CreateRotatingAsteroids();
		void CreateRepeatingAteroids();
		Actor* CreateParentActor(Vector3f position);

		void PushBackLerpVectors(Vector3f start);
		void AsterTransforms(float runtime);
		void RotatingAsterTransforms(float runtime);
		void RepeatingAsterTransforms();
		void MessengerTransform(float timerperframe);

		//Toggle UI functions
		void ToggleShading();
		void TogglePlanetTransforms(Matrix3f rotation, float runtime);
		void ToggleShipTransform(float runtime);

		template <typename TValue>
		TValue TrigonometricInterpolation(const TValue& start, const TValue& end, float t);

	    protected:
		//-------------CLASS MEMBERS-----------------------------------------------
		RendererDX11*			m_pRenderer11;	//Pointer to our DirectX 11 Device
		Win32RenderWindow*		m_pWindow;		//Pointer to our Windows-Based Window

		int						m_swapIndex;		//Index of our Swap Chain 
		ResourcePtr				m_targetRender;		//Pointer to the GPU Render Target for Colour
		ResourcePtr				m_targetDepth;			//Pointer to the GPU Render Target for Depth

		//--------------HIEROGLYPH OBJECTS-----------------------------------------
		ViewPerspective*		m_renderView;	//3D Output View - DirectX 11 Accelerated
		Camera*					m_objCamera;	//Camera Object
		unsigned int			m_index;
		LJMUDefaultResources	m_defaultResources;
		SkyboxActor*			m_pSkyBox;
		//------------2D OVERLAY-------------------------------------------------
		OverlayWrapper			m_overlay;
		unsigned int			m_currentObj;

		//-----------OBJECT STATES------------------------------------------------
		bool	m_bLShadingOn;
		bool	m_bRotPlanets;
		bool	m_bSpinPlanets;
		bool	m_bRotMoon;
		bool	m_bTransShip;
		bool	m_bShipForward;
		bool	m_bUFOForward;

		float	m_asteroidTime;
		std::vector<Vector3f> m_lerpPos;
		int		m_lerpIndex;
		Vector3f m_lerpStart;
		Vector3f m_lerpEnd;

		float	m_trigFactor;
		Vector3f m_trigStart;
		Vector3f m_trigEnd;
				
		//----------OBJETS IN SCENE-----------------------------------------------
		std::vector<GeometryActor*>	m_objects; //List of objects within scene
		std::vector<Planet*> m_planets;
		std::vector<WorldObject*> m_worldObjects;
		std::vector<std::wstring> m_objNames;
		std::vector<Actor*> m_parents;
		std::vector<WorldObject*> m_asteroids;

		PointLight* m_sunlight;

		Planet* m_pSun;
		Node3D*	m_pMoonNode; //Node for moon rotation around earth
		Node3D*	m_pPlanetNode; //Node that all the planets rotate around
	};
}
