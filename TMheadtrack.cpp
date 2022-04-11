#include "iostream"
#include "HsvDlib.h"
#include "HSV.h"
#include "INIReaderV2.h"
#include "Cursor.h"
using namespace std;

bool is_file_exist(const char* fileName)
{
	std::ifstream infile(fileName);
	return infile.good();
}

int changeLine(string FileName, string strToReplace, string newStr) {
	string search_string = strToReplace;
	string replace_string = newStr;
	string inbuf;
	fstream input_file(FileName, ios::in);
	ofstream output_file("TMheadtrack_config_temporary.ini");

	while (!input_file.eof())
	{
		getline(input_file, inbuf);

		int spot = inbuf.find(search_string);
		if (spot >= 0)
		{
			string tmpstring = inbuf.substr(0, spot);
			tmpstring += replace_string;
			tmpstring += inbuf.substr(spot + search_string.length(), inbuf.length());
			inbuf = tmpstring;
		}

		output_file << inbuf << endl;
	}
	return 0;
}


void removeTemp() {
	remove("TMheadtrack_config_temporary.ini");
}

int tracking_method(const char* iniFile) {
	std::cout << "TEST006" << std::endl;
	INIReader reader(iniFile);
	if (reader.ParseError() != 0) {
		std::cout << "Can't load 'ini file. check if file exists or if the name is correct'\n";
		return 1;
	}
	//std::cout << reader.GetInteger("main", "tmheadtrack_tracking_method", 200) << std::endl;
	int tmheadtrack_tracking_method = reader.GetInteger("main", "tmheadtrack_tracking_method", 200);
	int tmheadtrack_camera_state = reader.GetInteger("main", "tmheadtrack_camera", 200);
	int tmheadtrack_mouse_click_method = reader.GetInteger("main", "tmheadtrack_mouse_click_method", 200);
	int tmheadtrack_camera_number = reader.GetInteger("main", "tmheadtrack_camera_nummer", 200);
	int tmheadtrack_correct_number = tmheadtrack_camera_number - 1;
	int face_rec_method = reader.GetInteger("main", "face_rec_method", 200);
	/*cout << "camera num main: " << tmheadtrack_camera_number << endl;

	cout << "correct num main: " << tmheadtrack_correct_number << endl;*/

	if (tmheadtrack_tracking_method == 0) {
		std::cout << "no valid tracking method chosen. Start the program from TMhdtr-gui and chose tracking method" << std::endl;
	}
	else if (tmheadtrack_tracking_method == 1) {
		std::cout << "TEST003";
		HSV(tmheadtrack_camera_state, tmheadtrack_mouse_click_method, tmheadtrack_correct_number, face_rec_method);
	}
	else if (tmheadtrack_tracking_method == 2) {
		std::cout << "TEST004";
		HsvDlib(tmheadtrack_camera_state, tmheadtrack_mouse_click_method, tmheadtrack_correct_number, face_rec_method);
	}

	return 1;
}

int main(int argc, char* argv[])
{
	std::cout << "TEST001" << std::endl;
	if (is_file_exist("TMheadtrack_config_user.ini")) {
		changeLine("TMheadtrack_config_user.ini", "##[main]", "[main]");

	}
	else {
		changeLine("TMheadtrack_config.ini", "##[main]", "[main]");

	}
	
	if (is_file_exist("TMheadtrack_config_temporary.ini")) {
		
		tracking_method(".\\TMheadtrack_config_temporary.ini");
	}
	else {
		std::cout << "TEST004" << std::endl;

		std::cout << "No .ini file" << std::endl;
	}
	std::cout << "TEST005" << std::endl;

	std::atexit(removeTemp);
	//std::atexit(cursor_Image("mouse.cur", 32512, def_arrow_cur, false));
	return 0;
}

