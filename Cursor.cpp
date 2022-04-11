#include <Windows.h>
#include <iostream>
using namespace std;
void cursor_Image(LPCSTR cursor_file_name, int value_of_cursor_to_change, HCURSOR copy_of_original_cursor, bool program_running) {
	
	if (program_running) {
		HCURSOR cursor = LoadCursorFromFile(cursor_file_name);
		SetSystemCursor(cursor, value_of_cursor_to_change);
	}
	else {
		SetSystemCursor(copy_of_original_cursor, value_of_cursor_to_change);
	}
	
}
