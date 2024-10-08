#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>
using namespace Slyvina::Units;

namespace Slyvina {
	namespace Syldeyn {
		String SyldeynDir{ "" };
		std::map<String, String> Prefixes{};

		void GetPrefixes() {
			String PrefIni{ SyldeynDir + "/Syldeyn_Langs.ini" };
			if (!FileExists(PrefIni)) { QCol->Error("Syldeyn_Langs.ini not found"); exit(404); }
			auto PrefG{ LoadGINIE(PrefIni) };
			auto PrefC{ PrefG->Categories() };
			for (auto C : *PrefC) {
				auto Ext{ Split(PrefG->Value(C,"Extensions"),',') };
				for (auto E : *Ext) Prefixes[E] = PrefG->Value(C, "Prefix");
			}
		}

		void Process(String Dir){
#pragma region "Check or initiate"
			Dir = ChReplace(Dir, '\\', '/');
			QCol->Doing("Checking", Dir);
			auto TDir{ Dir };
			while (!FileExists(TDir + "/MKL_Syldeyn.ini")) {
				//QCol->Doing("->", TDir);
				if (FileExists(TDir + "/License.MKL.ini")) {
#ifdef SlyvWindows
					QCol->Doing("Converting", TDir);
					auto rc = system(String(SyldeynDir + " \"" + TDir + "\"").c_str());
					if (rc) { QCol->Error(TrSPrintF("Converter program threw an error (%d)", rc)); return; }
					if (!FileExists(TDir + "/MKL_Syndeyn.ini")) { QCol->Error("Converter threw no error, but Syldeyn data appears not to be generated"); return; }					
					break; // All's well, so no futher operation needed.
#else
					QCol->Error("MKL license found for this project, but no Syldeyn data");
					QCol->LMagenta("I cannot convert this in Linux. Windows will be required.");
					return;
#endif
				}
				TDir = ExtractDir(TDir);
				if (TDir == "") {
					TDir = Dir;
					if (QuickYes("It appears this is no project known to Syldeyn. Initiate it")) {
						SaveString(TDir + "/MKL_Syldeyn.ini", "# Syldeyn initial file!\n\n[::Syldeyn::]\nCreated=" + CurrentDate() + "; " + CurrentTime());
						break;
					}
					QCol->Error("User doesn't want this to be a Syldeyn project!"); return;
				}
			}
#pragma endregion
			auto SylCfg{ LoadGINIE(TDir + "/MKL_Syldeyn.ini",TDir + "/MKL_Syldeyn.ini","Syldeyn data\nUsed by Syldeyn so you don't have to type it over and over!") };
			QCol->Doing("Project", Ask(SylCfg, "::Syldeyn::", "Project", "What's the name of this project? ", StripDir(TDir)));
		}
	}
}

int main(int arglen, char** args) {
	using namespace Slyvina::Syldeyn;
	QCol->Magenta("Syldeyn\t");
	QCol->LGreen("Build: " __DATE__ "\n");
	QCol->Yellow("(c) Copyright Jeroen P. Broks\n\n");
	SyldeynDir = ExtractDir(args[0]);
	GetPrefixes();
	if (arglen <= 1) Process(CurrentDir());
	else for (int i = 1; i < arglen; i++) Process(args[i]);
	return 0;
}