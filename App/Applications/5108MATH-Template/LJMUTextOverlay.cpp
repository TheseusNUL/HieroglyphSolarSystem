#include "LJMUTextOverlay.h"
#include "Entity3D.h"
#include "Node3D.h"
#include "Texture2dConfigDX11.h"
#include "Log.h"
#include "IParameterManager.h"
#include "PipelineManagerDX11.h"
#include "Texture2dDX11.h"

using namespace LJMUDX;
using namespace Glyph3;

///////////////////////////
// Constructor for our Text
// Overlay Class. 
///////////////////////////
LJMUTextOverlay::LJMUTextOverlay(RendererDX11& renderer, ResourcePtr rtt, std::wstring font, int size)
{
	m_objRenderTarget = rtt;
	ResourceDX11* ptr_res = renderer.GetResourceByIndex(m_objRenderTarget->m_iResource);

	if (ptr_res->GetType() == RT_TEXTURE2D)
	{
		Texture2dDX11* ttex = (Texture2dDX11*)ptr_res;
		D3D11_TEXTURE2D_DESC tdesc = ttex->GetActualDescription();

		D3D11_VIEWPORT viewport;
		viewport.Width = static_cast<float>(tdesc.Width);
		viewport.Height = static_cast<float>(tdesc.Height);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		m_objViewport = renderer.CreateViewPort(viewport);
	}

	//Create the Sprite Font and the Sprite Renderer (SpriteBatch) for our Text Elements
	m_objPtrFont = SpriteFontLoaderDX11::LoadFont(font,size, 0,false);	
	m_objSpriteBatch = new SpriteRendererDX11();
	m_objSpriteBatch->Initialize();
}

///////////////////////////
// Destructor For our Text
// Overlay Class
///////////////////////////
LJMUTextOverlay::~LJMUTextOverlay()
{
	SAFE_DELETE(m_objSpriteBatch);
}

///////////////////////////
// Queue the Render Tasks
///////////////////////////
void LJMUTextOverlay::QueuePreTasks(RendererDX11* renderer)
{
	renderer->QueueTask(this);
}

///////////////////////////
// Execute our Text Rendering
// Task
///////////////////////////
void LJMUTextOverlay::ExecuteTask(PipelineManagerDX11* pipelinemanager, IParameterManager* paramManager)
{
	//If we have any text elements to draw
	if (m_listText.size() > 0)
	{
		// Set the parameters for rendering this view
		pipelinemanager->ClearRenderTargets();
		pipelinemanager->OutputMergerStage.DesiredState.RenderTargetViews.SetState(0, m_objRenderTarget->m_iResourceRTV);
		pipelinemanager->ApplyRenderTargets();

		pipelinemanager->RasterizerStage.DesiredState.ViewportCount.SetState(1);
		pipelinemanager->RasterizerStage.DesiredState.Viewports.SetState(0,m_objViewport);
		pipelinemanager->RasterizerStage.DesiredState.RasterizerState.SetState(0);

		// Set default states for these stages
		pipelinemanager->OutputMergerStage.DesiredState.DepthStencilState.SetState(0);
		pipelinemanager->OutputMergerStage.DesiredState.BlendState.SetState(0);

		//Loop over each text element and use the spritebatch to draw them
		for (auto tentry : m_listText)
		{
			//Use the RenderText function of our Sprite Batch to draw the text
			m_objSpriteBatch->RenderText(pipelinemanager, paramManager,
		           m_objPtrFont, tentry.text.c_str(), tentry.xform, tentry.colour);
		}

		//Clear our list of text entries each frame
		m_listText.clear();
	}
}

///////////////////////////
// Add a Text Entry with
// the given parameters
///////////////////////////
void LJMUTextOverlay::writeText(std::wstring& text, Matrix4f& xform, Vector4f& colour)
{
	m_listText.push_back(LJMUTextEntry(text, xform, colour));
}

///////////////////////////
// Get the Name of this Task
///////////////////////////
std::wstring LJMUTextOverlay::GetName()
{
	return(L"LJMUTextOverlay");
}

///////////////////////////
// Resize the Viewport
///////////////////////////
void LJMUTextOverlay::Resize(UINT width, UINT height)
{
	RendererDX11::Get()->ResizeViewport(m_objViewport, width, height);
}


//------------INTERFACE METHODS---------------------------------------------------

///////////////////////////
//
///////////////////////////
void LJMUTextOverlay::SetRenderParams(IParameterManager* paramManager)
{
	//NO RENDER PARAMS NEEDED
}

///////////////////////////
//
///////////////////////////
void LJMUTextOverlay::SetUsageParams(IParameterManager* paramManager)
{
	//NO USAGE PARAMS NEEDED
}

///////////////////////////
//
///////////////////////////
void LJMUTextOverlay::Update(float time)
{
	//NOTHING TO UPDAATE
}