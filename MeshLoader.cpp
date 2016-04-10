
/***********************************************************************

					Description : STL file Operation

************************************************************************/

#include "MyConsoleEngine.h"


/*******************************************************************
									INTERFACE
*********************************************************************/
BOOL IFileManager::ImportFile_STL(std::string pFilePath, std::vector<VECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<VECTOR3>& refNormalBuffer, std::string & refFileInfo)
{
	std::ifstream tmpFile(pFilePath, std::ios::binary);

	if (!tmpFile.good())
	{
		DEBUG_MSG1("Load STL : file Open Failed!!");
		return FALSE;
	}

	//move file cursor to the end
	tmpFile.seekg(0, std::ios::end);
	std::streamoff static_fileSize = tmpFile.tellg();
	if (static_fileSize < 84L)
	{
		DEBUG_MSG1("Load STL : file Damaged!!File Size is Too Small!!");
		return FALSE;
	}
	tmpFile.seekg(0);

	char firstChar;
	tmpFile.read(&firstChar, 1);
	tmpFile.close();

	//ASCII STL starts with "solid"
	if (firstChar == 's')
	{
		return mFunction_ImportFile_STL_Ascii(pFilePath, refVertexBuffer, refIndexBuffer, refNormalBuffer, refFileInfo);
	}
	else
	{
		return mFunction_ImportFile_STL_Binary(pFilePath, refVertexBuffer, refIndexBuffer, refNormalBuffer, refFileInfo);
	}
}


/*******************************************************************
									PRIVATE
*********************************************************************/
BOOL IFileManager::mFunction_ImportFile_STL_Binary(std::string pFilePath, std::vector<VECTOR3>& refVertexBuffer,
	std::vector<UINT>& refIndexBuffer, std::vector<VECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath, std::ios::binary);

	if (!fileIn.good())
	{
		DEBUG_MSG1("Load STL Binary : Open File Failed!");
		return FALSE;
	}

	/*STL: Baidu encyclopedia

	binary STL use fixed length of bit patterns to store vertex information,
	At the beginning, 80 bytes of header will be some custom info,
	Right behind the header , next 4 bytes will be the total number of triangles;

	the rest of the file will represent every triangles in 50 bytes blocks:

	3xfloat =12bytes ------- Facet Normal
	3xfloat =12bytes ------- Vertex1
	3xfloat =12bytes ------- Vertex2
	3xfloat =12bytes ------- Vertex3
	2 byte ------ face attribute info (I don't know what's the use...)

	the length of a complete STL will be 50 *(triangleCount) + 84  */


	//newly input string
	char headerInfo[81] = {};
	fileIn.read(headerInfo, 80);

	//the first char could not be "s" in BINARY mode , in order to distinguish form ASCII mode,
	//which starts with "solid"
	if (headerInfo[0] == 's')
	{
		DEBUG_MSG1("Load STL Binary : File Damaged!! It's not binary STL file!");
		return FALSE;
	}

	refFileInfo = headerInfo;
	//move reading cursor
	fileIn.seekg(80);

	//read bit patterns, and reinterpret the data to interested type
	//using decltype()
#define REINTERPRET_READ(var) \
	fileIn.read(dataBlock,sizeof(var));\
	var=*(decltype(var)*)(void*)dataBlock;\

	//a char array used to store bit pattern (used in REINTERPRET_READ)
	char dataBlock[5] = {};

	uint32_t triangleCount = 0;
	REINTERPRET_READ(triangleCount);

	if (triangleCount > 500000)
	{
		DEBUG_MSG1("Load STL Binary : Triangle Count is larger than 500000 / Data Damamged!!");
		fileIn.close();
		return FALSE;
	}

	//then reserve spaces for vectors (optimization)
	refVertexBuffer.reserve(triangleCount * 3);
	refNormalBuffer.reserve(triangleCount);
	refIndexBuffer.reserve(triangleCount * 3);

	while (!fileIn.eof())
	{

		//a facet normal
		VECTOR3 tmpVec3(0, 0, 0);
		REINTERPRET_READ(tmpVec3.x);
		REINTERPRET_READ(tmpVec3.y);
		REINTERPRET_READ(tmpVec3.z);
		refNormalBuffer.push_back(tmpVec3);

		//3 vertices
		for (UINT i = 0;i < 3;i++)
		{
			REINTERPRET_READ(tmpVec3.x);
			REINTERPRET_READ(tmpVec3.z);
			REINTERPRET_READ(tmpVec3.y);
			refVertexBuffer.push_back(tmpVec3);
		}

		uint16_t faceAttr = 0;
		REINTERPRET_READ(faceAttr);
	}

	//clockwise or counterClockwise
	for (UINT i = 0;i < refVertexBuffer.size();i += 3)
	{
		std::swap(refVertexBuffer[i + 1], refVertexBuffer[i + 2]);
	}

	//without optimization, vertices can be overlapped
	refIndexBuffer.resize(refVertexBuffer.size());
	for (UINT i = 0;i < refIndexBuffer.size();i++)
	{
		refIndexBuffer.at(i) = i;
	}


	fileIn.close();

	return TRUE;
}

BOOL IFileManager::mFunction_ImportFile_STL_Ascii(std::string pFilePath, std::vector<VECTOR3>& refVertexBuffer, std::vector<UINT>& refIndexBuffer, std::vector<VECTOR3>& refNormalBuffer, std::string& refFileInfo)
{
	std::ifstream fileIn(pFilePath);

	if (!fileIn.good())
	{
		DEBUG_MSG1("Load STL Ascii : Open File Failed!!");
		return FALSE;
	}

	//newly input string
	std::string currString;

	//object Name
	std::string objectName;

	//the first line (object name) need to be dealt with specially
	fileIn >> currString;

	//the beginning of the 
	if (currString == "solid")
	{
		//the name of the object could be null, thus input the first line instead of
		//input next string will be safer
		char pFirstLineString[80] = {};
		fileIn.getline(pFirstLineString, 80);
		objectName = pFirstLineString;

		//delete the space at the front
		if (objectName.size()> 0)objectName.erase(objectName.begin());
	}
	else
	{
		DEBUG_MSG1("Load STL Ascii : file damaged!!");
		return FALSE;
	}

	refFileInfo = objectName;


	//loop reading
	while (!fileIn.eof())
	{
		fileIn >> currString;

		if (currString == "endsolid")break;

		//"facet normal" + x+y+z
		if (currString == "normal")
		{
			VECTOR3 tmpFaceNormal(0, 0, 0);
			fileIn >> tmpFaceNormal.x >> tmpFaceNormal.y >> tmpFaceNormal.z;
			//face normal (may be used as vertex normal)
			refNormalBuffer.push_back(tmpFaceNormal);
		}

		//"vertex" +x +y+z
		if (currString == "vertex")
		{
			VECTOR3 tmpPoint(0, 0, 0);
			fileIn >> tmpPoint.x >> tmpPoint.z >> tmpPoint.y;
			refVertexBuffer.push_back(tmpPoint);
		}
	}

	//clockwise or counterClockwise
	for (UINT i = 0;i < refVertexBuffer.size();i += 3)
	{
		std::swap(refVertexBuffer[i + 1], refVertexBuffer[i + 2]);
	}


	fileIn.close();

	//without optimization, vertices can be overlapped
	refIndexBuffer.resize(refVertexBuffer.size());
	for (UINT i = 0;i < refIndexBuffer.size();i++)
	{
		refIndexBuffer.at(i) = i;
	}

	return TRUE;
}

