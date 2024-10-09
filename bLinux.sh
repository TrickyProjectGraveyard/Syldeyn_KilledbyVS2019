echo "Output dir"
mkdir LinuxExe
echo "Compiling"
g++ -o LinuxExe/syldeyn Syldeyn/*.cpp ../../Libs/JCR6/Source/JCR6_Core.cpp ../../Libs/JCR6/Source/JCR6_Write.cpp -I../../Libs/JCR6/Headers ../../Libs/Units/Source/SlyvAsk.cpp ../../Libs/Units/Source/SlyvBank.cpp ../../Libs/Units/Source/SlyvDir.cpp ../../Libs/Units/Source/SlyvQCol.cpp ../../Libs/Units/Source/SlyvRoman.cpp ../../Libs/Units/Source/SlyvSTOI.cpp ../../Libs/Units/Source/SlyvStream.cpp ../../Libs/Units/Source/SlyvString.cpp ../../Libs/Units/Source/SlyvVolumes.cpp ../../Libs/Units/Source/SlyvTime.cpp -I../../Libs/Units/Headers
echo "Data"
cp Syldeyn/Syldeyn_Blocks.ini LinuxExe
cp Syldeyn/Syldeyn_Langs.ini LinuxExe
