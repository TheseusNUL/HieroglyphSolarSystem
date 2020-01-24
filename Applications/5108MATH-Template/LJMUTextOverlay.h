#pragma once

#include "Task.h"
#include "SpriteFontLoaderDX11.h"
#include "SpriteRendererDX11.h"

namespace LJMUDX
{
	class Entity3D;

	using namespace Glyph3;

	/////////////////////////
	// Simple Structure to
	// represent a Text Entry
	// and its properties.
	/////////////////////////
	struct LJMUTextEntry
	{
		std::wstring	text;
		Matrix4f		xform;
		Vector4f		colour;

		LJMUTextEntry() {};
		LJMUTextEntry(std::wstring& ptext, Matrix4f& pxform, Vector4f& pcolour)
		{
			this->text = ptext;
			this->xform = pxform;
			this->colour = pcolour;
		};
	};

	/////////////////////////////////////////
	// LJMUTEXTOVERLAY.H
	// Class to simplify drawing of text elements
	// using SpriteFont instances.
	//
	// AUTHORS:  DR Po Yang
	//			 DR CHRIS CARTER
	/////////////////////////////////////////
	class LJMUTextOverlay : public Task
	{
	public:
		//--------CONSTRUCTORS/DESTRUCTORS----------------------------------------------------
		LJMUTextOverlay(RendererDX11& Renderer, ResourcePtr RenderTarget,std::wstring pfont,int psize);
		virtual ~LJMUTextOverlay();

		//-------TASK INTERFACE IMPLEMENTATION-----------------------------------------------
		virtual void Update(float ptime);
		virtual void QueuePreTasks(RendererDX11* pRenderer);
		virtual void ExecuteTask(PipelineManagerDX11* pPipelineManager, IParameterManager* pParamManager);
		virtual void Resize(UINT pwidth, UINT pheight);
		virtual std::wstring GetName();
		virtual void SetRenderParams(IParameterManager* pParamManager);
		virtual void SetUsageParams(IParameterManager* pParamManager);

		//--------PUBLIC METHODS-------------------------------------------------------------
		void writeText(std::wstring& ptext, Matrix4f& pxform, Vector4f& pcolour);

		//--------CLASS MEMBERS--------------------------------------------------------------
	protected:
		Vector4f					m_colour;
		int							m_objViewport;
		ResourcePtr					m_objRenderTarget;
		SpriteFontPtr				m_objPtrFont;
		SpriteRendererDX11*			m_objSpriteBatch;
		std::vector<LJMUTextEntry>	m_listText;
	};
};