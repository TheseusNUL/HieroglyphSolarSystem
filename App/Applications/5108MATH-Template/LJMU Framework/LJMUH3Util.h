#ifndef _LJMU_UTIL_H_
#define _LJMU_UTIL_H_

#include <map>

#include "ResourceProxyDX11.h"
#include "SpriteFontDX11.h"

namespace LJMUDX
{
	
	//---------------------UTIL RESOURCE HOLDER CLASS---------------------------------------------------------------------------------
	class LJMUResourceHelper
	{
	public:
		void					LoadTexture(int id, const std::wstring& filename);
		void					LoadFont(int id,std::wstring& filename, int size);
		Glyph3::ResourcePtr		GetTexture(int id);
		Glyph3::SpriteFontPtr   GetFont(int id);

	private:
		void	InsertResource(int id, Glyph3::ResourcePtr resource);
		void	InsertResource(int id, Glyph3::SpriteFontPtr resource);


	private:
		std::map<int, Glyph3::ResourcePtr>	m_resourceMap;
		std::map<int, Glyph3::SpriteFontPtr> m_resourceFont;
	};
}

#endif 
