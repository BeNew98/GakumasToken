#include <string>
#include <iostream>
#include <filesystem>

void ReadFile(std::string_view strPath, std::string& strBuf)
{
	std::FILE* pFile = nullptr;

	size_t FileSize = std::filesystem::file_size(strPath);

	strBuf.resize(std::filesystem::file_size(strPath));

	fopen_s(&pFile, strPath.data(), "rt");

	if (pFile == nullptr)
	{
		std::cout << "File is null" << std::endl;
		return;
	}

	fread_s(strBuf.data(), strBuf.size(), strBuf.size(), 1, pFile);

	if (nullptr != pFile)
	{
		fclose(pFile);
	}
}

void WriteFile(std::string_view strPath, std::string_view strData)
{
	std::FILE* pFile = nullptr;

	fopen_s(&pFile, strPath.data(), "wt");

	if (pFile == nullptr)
	{
		std::cout << "File is null" << std::endl;
		return;
	}

	fwrite(strData.data(), strData.size(), 1, pFile);

	if (nullptr != pFile)
	{
		fclose(pFile);
	}
}

bool CompareLogFileSize(std::string_view strLogPath, std::string_view strBatPath)
{
	//배치파일이 만들어진 적 없음
	if (!std::filesystem::exists(strBatPath))
	{
		return false;
	}

	size_t BatFileSize = std::filesystem::file_size(strBatPath);

	std::string strBat;
	strBat.resize(BatFileSize);

	ReadFile(strBatPath, strBat);

	size_t FirstIndex = strBat.find("::");
	
	//이전 파일크기를 저장한적 없을 때
	if (FirstIndex == std::string::npos)
	{
		return false;
	}

	size_t LogFileSize = std::filesystem::file_size(strLogPath);

	size_t LastIndex = strBat.find("\n");

	std::string strSize = strBat.substr(FirstIndex + 2, LastIndex - FirstIndex + 2);

	size_t LastSize = std::stoi(strSize);
	
	//이전 로그파일의 크기가 달라졌을 때
	if (LastSize != LogFileSize)
	{
		return false;
	}

	return true;
}

std::string CheckToken()
{
	char* cPath = nullptr;
	size_t Length = 0;

	//Appdata Roaming 경로 가져오기
	_dupenv_s(&cPath, &Length, "APPDATA");

	if (cPath == nullptr)
	{
		std::cout << "Appdata 경로가 없습니다.\n" << std::endl;
		return "";
	}

	std::filesystem::path fLogPath = cPath;
	fLogPath += "\\dmmgameplayer5\\logs\\dll.log";

	//로그파일이 만들어진 적 없음
	if (!std::filesystem::exists(fLogPath))
	{
		std::cout << "로그 파일이 없습니다. DMM에 로그인하여 학원마스를 실행시켜 주세요.\n";
		return "";
	}



	std::filesystem::path fBatPath = std::filesystem::current_path();
	fBatPath += "\\Gakumas.bat";

	bool bEqualSize = CompareLogFileSize(fLogPath.string(), fBatPath.string());

	if (bEqualSize)
	{
		return fBatPath.string();
	}

	std::string strLogFile;
	ReadFile(fLogPath.string(), strLogFile);

	//마지막 DMM Player 실행한곳 찾기
	std::string strFind = "Execute of:: gakumas exe: ";
	size_t StartIndex = strLogFile.rfind(strFind);

	if (StartIndex == std::string::npos)
	{
		std::cout << "실행지점을 찾을 수 없습니다. DMM에 로그인하여 학원마스를 실행시켜 주세요.\n";
		return "";
	}

	std::string strTemp = strLogFile.substr(StartIndex + strFind.size());

	//exe 경로 찾기
	size_t ExeEndIndex = strTemp.find("dir");

	std::string ExePath = "\"";
	ExePath += strTemp.substr(0, ExeEndIndex - 1);
	ExePath += "\"";


	//마지막으로 읽은 로그파일 크기 저장해서 이전과 비교하게끔 하기
	std::string strLastSize = "::" + std::to_string(std::filesystem::file_size(fLogPath)) + "\n";

	//arg 찾기
	size_t ArgIndex = strTemp.find("arg:");
	size_t EndIndex = strTemp.find("admin");

	std::string strArg = strTemp.substr(ArgIndex + 4, EndIndex - (ArgIndex + 4));

	std::string strCmd = strLastSize + ExePath + " " + strArg;

	//배치 파일 생성
	WriteFile(fBatPath.string(), strCmd);

	return fBatPath.string();
}

void ExcuteGakumas(std::string_view strPath)
{
	if (strPath.empty())
	{
		system("pause");
		return;
	}

	std::string strCmd = "cmd /c ";
	strCmd += strPath;
	std::system(strCmd.c_str());
}

int main()
{
	ExcuteGakumas(CheckToken());
}