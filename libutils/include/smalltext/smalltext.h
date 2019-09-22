#pragma once
#include <list>
#include "../../src/smalltext/smalltextrenderer.h"
/*
Render basic monospace text in OpenGL using an embedded 8x8 font.
*/
class SmallText
{
public:
	// Opaque handle to string.
	struct Handle;
	// SmallText::Character.
	typedef SmallTextRenderer::Character Character;
	SmallText();
	~SmallText();
	// Add a number of characters.
	Handle& Add(size_t count);
	Handle& Add(const std::string& s);
	// Delete characters.
	void Delete(Handle& handle);
	// Draw everything.
	void Draw();
	// Apply function to range.
	void ForEach(const Handle& handle,std::function<Character(Character)> f);
	// Arrange in a box of width characters positioned at x,y pixels.
	void Paragraph(const Handle& handle,int x,int y,int width);
	// Set colors of range of characters.
	void SetColor(const Handle& handle,size_t offset,size_t length,uint8_t front,uint8_t back);
	// Get character at offset.
	Character Read(const Handle& h,size_t offset) const;
	// Set pixel resolution.
	void Resolution(int width,int height) const;
	// Write string at offset.
	void Write(const Handle& handle,size_t offset,const std::string& s);
private:
	SmallTextRenderer _renderer;
	std::list<Handle> _handles;
};
/*
Character colors are 2 bits per component in a byte (rrggbbaa).
*/
constexpr Color2b(int r,int g,int b,int a)
{
	return ((r<<6)|(g<<4)|(b<<2)|a);
}
