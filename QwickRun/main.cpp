#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <cstdio>

const std::string FILENAME = "qwkapps.txt";
const std::string TEMPFILENAME = "temp.txt";

const int MAX_PATH_SIZE = 30;

std::string GetExeDirectory() {
	char path[MAX_PATH_SIZE];
	GetModuleFileName(NULL, path, MAX_PATH_SIZE);
	std::cout << path << std::endl;
	return path;
}

///Representation of entries in the file format. So far, it is just alias/npath/n
class App {
public:
	//std::vector<std::string> Aliases;
	std::string Alias;
	std::string Path;
};

///Fills the apps array with all entries in the FILENAME file
void ReadAppsFromFile(std::vector<App*> *apps) {
	
	if (apps == NULL) {
		return;
	}
	if (apps->size() > 0) {
		apps->clear();
	}

	std::ifstream readstream;
	readstream.open(FILENAME);
	if (readstream.is_open()) {
		std::string str;
		App *temp;
		while (!readstream.eof()) {
			std::getline(readstream, str);
			temp = new App();
			temp->Alias = str;
			std::getline(readstream, str);
			temp->Path = str;
			apps->push_back(temp);
		}

	}
	else {
		std::cerr << "Could not open file." << std::endl;
	}
	readstream.close();

}

///Runs an app from apps given the desired alias.
void RunByAlias(std::vector<App*> *apps, std::string DesiredAlias) {

	for (int i = 0; i < apps->size(); i++) {
		if ((*apps)[i]->Alias == DesiredAlias) {
			std::stringstream ss;
			ss << "\"";
			ss << (*apps)[i]->Path;
			ss << "\"";
			std::string str = ss.str();
			system(str.c_str());
			return;
		}
	}
	std::cout << "Could not find an app with that alias" << std::endl;
}

int main(int argc, char **argv) {
	std::string ExeDirectory = GetExeDirectory();
	std::string FullExePath;
	std::string FullTempPath;
	std::vector<App*> AppsList;
	
	//It is assumed the number of arguments helps narrow down what the desired action is.
	//This isn't a good assumption for future features, but fine for what I want.
	if (argc == 1) {
		std::cout << "Must specify actions run, config, help, or list" << std::endl;
		return 0;
	}
	else if (argc == 2) {
		std::stringstream ss;
		ss << argv[1];

		//--- List ---
		if (ss.str() == "list") {
			std::cout << "Listing all applications..." << std::endl;
			ReadAppsFromFile(&AppsList);
			for (int i = 0; i < AppsList.size(); i++) {
				std::cout << AppsList[i]->Alias << std::endl;
			}

			return 1;
		}

		//--- Help ---
		else if (ss.str() == "help") {
			std::cout << "App that lets users quickly add, remove, and run applications." << std::endl;
			std::cout << " RUN [app alias] - Run the application with the given user alias" << std::endl;
			std::cout << " LIST - List all aliases the user has created" << std::endl;
			std::cout << " CONFIG - Enter configuration mode to add/remove applications" << std::endl;

		}

		//--- Config ---
		else if (ss.str() == "config") {
			std::cout << "Entering config mode..." << std::endl;
			ReadAppsFromFile(&AppsList);

			//prompt for add or remove apps
			bool inConfig = true;
			while (inConfig) {
				std::cout << "Please enter add, remove, or exit:" << std::endl;
				std::string input;
				std::cin >> input;

				//--- Add App ---
				if (input == "add") {
					//Prompt user for the alias and path for the new app
					std::string alias;
					std::string path;
					std::cout << "Please enter alias for app:" << std::endl;
					std::cin.ignore();
					std::getline(std::cin, alias);
					std::cout << "Please enter the path for the app:" << std::endl;
					std::getline(std::cin, path);

					//Open the file stream to add the app
					std::ofstream out;
					out.open(FILENAME, std::ios::app);

					if (out.is_open()) {
						out << alias << std::endl << path << std::endl;
					}
					else {
						std::cerr << "Was not able to open the out file for writing" << std::endl;
					}

					out.close();
				}

				//--- Remove App ---
				else if (input == "remove") {
					std::string alias;
					//prompt for alias being removed
					std::cout << "enter alias to be removed:" << std::endl;
					std::cin.ignore();
					std::getline(std::cin, alias);
					
					std::ofstream out;
					std::ifstream in;

					out.open(TEMPFILENAME);
					in.open(FILENAME);
					int removedEntries = 0;

					if (out.is_open() && in.is_open()) {
						std::string readString;
						while (!in.eof()) {
							std::getline(in, readString);
							if (readString == alias) {
								//dont write to temp
								std::getline(in, readString);
								removedEntries++;
							}
							else if (readString != "") {
								//copy the contents to temp
								out << readString << std::endl;
								std::getline(in, readString);
								out << readString << std::endl;
							}
						}
						std::cout << "Removed " << removedEntries << " entries." << std::endl;
					}
					else {
						std::cerr << "Was not able to access files for removing the app" << std::endl;
						in.close();
						out.close();
						return 0; // Return so we don't do the remove and rename file ops.
					}

					in.close();
					out.close();
					
					//Swap the temp file for the normal file
					std::remove(FILENAME.c_str());
					std::rename(TEMPFILENAME.c_str(), FILENAME.c_str());
				}
				else if (input == "exit") {
					inConfig = false;
				}

			}
			return 1;
		}
		return 1;
	}
	else if (argc == 3) {
		std::stringstream ss;
		ss << argv[1];
		if (ss.str() == "run") {
			ss.str("");
			ss << argv[2];
			if (ss.str() == "") {
				std::cerr << "No alias entered. Please run by using 'qwk run [ALIAS]'." << std::endl;
				return 0;
			}
			ReadAppsFromFile(&AppsList);
			RunByAlias(&AppsList, ss.str());
		}
	}
	else {
		std::cout << argc << std::endl;
		std::cout << argv[0] << std::endl;
	}

	//app name is qwik
	//Three possible arguments:
	// Run [app]
	// Config - enters config mode
	// List - lists all applications

	
	return 0;
}