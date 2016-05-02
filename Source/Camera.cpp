/************************************************************************

							CPP:  	Noise Camera
				
1，关于YawPitchRoll的正角定义：（其实是左手系正角的定义）
			Yaw:俯视图顺时针转
			Pitch:y往负的方向转（低头）
			Roll:逆时针转


************************************************************************/

#pragma once

#include "MyConsoleEngine.h"

using namespace Math;

ICamera::ICamera()
{
	mRotateX_Pitch=0;
	mRotateY_Yaw=0;
	mRotateZ_Roll=0;
	mViewAngleY = 60.0f/180.0f * CONST_PI;
	mAspectRatio = 1.5;
	mPosition = VECTOR3(0,0,0);
	mLookat = VECTOR3(1,0,0);
	mDirection = VECTOR3(1,0,0);
	mNearPlane=1.0f;
	mFarPlane= 1000.0f;


	mMatrixProjection= Matrix_PerspectiveProjection(mViewAngleY,mAspectRatio,mNearPlane,mFarPlane);
	mMatrixView.Identity();
};

void ICamera::Destroy()
{
};

void	ICamera::SetLookAt(VECTOR3 vLookat)
{
	mLookat=vLookat;
	mFunction_UpdateRotation();
};

void	ICamera::SetLookAt(float x,float y,float z)
{
	VECTOR3 tmpLookat(x,y,z);
	mLookat=tmpLookat;
	mFunction_UpdateRotation();
};

VECTOR3	ICamera::GetLookAt()
{
	return  mLookat;
};

void	ICamera::SetPosition(VECTOR3 vPos)
{
	//lookat和位置不能重合啊
		mPosition=vPos;
		mFunction_UpdateRotation();
}

void	ICamera::SetPosition(float x,float y,float z)
{
	VECTOR3 tmpPos(x,y,z);
	mPosition=tmpPos;
	mFunction_UpdateRotation();
};

VECTOR3 ICamera::GetPosition()
{
	return mPosition;
}
VECTOR3 ICamera::GetDirection()
{
	return mLookat - mPosition;
};

void	ICamera::Move(VECTOR3 vRelativePos)
{
	mPosition+=vRelativePos;
	mLookat+=vRelativePos;

};

void	ICamera::Move(float relativeX,float relativeY,float relativeZ)
{
	mPosition += VECTOR3(relativeX, relativeY, relativeZ);
};

void	ICamera::SetRotation(float RX_Pitch,float RY_Yaw,float RZ_Roll)//要更新Lookat
{
	SetRotationX_Pitch(RX_Pitch);
	SetRotationY_Yaw(RY_Yaw);
	SetRotationZ_Roll(RZ_Roll);
	mFunction_UpdateDirection();
};

void	ICamera::SetRotationY_Yaw(float angleY)
{
	mRotateY_Yaw = angleY;
	mFunction_UpdateDirection();
};

void	ICamera::SetRotationX_Pitch(float AngleX)
{
	//clamp to [-pi/2,pi/2]
	mRotateX_Pitch = Clamp(AngleX, -(CONST_PI / 2), (CONST_PI / 2));

	mFunction_UpdateDirection();
};

void	ICamera::SetRotationZ_Roll(float AngleZ)
{
	//roll翻滚不需要更新lookat
	mRotateZ_Roll = AngleZ;
}

float ICamera::GetRotationY_Yaw()
{
	return mRotateY_Yaw;
}

float ICamera::GetRotationX_Pitch()
{
	return mRotateX_Pitch;
}

float ICamera::GetRotationZ_Roll()
{
	return mRotateZ_Roll;
}

void ICamera::RotateY_Yaw(float angleY)
{
	SetRotationY_Yaw(mRotateY_Yaw + angleY);
	mFunction_UpdateDirection();
};

void ICamera::RotateX_Pitch(float angleX)
{
	float newAngle = mRotateX_Pitch + angleX;
	SetRotationX_Pitch(newAngle);
	mFunction_UpdateDirection();
};

void ICamera::RotateZ_Roll(float angleZ)
{
	SetRotationZ_Roll(mRotateZ_Roll + angleZ);
	mFunction_UpdateDirection();
};

void ICamera::fps_MoveForward(float fSignedDistance, BOOL enableYAxisMovement)
{
	//...Yaw Angle Starts at Z axis ( left-handed system) 
	/*		Z
			|    / A
			|_ /   
		O	|/__________ X		

			angle AOZ is the yaw angle
	
	*/

	VECTOR3 relativePos;

	//generate a direction first (later multiply it with fDist)
	if(enableYAxisMovement)
	{ 
		relativePos.x = sinf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
		relativePos.z = cosf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
		relativePos.y = sinf(mRotateX_Pitch);
	}
	else
	{
		relativePos.x = sinf(mRotateY_Yaw);
		relativePos.z = cosf(mRotateY_Yaw);
		relativePos.y = 0;
	}

	// length * unit_dir
	relativePos *= fSignedDistance;

	//relative movement
	Move(relativePos);

}

void ICamera::fps_MoveRight(float fSignedDistance, BOOL enableYAxisMovement)
{
	//...Yaw Angle Starts at Z axis ( left-handed system) 
	/*		
		Z
		|    / A
		|_ /
	O	|/__________ X

	angle AOZ is the yaw angle

	*/

		VECTOR3 relativePos;

		//generate a direction first (later multiply it with fDist)
		if (enableYAxisMovement)
		{
			relativePos.x = cosf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
			relativePos.z = -sinf(mRotateY_Yaw) *cosf(mRotateX_Pitch);
			relativePos.y = sinf(mRotateX_Pitch);
		}
		else
		{
			relativePos.x = cosf(mRotateY_Yaw);
			relativePos.z = -sinf(mRotateY_Yaw);
			relativePos.y = 0;
		}

		// length * unit_dir
		relativePos *= fSignedDistance;

		//relative movement
		Move(relativePos);
}

