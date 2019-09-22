#pragma once
// Writes tag, then last OpenGL error to standard output.
// Returns true if there was an error.
namespace gl
{
bool LogErrors(const char* tag);
}
