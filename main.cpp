#include <string>
#include <iostream>
#include <filesystem>

void CreateBatchFile(std::string_view strCmd)
{
	//파일 생성
	std::FILE* pFile = nullptr;
	std::filesystem::path fPath = std::filesystem::current_path();
	fPath += "\\Gakumas.bat";
	fopen_s(&pFile, fPath.string().data(), "wt");

	if (pFile == nullptr)
	{
		std::cout << "File is null" << std::endl;
		return;
	}

	//데이터 쓰기
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

	//Appdata Roaming 경로 가져오기
	_dupenv_s(&cPath, &Length, "APPDATA");

	if (cPath == nullptr)
	{
		std::cout << "Appdata Path is null" << std::endl;
		return;
	}

	std::filesystem::path fPath = cPath;
	fPath += "\\dmmgameplayer5\\logs\\dll.log";

	std::FILE* pFile = nullptr;

	//로그 파일 열기
	fopen_s(&pFile, fPath.string().data(), "rt");

	if (pFile == nullptr)
	{
		std::cout << "File is null" << std::endl;
		return;
	}

	std::string strFile;
	size_t fSize = std::filesystem::file_size(fPath);
	strFile.resize(fSize);

	//string으로 변환
	fread_s(strFile.data(), fSize, fSize, 1, pFile);


	//마지막 DMM Player 실행한곳 찾기
	std::string strFind = "Execute of:: gakumas exe: ";
	size_t StartIndex = strFile.rfind(strFind);

	if (StartIndex == std::string::npos)
	{
		std::cout << "NotFound Execute Point. Please Execute DMM Player";
		return;
	}

	std::string strTemp = strFile.substr(StartIndex + strFind.size());

	//exe 경로 찾기
	size_t ExeEndIndex = strTemp.find("dir");

	std::string ExePath = "\"";
	ExePath += strTemp.substr(0, ExeEndIndex - 1);
	ExePath += "\"";


	//arg 찾기
	size_t ArgIndex = strTemp.find("arg:");
	size_t EndIndex = strTemp.find("admin");

	std::string strArg = strTemp.substr(ArgIndex + 4, EndIndex - (ArgIndex + 4));

	std::string strCmd = ExePath + " " + strArg;

	if (pFile != nullptr)
	{
		fclose(pFile);
	}

	//배치 파일 생성
	CreateBatchFile(strCmd);
}

int main()
{
	ReadLogFile();
}