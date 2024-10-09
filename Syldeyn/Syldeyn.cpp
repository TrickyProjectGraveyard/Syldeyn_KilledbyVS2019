#include "Syldeyn_Config.hpp"

#include <SlyvQCol.hpp>
#include <SlyvString.hpp>
#include <SlyvStream.hpp>
#include <SlyvAsk.hpp>
#include <SlyvTime.hpp>
#include <JCR6_Core.hpp>
#include <JCR6_Write.hpp>
#include <SlyvDir.hpp>
#include <SlyvSilly.hpp>

using namespace Slyvina::Units;
using namespace Slyvina::JCR6;

#define J6E if (Last()->Error) { QCol->Error("JCR6 error: "+Last()->ErrorMessage); QCol->Doing("-> Main",Last()->MainFile); QCol->Doing("Entry",Last()->Entry); exit(500);}

namespace Slyvina {
	namespace Syldeyn {
		String SyldeynDir{ "" };
		std::map<String, String> Prefixes{};
		struct _LBlock {
			String
				Name{ "" },
				ID{ "" };				
			std::vector<String> Fields, License{};
			std::map<String, String> Defaults;
		};
		typedef std::shared_ptr<_LBlock> LBlock;
		std::map<uint32, LBlock> Blocks;
		std::map<String, LBlock> BlocksByName;
		std::map<String, LBlock> BlocksByID;

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

