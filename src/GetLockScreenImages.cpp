#include <iostream>
#include "LockscreenLocator.h"
#include "LockscreenHandler.h"

int main()
{
	std::cout << "Welcome!\n";
	std::cout << "This program delivers all Windows 10 lockscreen wallpapers it can find in the system.\n";
	std::cout << "These are the images you see when you log into your computer.\n";

	LockscreenHandler handler{};
	handler.Init();

	LockscreenLocator locator{};
	locator.Run();

	handler.IntegrateNewLockscreens(locator.GetLockscreenMetadata());

	std::cout << "Press ENTER to close the window ...";
	std::cin.get();

	return 0;
}
