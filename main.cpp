#include <string>
#include <iostream>
#include <filesystem>

void CreateBatchFile(std::string_view strCmd)
{
	std::FILE* pFile = nullptr;
	std::filesystem::path fPath = std::filesystem::current_path();
	fPath += "\\Gakumas.bat";
	fopen_s(&pFile, fPath.string().data(), "wt");

	if (pFile == nullptr)
	{
		std::cout << "File is null" << std::endl;
		return;
	}

	fwrite(strCmd.data(), strCmd.size(), 1, pFile);

	if (nullptr != pFile)
	{
		fclose(pFile);
	}	
}


void ReadLogFile()
{
	char* cPath = nullptr;
	size_t Length = 0;
	_dupenv_s(&cPath, &Length, "APPDATA");

	if (cPath == nullptr)
	{
		std::cout << "Appdata Path is null" << std::endl;
		return;
	}

	std::filesystem::path fPath = cPath;

	fPath += "\\dmmgameplayer5\\logs\\dll.log";

	std::FILE* pFile = nullptr;
	fopen_s(&pFile, fPath.string().data(), "rt");

	if (pFile == nullptr)
	{
		std::cout << "File is null" << std::endl;
		return;
	}

	std::string strFile;
	size_t fSize = std::filesystem::file_size(fPath);
	strFile.resize(fSize);

	fread_s(strFile.data(), fSize, fSize, 1, pFile);


	std::string strFind = "Execute of:: gakumas exe: ";
	size_t StartIndex = strFile.rfind(strFind);

	if (StartIndex == std::string::npos)
	{
		std::cout << "NotFound Execute Point. Please Execute DMM Player";
		return;
	}

	std::string strTemp = strFile.substr(StartIndex + strFind.size());

	size_t ExeEndIndex = strTemp.find("dir");

	std::string ExePath = "\"";
	ExePath += strTemp.substr(0, ExeEndIndex - 1);
	ExePath += "\"";

	size_t ArgIndex = strTemp.find("arg:");
	size_t EndIndex = strTemp.find("admin");

	std::string strArg = strTemp.substr(ArgIndex + 4, EndIndex - (ArgIndex + 4));

	std::string strCmd = ExePath + " " + strArg;

	if (pFile != nullptr)
	{
		fclose(pFile);
	}

	CreateBatchFile(strCmd);
}

int main()
{
	ReadLogFile();
}