void ICamera::fps_MoveUp(float fSignedDistance)
{
	mPosition += VECTOR3(0, fSignedDistance, 0);
	mLookat += VECTOR3(0, fSignedDistance, 0);
}

void ICamera::GetViewMatrix(MATRIX4x4 & outMat)
{
	mFunction_UpdateViewMatrix();
	outMat = mMatrixView;
}

void ICamera::GetProjMatrix(MATRIX4x4 & outMat)
{
	mFunction_UpdateProjMatrix();
	outMat = mMatrixProjection;
};




void	ICamera::SetViewFrustumPlane(float iNearPlaneZ,float iFarPlaneZ)
{
	if ( (iNearPlaneZ >0) && (iFarPlaneZ>iNearPlaneZ))
	{
		mNearPlane	= iNearPlaneZ;
		mFarPlane	=	iFarPlaneZ;
	}

};

void ICamera::SetViewAngle(float iViewAngleY,float iAspectRatio)
{
	if(iViewAngleY>0 && (mViewAngleY <(CONST_PI/2.0f))){mViewAngleY	=	iViewAngleY;	}
	if(iAspectRatio>0){mAspectRatio	= iAspectRatio;}
};


/************************************************************************

											PRIVATE	

************************************************************************/

void	ICamera::mFunction_UpdateProjMatrix()
{
	mMatrixProjection=Matrix_PerspectiveProjection(
		mViewAngleY,
		mAspectRatio*c_ConsoleCharAspectRatio,
		mNearPlane,
		mFarPlane);

};

void	ICamera::mFunction_UpdateViewMatrix()
{

	MATRIX4x4	tmpMatrixTranslation;
	MATRIX4x4	tmpMatrixRotation;
	//先对齐原点
	
	tmpMatrixTranslation=Matrix_Translation(-mPosition.x, -mPosition.y, -mPosition.z);
	//然后用 yawpitchroll的逆阵 转到view空间
	tmpMatrixRotation = Matrix_YawPitchRoll(mRotateY_Yaw,mRotateX_Pitch, mRotateZ_Roll);
	//正交矩阵的转置是逆
	tmpMatrixRotation=Matrix_Transpose(tmpMatrixRotation);
	//先平移，再旋转 (column vector)
	mMatrixView= Matrix_Multiply(tmpMatrixRotation, tmpMatrixTranslation);


	//!!!!!!!!!!!怎么原来Noise3D一直用着行向量吗尼玛！连shader也跟着行向量了
};

void	ICamera::mFunction_UpdateRotation()
{
	//主要功能：这个函数主要是为了处理Direction改变带来的姿态角变化

	//更新direction
	VECTOR3	tmpDirection;
	//检查新direction是否为0
	tmpDirection=mLookat-mPosition;
	float mLength = Vec3_Length(tmpDirection);
	//注意浮点数误差，视点和位置不能重合
	if (mLength<0.001f)
	{
		//重置摄像机啊 尼玛 这样都set得出来不如去死啊
		mRotateX_Pitch = 0;
		mRotateY_Yaw = 0;
		mRotateZ_Roll = 0;
		mDirection = VECTOR3(1.0f, 0, 0);
		mLookat = mPosition + mDirection;
		return;
	}
	else
	//视点和位置不重合 ，再赋值
	{ mDirection = tmpDirection; }
	;

	//临时变量，储存的比值用来arctan获得Pitch角度
	float tmpRatio;
	//pitch角： tan = y/sqr(x^2+z^2))
	/*	注意：	atan取值范围是 [-pi/2,pi/2]  
					atan2取值范围是 [-pi,pi] 		*/
	if((mDirection.x==0) && (mDirection.z==0))
	{
		//是否在原点的正上下方
		if(mDirection.y>=0)
		{mRotateX_Pitch=-CONST_PI/2;}
		else
		{mRotateX_Pitch= CONST_PI /2;}
	}
	else
	{
		//去你的 难道y负的时候是正角————对的没错，DX用左手系，左手系旋转正方向用左手决定
		//用大拇指指着旋转轴正向，四指指向正旋转方向
		tmpRatio =-mDirection.y /  sqrt(pow(mDirection.x,2.0f)+pow(mDirection.z,2.0f));
		mRotateX_Pitch = atan(tmpRatio);//返回[-0.5pi,0.5pi]
	}

	//yaw角： tan = -x/z
		mRotateY_Yaw = atan2(mDirection.x,mDirection.z);//俯视图yaw是顺时针正角

	//roll角：更新direction不会改变roll角 
	//roll逆时针转是正角
};

void	ICamera::mFunction_UpdateDirection()
{
	//主要功能：这个函数主要是为了处理姿态角改变带来的视线Direction变化

	//要更新Lookat
	float tmpDirectionLength = Vec3_Length(mDirection);
	//直接用三角函数表达Direction	3dscanner的世界变换一章里面有
	mDirection.x = tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	mDirection.z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	mDirection.y =-tmpDirectionLength* sin(mRotateX_Pitch);
	mLookat=mPosition+mDirection;
};
