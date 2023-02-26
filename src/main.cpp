#include "main_loop.hpp"

int main()
{
	if(!hackru_init(1920, 1080, "HackRU 2023"))
		return -1;

	hackru_main_loop();
	hackru_close();
}