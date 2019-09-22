#pragma once
#include <functional>
#include <string>
#include "misc/backcache.hpp"
#include "gl/buffer.h"
#include "gl/shader.h"
#include "gl/program.h"
#include "gl/texture.h"
#include "gl/vao.h"
class SmallTextRenderer
{
public:
#include "smalltextrenderer_types.h"
	SmallTextRenderer();
	~SmallTextRenderer();
	// Add a number of characters to the end and return offset of added chars.
	size_t Add(size_t count);
	size_t Add(const std::string& s);
	// Delete characters.
	void Delete(size_t offset,size_t length);
	// Draw everything.
	void Draw();
	// Apply function to range.
	void ForEach(size_t offset,size_t length,std::function<Character(Character)> f);
	// Arrange in a box.
	void Paragraph(size_t offset,size_t length,int x_px,int y_px,int width);
	// Set colors for range of characters.
	void SetColor(size_t offset,size_t length,uint8_t front,uint8_t back);
	// Get character at offset.
	Character Read(size_t offset) const;
	// Set pixel resolution.
	void Resolution(int width,int height) const;
	// Write string at offset.
	void Write(size_t offset,const std::string& s);
private:
	void UploadWholeBuffer();
	BackCache<Character> _chars;
	gl::Buffer _charBuf;
	gl::Program _program;
	gl::Texture _texture;
	gl::Vao _vao;
};
