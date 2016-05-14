#pragma once

namespace Math
{
	const float CONST_PI = 3.1415926f;

	struct VECTOR2
	{
		VECTOR2() { };
		VECTOR2(float X, float Y) { x = X; y = Y; };
		VECTOR2(const float column[2])
		{
			x = column[0];
			y = column[1];
		}

		float	Length()
		{
			return sqrt(x*x + y*y);
		};

		VECTOR2&	Normalize()
		{
			float len = Length();
			if (len != 0)
			{
				x /= len;
				y /= len;
			}
			else
			{
				x = 0.0f;
				y = 0.0f;
			}
			return *this;
		}

		BOOL	operator==(const VECTOR2& vec)
		{
			if (vec.x == x&&vec.y == y)
				return TRUE;
			else
				return FALSE;
		}

		VECTOR2 operator+(const VECTOR2& vec)
		{
			return VECTOR2(x + vec.x, y + vec.y);
		}

		VECTOR2 operator-(const VECTOR2& vec)
		{
			return VECTOR2(x - vec.x, y - vec.y);
		}

		VECTOR2& operator+=(const VECTOR2& vec)
		{
			x += vec.x;
			y += vec.y;
			return *this;
		}

		VECTOR2& operator-=(const VECTOR2& vec)
		{
			x -= vec.x;
			y -= vec.y;
			return *this;
		}

		VECTOR2 operator*(const float scaleFactor)
		{
			return VECTOR2(scaleFactor*x, scaleFactor* y);
		}

		friend VECTOR2 operator*(float scaleFactor, const VECTOR2& vec)
		{
			return VECTOR2(scaleFactor*vec.x, scaleFactor* vec.y);
		}


		VECTOR2& operator*=(const float scaleFactor)
		{
			x *= scaleFactor;
			y *= scaleFactor;
			return *this;
		}

		float x;
		float y;
	};

	struct VECTOR3
	{
		VECTOR3() { };
		VECTOR3(float X, float Y, float Z) { x = X; y = Y; z = Z; };
		VECTOR3(const float column[3])
		{
			x = column[0];
			y = column[1];
			z = column[2];
		}

		float	Length() const
		{
			return sqrtf(x*x + y*y + z*z );
		};

		VECTOR3&	Normalize()
		{
			float len = Length();
			if (len != 0)
			{
				x /= len;
				y /= len;
				z /= len;
			}
			else
			{
				x = y = z = 0.0f;
			}
			return *this;
		}


		BOOL	operator==(const VECTOR3& vec) 
		{
			if (vec.x == x&&vec.y == y&&vec.z == z)
				return TRUE;
			else
				return FALSE;
		}

		VECTOR3 operator+(const VECTOR3& vec)
		{
			return VECTOR3(x + vec.x, y + vec.y, z + vec.z);
		}

		VECTOR3 operator-(const VECTOR3& vec)
		{
			return VECTOR3(x - vec.x, y - vec.y, z - vec.z);
		}

		friend VECTOR3 operator-(const VECTOR3& v1, const VECTOR3& v2)
		{
			return VECTOR3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
		};

		VECTOR3& operator+=(const VECTOR3& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
			return *this;
		}

		VECTOR3& operator-=(const VECTOR3& vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			return *this;
		}

		VECTOR3 operator*(const float scaleFactor)
		{
			return VECTOR3(scaleFactor*x, scaleFactor* y, scaleFactor* z);
		}

		VECTOR3 operator*(const VECTOR3& vec)
		{
			return VECTOR3(vec.x*x, vec.y*y, vec.z*z);
		}

		//the scalar is put before the vector3
		friend VECTOR3 operator*(float scaleFactor,const VECTOR3& vec)
		{
			return VECTOR3(scaleFactor*vec.x, scaleFactor* vec.y, scaleFactor* vec.z);
		}

		VECTOR3& operator*=(const float scaleFactor)
		{
			x *=scaleFactor;
			y *= scaleFactor;
			z *=scaleFactor;
			return *this;
		}

		float x;
		float y;
		float z;
	};

	struct VECTOR4
	{
		VECTOR4() { };
		VECTOR4(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; };
		VECTOR4(const float column[4]) 
		{
			x = column[0];
			y = column[1];
			z = column[2];
			w = column[3];
		}

		float	Length()
		{
			return sqrt(x*x + y*y + z*z + w*w);
		};

		VECTOR4&	Normalize()
		{
			float len = Length();
			if (len != 0.0f)
			{
				x /= len;
				y /= len;
				z /= len;
				w /= len;
			}
			else
			{
				x = y = z = w = 0.0f;
			}
			return *this;
		}

		BOOL	operator==(const VECTOR4& vec)
		{
			if (vec.x == x&&vec.y == y&&vec.z == z&&vec.w==w)
				return TRUE;
			else
				return FALSE;
		}

		VECTOR4 operator+(const VECTOR4& vec)
		{
			return VECTOR4(x + vec.x, y + vec.y, z + vec.z, w + vec.w);
		}

		VECTOR4 operator-(const VECTOR4& vec)
		{
			return VECTOR4(x - vec.x, y - vec.y, z - vec.z, w - vec.w);
		}

		VECTOR4& operator+=(const VECTOR4& vec)
		{
			x += vec.x;
			y += vec.y;
			z += vec.z;
			w += vec.w;
			return *this;
		}