		void GetBlocks() {
			String BlckIni{ SyldeynDir + "/Syldeyn_Blocks.ini" };
			if (!FileExists(BlckIni)) { QCol->Error(BlckIni + " not found"); exit(404); }
			auto BlckG{ LoadGINIE(BlckIni) };
			auto BlckC{ BlckG->Categories() };
			auto num{ 0u };
			for (auto C : *BlckC) if (!BlocksByID.count(C)) {
				auto B{ new _LBlock() };
				auto SB{ std::shared_ptr<_LBlock>(B) };
				auto
					LL{ BlckG->List(C,"License") },
					LF{ BlckG->List(C,"Fields") };
				B->ID = C;
				B->Name = BlckG->Value(C, "Name");
				Blocks[++num] = SB;
				BlocksByName[B->Name] = SB;
				BlocksByID[B->ID] = SB;
				B->Fields = *LF;
				std::cout << "Debug block #" << num << ": " << SB->Name << "; (" << SB->ID << ")\n";
				for (auto LLL : *LL) {
					auto NL{ StReplace(LLL,"<$tab>","\t") }; NL = StReplace(NL, "<$spc>", " ");
					if (LLL == "$WL") B->License.push_back(""); else B->License.push_back(NL);
				}
				for (auto FLD : B->Fields) {
					B->Defaults[FLD] = BlckG->Value(C, "Default." + FLD);
				}
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
				JBO->AddBytes({ '1','2','3','1','2','3','1','2','3','1','2','3','1','2','3',0 }, "Leeg", "Syldeyn", "Error preventor, as nothing has been backed up!");
			}
			JBO->Close(); J6E;
#endif
			JO->Close(); J6E;
#pragma endregion
#pragma region Scan
			QCol->Doing("Scanning", TDir);
			for (auto a : *SylDir) {
				auto ad{ ExtractDir(a) }, ae{ Upper(ExtractExt(a)) };
				auto Known{ SylCfg->BoolValue(a,"Known") }, Allow{ (!VecHasString(SylCfg->List(":Dirs:","Ignore"),ad)) && Prefixes.count(ae) };
				//std::cout << "DEBUG: " << Known << Allow << (!VecHasString(SylCfg->List(":Dirs:", "Ignore"), ad)) << Prefixes.count(ae) << ": " << a << "/" << ad << "/" << ae << "\n";
				if (!Allow) continue;
				if (!Known) {
					QCol->Reset();
					cls();
					QCol->Color(qColor::Yellow, qColor::Red);
					std::cout << a; QCol->Reset(); std::cout << "\n";
					std::cout << " 1 = Add to the Syldeyn registry for version update\n";
					std::cout << " 2 = Ignore this file for now\n";
					std::cout << " 3 = Ignore this file forever\n";
					if (ad != "") {
						std::cout << " 4 = Ignore the directory \"" << ad << "\" forever\n";
					}
				opnieuw:
					auto ch{ ToInt(ReadLine("Please tell me what to do: ")) };
					switch (ch) {
					case 1: SylCfg->BoolValue(a, "Allow", true); SylCfg->BoolValue(a, "Known", true); Allow = true; break;
					case 2: continue;
					case 3: SylCfg->BoolValue(a, "Allow", false); SylCfg->BoolValue(a, "Known", true); continue;
					case 4: if (ad != "") {
						SylCfg->Add(":Dirs:", "Ignore", ad);
						continue;
					}
						  // NO BREAK! This is a fallthrough!
					default:
						std::cout << "? Redo from start!\n"; // Just a nod to good old GW-BASIC. Part of my history as a coder!
					}
					if (!Allow) goto opnieuw; // the 'continue' command forces me to or the wrong loop will be 'continued'.
				}
				Allow = SylCfg->BoolValue(a, "Allow");
				auto Modified{ false }; // Must normally be false, but can be set to 'true' for debugging reasons. Then all files will always count as "modified".
				Modified = Modified || FileTimeStamp(TDir + "/" + a) != SylCfg->IntValue(a, "Time");
				Modified = Modified || FileSize(TDir + "/" + a) != SylCfg->IncValue(a, "Size");
				if (Allow && Modified) {
					QCol->Doing("Updating", a);
					if (SylCfg->Value(a, "License") == "" && SylCfg->Value(a, "LicenseByName") != "") {
						auto LBN{ SylCfg->Value(a, "LicenseByName") };
						if (BlocksByName.count(LBN)) SylCfg->Value(a, "License", Upper(BlocksByName[LBN]->ID));
						else QCol->Error("License \"" + LBN + "\" could not be retrieved from Syldeyn");
					}
					if (SylCfg->Value(a, "License") == "") {
						QCol->Color(qColor::Yellow, qColor::Blue); std::cout << "License";
						QCol->Reset(); std::cout << "\n";
						auto maxch{ 0u };
						for (auto id : Blocks) {
							std::cout << TrSPrintF("%2d = ", id.first) + id.second->Name+"\n";
							maxch = std::max(maxch, id.first);
						}
						auto ch{ 0u };
						do ch = ToUInt(ReadLine("Please select the license: ")); while (ch<1 || ch>maxch);
						SylCfg->Value(a, "License", Blocks[ch]->ID);
					}
					SylCfg->NewValue(a, "File", a); // Void case ignoring.
					auto cYear{ Trim(std::to_string(CurrentYear())) };
					auto iYear{ Ask(SylCfg,a,"iYear","Initial Year:",cYear) };
					auto cpyYear{ SylCfg->NewValue(a,"cYear",iYear) };
					if (!Suffixed(cpyYear, cYear)) {
						QCol->Doing("Year correction: " + cpyYear, " -> ");
						cpyYear + ", " + cYear;
						SylCfg->Value(a, "cYear", cpyYear);
						QCol->LGreen(cpyYear + "\n");
					}
					auto Blk{ BlocksByID[SylCfg->Value(a,"License")] };
					if (!Blk) {
						QCol->Error("Block '" + SylCfg->Value(a, "License") + "' appears to be null"); exit(501);
					}
					std::map<String, String> Fld{};
					for (auto variable : Blk->Fields) {
						if (variable != "");
						Fld[variable] = Ask(SylCfg, a, "FLD." + variable, "FIELD>\t" + variable + ": ", Blk->Defaults[variable]);
					}
				}
			}
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
	GetBlocks();
	if (arglen <= 1) Process(CurrentDir());
	else for (int i = 1; i < arglen; i++) Process(args[i]);
	QCol->LCyan("Ok");
	QCol->Reset();
	return 0;
}