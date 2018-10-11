#pragma once

#include <string>
#include <irrlicht.h>

class Validator {
public:
	template<unsigned int buffer_max = 16>
	void validateInput(irr::gui::IGUIEditBox* edit_box)
	{
		unsigned int len = wcslen(edit_box->getText());
		if (len >= buffer_max)
		{
			wchar_t fixedtext[buffer_max];
			std::memcpy(fixedtext, edit_box->getText(), sizeof(fixedtext) - sizeof(wchar_t));
			fixedtext[buffer_max - 1] = L'\0';
			edit_box->setText(fixedtext);
		}
	}
};