		VECTOR4& operator-=(const VECTOR4& vec)
		{
			x -= vec.x;
			y -= vec.y;
			z -= vec.z;
			w -= vec.w;
			return *this;
		}

		VECTOR4 operator*(const float scaleFactor)
		{
			return VECTOR4(scaleFactor*x, scaleFactor* y, scaleFactor* z, scaleFactor* w);
		}

		friend VECTOR4 operator*(float scaleFactor, const VECTOR4& vec)
		{
			return VECTOR4(scaleFactor*vec.x, scaleFactor* vec.y, scaleFactor* vec.z,scaleFactor*vec.w);
		}

		VECTOR4& operator*=(const float scaleFactor)
		{
			x *= scaleFactor;
			y *= scaleFactor;
			z *= scaleFactor;
			w *= scaleFactor;
			return *this;
		}

		float x;
		float y;
		float z;
		float w;
	};

	struct MATRIX4x4
	{
		MATRIX4x4()
		{
			Identity();
		}

		inline void	SetRow(UINT row, float newRow[4])
		{
			if (row < 4)
			{
				for (UINT i = 0;i < 4;i++)
					m[row][i] = newRow[i];
			}
		};

		inline void	SetRow(UINT row, float(&&newRow)[4])
		{
			if (row < 4)
			{
				for (UINT i = 0;i < 4;i++)
					m[row][i] = newRow[i];
			}
		};

		inline void	SetColumn(UINT col, float newCol[4])
		{
			if (col < 4)
			{
				for (UINT i = 0;i < 4;++i)
					m[i][col] = newCol[i];
			}
		};

		inline void	SetColumn(UINT col, float(&&newCol)[4])
		{
			if (col < 4)
			{
				for (UINT i = 0;i < 4;++i)
					m[i][col] = newCol[i];
			}
		};
		
		inline void SetMatrix(VECTOR4 (&&row)[4])
		{
			for (UINT i = 0;i < 4;++i)
			{
				m[i][0] = row[i].x;
				m[i][1] = row[i].y;
				m[i][2] = row[i].z;
				m[i][3] = row[i].w;
			}
		};

		void Identity()
		{
			SetRow(0, { 1,0,0,0 });
			SetRow(1, { 0,1,0,0 });
			SetRow(2, { 0,0,1,0 });
			SetRow(3, { 0,0,0,1 });
		};

		float m[4][4];
	};

	struct BOUNDINGBOX
	{
		BOUNDINGBOX()
		{
			max = { 1,1,1 };
			min = { 0,0,0 };
		}

		VECTOR3 max;
		VECTOR3 min;
	};


	extern float Vec2_Dot(const VECTOR2& vec1, const VECTOR2& vec2);

	extern float Vec2_Length(const VECTOR2& vec);

	extern float Vec3_Dot(const VECTOR3& vec1, const VECTOR3& vec2);

	extern VECTOR3 Vec3_Cross(const VECTOR3& vec1, const VECTOR3& vec2);

	extern float Vec3_Length(const VECTOR3& vec);

	extern VECTOR3 Vec3_Reflect(const VECTOR3& vec, const VECTOR3& axis);

	extern float Vec4_Dot(const VECTOR4& vec1, const VECTOR4& vec2);

	extern float Vec4_Length(const VECTOR4& vec);
	
		/*
	  Y|    / Z
		|  /
		|/________ X
		*/


	extern MATRIX4x4 Matrix_Multiply(const MATRIX4x4& leftMat, const MATRIX4x4& rightMat);

	extern VECTOR4	Matrix_Multiply(const MATRIX4x4 leftMat, const VECTOR4& rightColumnVec);


	extern MATRIX4x4	Matrix_Translation(float dx, float dy, float dz);

	extern MATRIX4x4	Matrix_RotationX(float angle);

	extern MATRIX4x4	Matrix_RotationY(float angle);

	extern MATRIX4x4	Matrix_RotationZ(float angle);

	extern MATRIX4x4	Matrix_YawPitchRoll(float yaw_Y, float pitch_X, float roll_Z);

	extern MATRIX4x4	Matrix_PerspectiveProjection(float ViewAngleY, float aspectRatio, float NearPlaneZ, float FarPlaneZ);

	extern MATRIX4x4	Matrix_Transpose(const MATRIX4x4& inMatrix);

	extern UINT Clamp(UINT val, UINT min, UINT max);

	extern float Clamp(float val, float min, float max);

	extern VECTOR2 Clamp(const VECTOR2& val,const VECTOR2& min, const VECTOR2& max);

	extern VECTOR3 Clamp(const VECTOR3& val, const VECTOR3& min, const VECTOR3& max);

	extern BYTE Lerp(BYTE start, BYTE end, float t);

	extern float Lerp(float start, float end, float t);

	extern VECTOR2 Lerp(const VECTOR2& start, const VECTOR2& end, float t);

	extern VECTOR3 Lerp(const VECTOR3& start, const VECTOR3& end, float t);

	extern BOOL Intersect_Ray_AABB(const VECTOR3& rayStart, const VECTOR3& rayEnd,const BOUNDINGBOX& box, VECTOR3& outIntersectPoint, BOOL testFrontSide);

}
