#pragma once
#include "application.h"

int main()
{
	auto aut = new terrgen(320, 180);
	application app(aut);
	app.run();
	delete aut;
}