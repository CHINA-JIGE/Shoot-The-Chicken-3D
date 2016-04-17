
#include <iostream>
#include "MyConsoleEngine.h"

using namespace Math;
using namespace std;

int main()
{
	VECTOR2 v2a = { 1,2 };
	VECTOR2 v2b = { 1,2 };
	cout<<"Vec2 Dot: "<<Vec2_Dot(v2a, v2b)<<endl;

	VECTOR3 v3a = { 1,2,3 };
	VECTOR3 v3b = { 2,3,4 };
	VECTOR3 v3c = { 1,0,0 };
	VECTOR3 v3d = { 0,1,0 };
	cout << "Vec3 Dot:"<<Vec3_Dot(v3a, v3b) << "     " << Vec3_Dot(v3c, v3d)<<endl;
	VECTOR3 tmpVec;
	tmpVec = Vec3_Cross(v3c, v3d);
	cout << "Vec3 Cross:" << tmpVec.x << "," << tmpVec.y << ", " << tmpVec.z << endl;

	VECTOR4 v4a = { 1,2,3,4 };
	VECTOR4 v4b = { 1,2,-1,-2 };
	cout << "Vec4 Dot:" << Vec4_Dot(v4a, v4b) << endl;

	MATRIX4x4 mat1;
	MATRIX4x4 mat2;
	MATRIX4x4 tmpMat;
	mat1.Identity();
	mat1.Identity();
	tmpMat = Matrix_Multiply(mat1, mat2);
	mat1=Matrix_RotationX(CONST_PI / 2.0f);
	mat2 = Matrix_RotationY(CONST_PI / 2.0f);
	tmpMat = Matrix_Multiply(mat1, mat2);

	mat1.SetMatrix({ 
		{ 1,2,3,4 },
		{ 1,2,2,1 },
		{ 2,2,2,2 },
		{ 1,1,1,1 }
	});

	mat2.SetMatrix({
		{ 1,2,3,4 },
		{ 1,2,3,4 },
		{ 1,2,3,4 },
		{ 1,2,3,4 }
	});

	tmpMat = Matrix_Multiply(mat1, mat2);
	system("pause");
}