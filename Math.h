#pragma once

#define MATH_PI 3.1415926f

struct VECTOR2
{
	VECTOR2() { };
	VECTOR2(float X, float Y) { x = X; y = Y;};

	VECTOR2 operator+(const VECTOR2& vec)
	{
		return VECTOR2(x + vec.x, y + vec.y);
	}

	VECTOR2 operator-(const VECTOR2& vec)
	{
		return VECTOR2(x - vec.x, y - vec.y);
	}

	VECTOR2 operator+=(const VECTOR2& vec)
	{
		return VECTOR2(x + vec.x, y + vec.y);
	}

	VECTOR2 operator-=(const VECTOR2& vec)
	{
		return VECTOR2(x - vec.x, y - vec.y);
	}

	VECTOR2 operator*(const float scaleFactor)
	{
		return VECTOR2(scaleFactor*x, scaleFactor* y);
	}

	VECTOR2 operator*=(const float scaleFactor)
	{
		return VECTOR2(scaleFactor*x, scaleFactor* y);
	}

	float x;
	float y;
};

struct VECTOR3
{
	VECTOR3() { };
	VECTOR3(float X, float Y, float Z) { x = X; y = Y; z = Z; };

	VECTOR3 operator+(const VECTOR3& vec)
	{
		return VECTOR3(x+vec.x , y+vec.y ,z+ vec.z);
	}

	VECTOR3 operator-(const VECTOR3& vec)
	{
		return VECTOR3(x-vec.x, y-vec.y, z-vec.z);
	}

	VECTOR3 operator+=(const VECTOR3& vec)
	{
		return VECTOR3(x + vec.x, y + vec.y, z + vec.z);
	}

	VECTOR3 operator-=(const VECTOR3& vec)
	{
		return VECTOR3(x - vec.x, y - vec.y, z - vec.z);
	}

	VECTOR3 operator*(const float scaleFactor)
	{
		return VECTOR3(scaleFactor*x, scaleFactor* y, scaleFactor* z);
	}

	VECTOR3 operator*=(const float scaleFactor)
	{
		return VECTOR3(scaleFactor*x, scaleFactor* y, scaleFactor* z);
	}

	float x;
	float y;
	float z;
};

struct VECTOR4
{
	VECTOR4() { };
	VECTOR4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; };

	VECTOR4 operator+(const VECTOR4& vec)
	{
		return VECTOR4(x + vec.x, y + vec.y, z + vec.z,w+vec.w);
	}

	VECTOR4 operator-(const VECTOR4& vec)
	{
		return VECTOR4(x - vec.x, y - vec.y, z - vec.z,w-vec.w);
	}

	VECTOR4 operator+=(const VECTOR4& vec)
	{
		return VECTOR4(x + vec.x, y + vec.y, z + vec.z,w+vec.w);
	}

	VECTOR4 operator-=(const VECTOR4& vec)
	{
		return VECTOR4(x - vec.x, y - vec.y, z - vec.z,w-vec.w);
	}

	VECTOR4 operator*(const float scaleFactor)
	{
		return VECTOR4(scaleFactor*x, scaleFactor* y, scaleFactor* z, scaleFactor* w);
	}

	VECTOR4 operator*=(const float scaleFactor)
	{
		return VECTOR4(scaleFactor*x, scaleFactor* y, scaleFactor* z,scaleFactor*w);
	}

	float x;
	float y;
	float z;
	float w;
};

class MATRIX4x4
{
public:
	MATRIX4x4()
	{
		Identity();
	}

	void	SetRow(UINT row, float newRow[4])
	{
		if (row < 4)
		{
			for (UINT i = 0;i < 4;i++)
				mMatrix[row][i] = newRow[i];
		}
	};

	void	SetRow(UINT row, float (&&newRow)[4])
	{
		if (row < 4)
		{
			for (UINT i = 0;i < 4;i++)
				mMatrix[row][i] = newRow[i];
		}
	};

	void	SetColumn(UINT col, float newCol[4])
	{
		if (col < 4)
		{
			for (UINT i = 0;i < 4;++i)
				mMatrix[i][col] = newCol[i];
		}
	};

	void	SetMatrix(float mat[4][4])
	{
		for (UINT i = 0;i < 4;i++)
			for (UINT j = 0;j < 4;j++)
				mMatrix[i][j] = mat[i][j];
	}

	void Identity()
	{
		SetRow(0, {1,0,0,0 });
		SetRow(1,{ 0,1,0,0 });
		SetRow(2,{ 0,0,1,0 });
		SetRow(3,{ 0,0,0,1 });
	};

	void	Transpose()
	{
		std::swap(mMatrix[0][1], mMatrix[1][0]);
		std::swap(mMatrix[0][2], mMatrix[2][0]);
		std::swap(mMatrix[0][3], mMatrix[3][0]);
		std::swap(mMatrix[2][1], mMatrix[1][2]);
		std::swap(mMatrix[3][1], mMatrix[1][3]);
		std::swap(mMatrix[3][2], mMatrix[2][3]);
	};

	BOOL Inverse()
	{

	};

	void	RotationX(float angle)
	{

	};

private:
	float mMatrix[4][4];
};