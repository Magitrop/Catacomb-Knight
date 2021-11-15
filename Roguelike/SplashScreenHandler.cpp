#define Initializer GameInitializer::Instance()
#define Controller GameController::Instance()
#include "GameController.h"

GameController::SplashScreen::SplashScreen(
	double _maxFadeState,
	double _fadeIn, 
	double _fadeOut, 
	double _waiting, 
	double _textIn, 
	double _textOut,
	TextsContainer *_text)
{
	maxFadeState = _maxFadeState;
	curFadeState = curTextFadeState = innerWaitingTimer = 0;
	fadeInDuration = _fadeIn;
	fadeOutDuration = _fadeOut;
	waitingDuration = _waiting;
	textInDuration = _textIn;
	textOutDuration = _textOut;
	text = _text;
	state = SplashScreenState::Preparation;
}

// returns true when full cycle is done
bool GameController::SplashScreen::Play()
{
	switch (state)
	{
	case SplashScreenState::Preparation:
		Controller.inSplashScreen = true;
		state = SplashScreenState::FadingIn;
		return false;
	case SplashScreenState::FadingIn:
		if (Controller.ScreenFade(true, maxFadeState, 255 * Time::GetDeltaTime() / fadeInDuration, &curFadeState))
		{
			state = SplashScreenState::TextFadingIn;
			if (onFullFadeIn != nullptr)
				onFullFadeIn();
		}
		return false;
	case SplashScreenState::TextFadingIn:
		Controller.ScreenFade(true, maxFadeState, 255 * Time::GetDeltaTime() / fadeInDuration, &curFadeState);
		if (Controller.TextFade(text, true, 255 * Time::GetDeltaTime() / textInDuration, &curTextFadeState))
		{
			state = SplashScreenState::Waiting;
			innerWaitingTimer = 0;
			if (onFullTextIn != nullptr)
				onFullTextIn();
		}
		return false;
	case SplashScreenState::Waiting:
		innerWaitingTimer += Time::GetDeltaTime();
		Controller.ScreenFade(true, maxFadeState, 255 * Time::GetDeltaTime() / fadeInDuration, &curFadeState);
		Controller.TextFade(text, true, 255 * Time::GetDeltaTime() / textInDuration, &curTextFadeState);
		if (innerWaitingTimer > waitingDuration)
		{
			state = SplashScreenState::TextFadingOut;
			if (onWaitingEnd != nullptr)
				onWaitingEnd();
		}
		return false;
	case SplashScreenState::TextFadingOut:
		Controller.ScreenFade(true, maxFadeState, 255 * Time::GetDeltaTime() / fadeInDuration, &curFadeState);
		if (Controller.TextFade(text, false, 255 * Time::GetDeltaTime() / textOutDuration, &curTextFadeState))
		{
			state = SplashScreenState::FadingOut;
			if (onFullTextOut != nullptr)
				onFullTextOut();
		}
		return false;
	case SplashScreenState::FadingOut:
		if (Controller.ScreenFade(false, maxFadeState, 255 * Time::GetDeltaTime() / fadeOutDuration, &curFadeState))
		{
			state = SplashScreenState::Done;
			if (onFullFadeOut != nullptr)
				onFullFadeOut();
		}
		return false;
	case SplashScreenState::Done:
		Controller.inSplashScreen = false;
		return true;
	}
}