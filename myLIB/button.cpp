#include "button.h"

Button::Button()
{
	button = button_old = 0;
}

bool Button::rise(int button_data)
{
	if (((button & button_data) != 0) && ((button_old & button_data) == 0))
		return true;
	else return false;
}

bool Button::push(int data)
{
	if ((button & data) != 0)return true;
	else return false;
}

void Button::set_Data(int newd)
{
	Button::button_old = Button::button;
	Button::button = newd;
}
