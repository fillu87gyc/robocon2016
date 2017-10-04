#include "button.h"

Button::Button()
{
	button = button_old = 0;
}

bool Button::rise(int data)
{
	if ((button & data) && !(button_old & data))
		return true;
	else return false;
}

bool Button::push(int data)
{
	if (button & data)return true;
	else return false;
}

void Button::set_data(int newd)
{
	button_old = button;
	button = newd;
}
