#include "Syldeyn_Config.hpp"

#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>
#include <JCR6_Core.hpp>
#include <JCR6_Write.hpp>
#include <SlyvDir.hpp>

using namespace Slyvina::Units;
using namespace Slyvina::JCR6;

#define J6E if (Last()->Error) { QCol->Error("JCR6 error: "+Last()->ErrorMessage); QCol->Doing("-> Main",Last()->MainFile); QCol->Doing("Entry",Last()->Entry); exit(500);}

namespace Slyvina {
	namespace Syldeyn {
		String SyldeynDir{ "" };
		std::map<String, String> Prefixes{};

		void GetPrefixes() {
			String PrefIni{ SyldeynDir + "/Syldeyn_Langs.ini" };
			if (!FileExists(PrefIni)) { QCol->Error(PrefIni+" not found"); exit(404); }
			auto PrefG{ LoadGINIE(PrefIni) };
			auto PrefC{ PrefG->Categories() };
			for (auto C : *PrefC) {
				auto Ext{ Split(PrefG->Value(C,"Extensions"),',') };
				for (auto E : *Ext) Prefixes[E] = PrefG->Value(C, "Prefix");
			}
		}

		void Process(String Dir) {
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
#pragma region Project
			auto SylCfg{ LoadGINIE(TDir + "/MKL_Syldeyn.ini",TDir + "/MKL_Syldeyn.ini","Syldeyn data\nUsed by Syldeyn so you don't have to type it over and over!") };
			QCol->Doing("Project", Ask(SylCfg, "::Syldeyn::", "Project", "What's the name of this project? ", StripDir(TDir)));
			QCol->Doing("Analyzing", TDir);
			auto SylDir{ GetTree(TDir) };
#pragma endregion
#pragma region Backup
			if (!DirectoryExists(TDir + "/MKL_Backup")) {
				QCol->Doing("Creating", TDir + "/MKL_Backup");
				MakeDir(TDir + "/MKL_Backup");
			}
			QCol->Doing("Shifting", "Backup");
			if (FileExists(TDir + "/MKL_Backup/10.JCR")) FileDelete(TDir + "/MKL_Backup/10.JCR");
			for (int i = 9; i >= 1; i--) {
				int err{ 0 };
				auto BDir{ TDir + "/MKL_Backup/%d.JCR" };
				auto
					Ori{ TrSPrintF(BDir.c_str(),i) },
					Tar{ TrSPrintF(BDir.c_str(),i + 1) };
				if (FileExists(Ori)) err = rename(Ori.c_str(), Tar.c_str());
			}
			QCol->Doing("Back-up", TDir);
			auto JO{ CreateJCR6(TDir + "/MKL_Backup/1.JCR") }; J6E;
#ifdef SylJCRUse_zlib
			auto JBO{ JO->AddBlock(SylJCRStorage) }; J6E;
#endif
			auto cnt{ 0 };
			for (size_t i = 0; i < SylDir->size(); i++) {
				auto f{ (*SylDir)[i] }, e{ Upper(ExtractExt(f)) }, pf{ Upper(ExtractDir(f)) };
				QCol->Cyan("\r");
				// std::cout << f << "; " << e << Prefixes.count(e) << "; " << pf << Prefixed(pf, "MKL_") << "\n";
				if ((!Prefixed(pf, "MKL_")) && (Prefixes.count(e))) {
					cnt++;
					std::cout << i << "/" << SylDir->size();
					//QCol->Pink("\t" + f);
					if (!FileExists(TDir + "/" + f))
						QCol->Error("Can't backup: " + TDir + "/" + f + " (File not found");
					else {
#ifdef SylJCRUse_zlib
						JBO->AddFile(TDir + "/" + f, f, "Syldeyn", "Syldeyn Backup");
#else
						JO->AddFile(TDir + "/" + f, f, SylJCRStorage, "Syldeyn", "Syldeyn Backup");
#endif
					}
					J6E;
				}				
			}
			std::cout << "\r";
			QCol->Doing("Packed", cnt);
#ifdef SylJCRUse_zlib
				if (!cnt) {
					JBO->AddBytes({ '1','2','3',0 }, "Leeg","Syldeyn","Error preventor, as nothing has been backed up!");
				}
				JBO->Close(); J6E;
#endif
				JO->Close(); J6E;
#pragma endregion
		}
	}
}

int main(int arglen, char** args) {
	using namespace Slyvina::Syldeyn;
	QCol->Magenta("Syldeyn\t");
	QCol->LGreen("Build: " __DATE__ "\n");
	QCol->Yellow("(c) Copyright Jeroen P. Broks\n\n");
	SylInit_zlib();
	SyldeynDir = ExtractDir(args[0]);
	GetPrefixes();
	if (arglen <= 1) Process(CurrentDir());
	else for (int i = 1; i < arglen; i++) Process(args[i]);
	return 0;
}