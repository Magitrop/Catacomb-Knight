#pragma once
#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()

#include <iostream>
#include <stdexcept>
#include <vector>
#include <windows.h>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "GameInitializer.h"
#include "GameController.h"

using namespace std;

class Text final
{
public:
	enum class AlignMode
	{
		ToLeft,
		ToMiddle,
		ToRight
	};
private:
	int length;
	AlignMode alignMode;
	/// <summary>
	/// text with max length, max font size etc.
	/// </summary>
	Text *mainText;

	int renderFromX = 0;
	int renderFromY = 0;

	TTF_Font *font;

	void Initialize()
	{
		if (mainText == nullptr)
			mainText = this;
		if (alignMode == AlignMode::ToLeft)
			textureRect =
				new SDL_Rect
				{
					renderFromX - (int)(mainText->fontSize * mainText->GetLength() / 2 * sharpnessCoefficient),
					renderFromY + (int)(fontSize * 2 * sharpnessCoefficient),
					(int)(fontSize * length * sharpnessCoefficient),
					(int)(fontSize * 2 * sharpnessCoefficient)
				};
		else if (alignMode == AlignMode::ToMiddle)
			textureRect =
				new SDL_Rect
				{
					renderFromX - (int)(fontSize * length / 2 * sharpnessCoefficient),
					renderFromY + (int)(fontSize * 2 * sharpnessCoefficient),
					(int)(fontSize * length * sharpnessCoefficient),
					(int)(fontSize * 2 * sharpnessCoefficient)
				};
		else
			textureRect =
				new SDL_Rect
				{
					renderFromX - 
						(int)(fontSize * length * sharpnessCoefficient) +
						(int)(mainText->fontSize * mainText->GetLength() / 2 * sharpnessCoefficient),
					renderFromY + (int)(fontSize * 2 * sharpnessCoefficient),
					(int)(fontSize * length * sharpnessCoefficient),
					(int)(fontSize * 2 * sharpnessCoefficient)
				};
		SDL_Surface *textSurface =
			TTF_RenderUNICODE_Blended(
				font,
				(Uint16*)(text.c_str()),
				color);
		texture = SDL_CreateTextureFromSurface(
			Initializer.renderer,
			textSurface);
		SDL_FreeSurface(textSurface);
		wasInitialized = true;
	}
public:
	bool wasInitialized;
	SDL_Texture *texture;
	SDL_Rect *textureRect;
	SDL_Color color;

	wstring text;
	const double sharpnessCoefficient = 0.25;
	const int fontSize;
	/// <summary>
	/// pixels above this text
	/// </summary>
	const int lineSpacingAfter;

	static wstring utf8toUtf16(const string& str)
	{
		if (str.empty())
			return wstring();

		size_t charsNeeded = ::MultiByteToWideChar(CP_UTF8, 0,
			str.data(), (int)str.size(), NULL, 0);
		if (charsNeeded == 0)
			throw runtime_error("Failed converting UTF-8 string to UTF-16");

		vector<wchar_t> buffer(charsNeeded);
		int charsConverted = ::MultiByteToWideChar(CP_UTF8, 0,
			str.data(), (int)str.size(), &buffer[0], buffer.size());
		if (charsConverted == 0)
			throw runtime_error("Failed converting UTF-8 string to UTF-16");

		return wstring(&buffer[0], charsConverted);
	}
	void RebuildText()
	{
		if (mainText == nullptr)
			mainText = this;
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
		length = text.length();
		if (alignMode == AlignMode::ToLeft)
		{
			textureRect->x = renderFromX - (int)(mainText->fontSize * mainText->GetLength() / 2 * sharpnessCoefficient);
			textureRect->y = renderFromY + (int)(fontSize * 2 * sharpnessCoefficient);
			textureRect->w = (int)(fontSize * length * sharpnessCoefficient);
			textureRect->h = (int)(fontSize * 2 * sharpnessCoefficient);
		}
		else if (alignMode == AlignMode::ToMiddle)
		{
			textureRect->x = renderFromX - (int)(fontSize * length / 2 * sharpnessCoefficient);
			textureRect->y = renderFromY + (int)(fontSize * 2 * sharpnessCoefficient);
			textureRect->w = (int)(fontSize * length * sharpnessCoefficient);
			textureRect->h = (int)(fontSize * 2 * sharpnessCoefficient);
		}
		else
		{
			textureRect->x = renderFromX -
				(int)(fontSize * length * sharpnessCoefficient) +
				(int)(mainText->fontSize * mainText->GetLength() / 2 * sharpnessCoefficient);
			textureRect->y = renderFromY + (int)(fontSize * 2 * sharpnessCoefficient);
			textureRect->w = (int)(fontSize * length * sharpnessCoefficient);
			textureRect->h = (int)(fontSize * 2 * sharpnessCoefficient);
		}
		
		SDL_Surface *textSurface =
			TTF_RenderUNICODE_Blended(
				font,
				reinterpret_cast<Uint16 const *>(text.c_str()),
				color);
		texture = SDL_CreateTextureFromSurface(
			Initializer.renderer,
			textSurface);
		SDL_FreeSurface(textSurface);
	}

	~Text()
	{
		TTF_CloseFont(font);
		if (textureRect != nullptr)
			delete textureRect;
		SDL_DestroyTexture(texture);
	}

