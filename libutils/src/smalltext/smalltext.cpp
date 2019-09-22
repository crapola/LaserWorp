#include "smalltext/smalltext.h"
#include <algorithm> // find, min

struct SmallText::Handle
{
	size_t index;
	size_t size;
	Handle(size_t i,size_t s):index(i),size(s)
	{
	}
	// Required for std::find.
	bool operator==(const Handle& other)
	{
		return this==&other;
	}
};

SmallText::SmallText():_renderer(),_handles()
{
}

SmallText::~SmallText()
{
}

SmallText::Handle& SmallText::Add(size_t c)
{
	size_t offset=_renderer.Add(c);
	return *_handles.emplace(_handles.end(),offset,c);
}

SmallText::Handle& SmallText::Add(const std::string& s)
{
	size_t offset=_renderer.Add(s);
	return *_handles.emplace(_handles.end(),offset,s.size());
}

void SmallText::Delete(Handle& h)
{
	// Quit if not valid handle.
	auto it=std::find(_handles.begin(),_handles.end(),h);
	if (it==_handles.end())
		return;
	// Delete characters.
	_renderer.Delete(h.index,h.size);
	// Fix handles.
	for (Handle& x: _handles)
	{
		if (x.index>h.index)
		{
			x.index-=h.size;
		}
	}
	// Remove this handle.
	_handles.erase(it);
}

void SmallText::Draw()
{
	_renderer.Draw();
}

void SmallText::ForEach(const Handle& h,std::function<Character(Character)> f)
{
	_renderer.ForEach(h.index,h.size,f);
}

void SmallText::Paragraph(const Handle& h,int x,int y,int w)
{
	_renderer.Paragraph(h.index,h.size,x,y,w);
}

void SmallText::SetColor(const Handle& h,size_t o,size_t s,uint8_t f,uint8_t b)
{
	o=std::min(o,h.size);
	s=std::min(s,h.size-o);
	_renderer.SetColor(h.index+o,s,f,b);
}

SmallText::Character SmallText::Read(const Handle& h,size_t o) const
{
	return _renderer.Read(h.index+o);
}

void SmallText::Resolution(int w,int h) const
{
	_renderer.Resolution(w,h);
}

void SmallText::Write(const Handle& h,size_t o,const std::string& s)
{
	_renderer.Write(h.index+o,s);
}
