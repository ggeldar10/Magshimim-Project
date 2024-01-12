#pragma once
#include <exception>

class GetCursorPositionException : public std::exception
{
	virtual const char* what() const override {
		return "Error: Failed to get cursor position";
	};
};

class SetCursorPositionException : public std::exception
{
	virtual const char* what() const override {
		return "Error: Failed to set cursor position";
	};
};

class CursorButtonActionException : public std::exception
{
	virtual const char* what() const override {
		return "Error: Failed to make cursor button action";
	};
};