	Text() : fontSize(0), lineSpacingAfter(0) {}
	Text(string fontLocation,
		wstring value, int _fontSize, 
		int _lineSpacing = 5,
		AlignMode _alignMode = AlignMode::ToMiddle,
		SDL_Color _color = { 255, 255, 255, 255},
		int _x = 0,
		int _y = 0) : fontSize(_fontSize), lineSpacingAfter(_lineSpacing)
	{
		font = TTF_OpenFont(fontLocation.c_str(), _fontSize);
		text = value;
		color = _color;
		alignMode = _alignMode;
		renderFromX = _x;
		renderFromY = _y;
		length = text.length();
		mainText = nullptr;
		texture = nullptr;
		wasInitialized = false;
		//Initialize();
	}
	Text(string fontLocation,
		string value, int _fontSize,
		int _lineSpacing = 5,
		AlignMode _alignMode = AlignMode::ToMiddle,
		SDL_Color _color = { 255, 255, 255, 255 },
		int _x = 0,
		int _y = 0) : fontSize(_fontSize), lineSpacingAfter(_lineSpacing)
	{
		font = TTF_OpenFont(fontLocation.c_str(), _fontSize);
		text = Text::utf8toUtf16(value);
		color = _color;
		alignMode = _alignMode;
		renderFromX = _x;
		renderFromY = _y;
		length = text.length();
		mainText = nullptr;
		texture = nullptr;
		wasInitialized = false;
		//Initialize();
	}
	Text(string fontLocation, 
		const char* value, 
		int _fontSize, 
		int _lineSpacing = 5,
		AlignMode _alignMode = AlignMode::ToMiddle,
		SDL_Color _color = { 255, 255, 255, 255 },
		int _x = 0,
		int _y = 0) : fontSize(_fontSize), lineSpacingAfter(_lineSpacing)
	{
		font = TTF_OpenFont(fontLocation.c_str(), _fontSize);
		text = wstring((wchar_t*)value);
		color = _color;
		alignMode = _alignMode;
		renderFromX = _x;
		renderFromY = _y;
		length = text.length();
		mainText = nullptr;
		texture = nullptr;
		wasInitialized = false;
		//Initialize();
	}

	/// <summary>
	/// for left and right align
	/// </summary>
	void SetMainText(Text *_mainText)
	{
		mainText = _mainText;
	}
	void SetRenderCoordinates(int _x, int _y)
	{
		if (renderFromX == _x && renderFromY == _y)
			return;
		renderFromX = _x;
		renderFromY = _y;
		if (!wasInitialized)
			Initialize();
		else 
			RebuildText();
		//Initialize();
	}
	void SetRenderCoordinatesWithoutRerender(int _x, int _y)
	{
		if (renderFromX == _x && renderFromY == _y)
			return;
		renderFromX = _x;
		renderFromY = _y;
		length = text.length();
		if (alignMode == AlignMode::ToLeft)
		{
			textureRect->x = renderFromX - (int)(mainText->fontSize * mainText->GetLength() / 2 * sharpnessCoefficient);
			textureRect->y = renderFromY + (int)(fontSize * 2 * sharpnessCoefficient);
		}
		else if (alignMode == AlignMode::ToMiddle)
		{
			textureRect->x = renderFromX - (int)(fontSize * length / 2 * sharpnessCoefficient);
			textureRect->y = renderFromY + (int)(fontSize * 2 * sharpnessCoefficient);
		}
		else
		{
			textureRect->x = renderFromX -
				(int)(fontSize * length * sharpnessCoefficient) +
				(int)(mainText->fontSize * mainText->GetLength() / 2 * sharpnessCoefficient);
			textureRect->y = renderFromY + (int)(fontSize * 2 * sharpnessCoefficient);
		}
	}
	int Render(Uint8 alpha)
	{
		SDL_SetTextureAlphaMod(texture, alpha);
		return SDL_RenderCopy(
			Initializer.renderer,
			texture,
			nullptr, textureRect);
	}
	int GetWidth()
	{
		if (mainText != nullptr)
			return mainText->GetLength() * fontSize * sharpnessCoefficient;
		else return GetLength() * fontSize * sharpnessCoefficient;
	}
	int GetHeight()
	{
		return fontSize * 2 * sharpnessCoefficient;
	}
	int GetLength()
	{
		return length;
	}
	operator wstring()
	{
		return text;
	}
};
class TextsContainer final
{
private:
	Text *mainText;
	int linesCount;
	int maxWidth;
public:
	Text* texts;

	TextsContainer(int _linesCount, Text *_texts)
	{
		linesCount = _linesCount;
		texts = _texts;

		mainText = &texts[0];
		int curLength = 0;
		for (int i = 0; i < linesCount; i++)
		{
			if (texts[i].GetLength() * texts[i].fontSize > 
				mainText->GetLength() * mainText->fontSize)
				mainText = &texts[i];
		}

		maxWidth = 0;
		int curWidth = 0;
		for (int i = 0; i < linesCount; i++)
		{
			texts[i].SetMainText(mainText);
			curWidth = texts[i].GetWidth();
			if (curWidth > maxWidth)
				maxWidth = curWidth;
		}
	}
	void Initialize(int fromX, int fromY)
	{
		for (int i = 0; i < linesCount; i++)
		{
			texts[i].SetRenderCoordinates(
				fromX,
				fromY);
			fromY += (int)(texts[i].fontSize * 2 * texts[i].sharpnessCoefficient) + texts[i].lineSpacingAfter;
		}
	}
	void Render(Uint8 alpha)
	{
		for (int i = 0; i < linesCount; i++)
			texts[i].Render(alpha);
	}
	int GetHeight(void)
	{
		int result = 0;
		for (int i = 0; i < linesCount; i++)
			result += texts[i].GetHeight() + texts[i].lineSpacingAfter;
		return result;
	}
	int GetWidth(void)
	{
		return maxWidth;
	}
};