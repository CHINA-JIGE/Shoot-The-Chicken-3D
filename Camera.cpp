/************************************************************************

							CPP:  	Noise Camera
				
1，关于YawPitchRoll的正角定义：（其实是左手系正角的定义）
			Yaw:俯视图顺时针转
			Pitch:y往负的方向转（低头）
			Roll:逆时针转


************************************************************************/

#pragma once

#include "MyConsoleEngine.h"

ICamera::ICamera()
{
	m_pFatherScene = NULL;
	mRotateX_Pitch=0;
	mRotateY_Yaw=0;
	mRotateZ_Roll=0;
	mViewAngleY = (float)60/180 * MATH_PI;
	mAspectRatio = 1.5;
	m_pPosition = new D3DXVECTOR3(0,0,0);
	m_pLookat = new D3DXVECTOR3(1,0,0);
	m_pDirection = new D3DXVECTOR3(1,0,0);
	mNearPlane=1;
	mFarPlane= 1000;

	m_pMatrixProjection = new D3DXMATRIX();
	m_pMatrixView = new D3DXMATRIX();
	D3DXMatrixPerspectiveFovLH(m_pMatrixProjection,mViewAngleY,mAspectRatio,mNearPlane,mFarPlane);
	D3DXMatrixIdentity(m_pMatrixView);
};

void ICamera::Destroy()
{
	 m_pMatrixView=nullptr;
	 m_pMatrixProjection = nullptr;
	 m_pDirection = nullptr;
	 m_pPosition = nullptr;
	 m_pLookat = nullptr;

};

void	ICamera::SetLookAt(NVECTOR3 vLookat)
{
	*m_pLookat=vLookat;
	mFunction_UpdateRotation();
};

void	ICamera::SetLookAt(float x,float y,float z)
{
	NVECTOR3 tmpLookat(x,y,z);
	*m_pLookat=tmpLookat;
	mFunction_UpdateRotation();
};

NVECTOR3	ICamera::GetLookAt()
{
	return  *m_pLookat;
};

void	ICamera::SetPosition(NVECTOR3 vPos)
{
	//lookat和位置不能重合啊
		*m_pPosition=vPos;
		mFunction_UpdateRotation();
}

void	ICamera::SetPosition(float x,float y,float z)
{
	NVECTOR3 tmpPos(x,y,z);
	*m_pPosition=tmpPos;
	mFunction_UpdateRotation();
};

NVECTOR3 ICamera::GetPosition()
{
	return *m_pPosition;
};

void	ICamera::Move(NVECTOR3 vRelativePos)
{
	D3DXVec3Add(m_pPosition,m_pPosition,&vRelativePos);
	D3DXVec3Add(m_pLookat, m_pLookat, &vRelativePos);

};

void	ICamera::Move(float relativeX,float relativeY,float relativeZ)
{
	*m_pPosition += NVECTOR3(relativeX, relativeY, relativeZ);
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
	mRotateX_Pitch = AngleX > (MATH_PI / 2) ? (MATH_PI / 2) : (AngleX < (-MATH_PI / 2) ? (-MATH_PI / 2) : AngleX);

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

	NVECTOR3 relativePos;

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

		NVECTOR3 relativePos;

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
	*m_pPosition += NVECTOR3(0, fSignedDistance, 0);
	*m_pLookat += NVECTOR3(0, fSignedDistance, 0);
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
	if(iViewAngleY>0 && (mViewAngleY <(MATH_PI/2))){mViewAngleY	=	iViewAngleY;	}
	if(iAspectRatio>0){mAspectRatio	= iAspectRatio;}
};


/************************************************************************
											PRIVATE	
************************************************************************/

void	ICamera::mFunction_UpdateProjMatrix()
{
	D3DXMatrixPerspectiveFovLH(
		m_pMatrixProjection,
		mViewAngleY,
		mAspectRatio,
		mNearPlane,
		mFarPlane);

	//要更新到GPU，TM居然要先转置
	D3DXMatrixTranspose(m_pMatrixProjection,m_pMatrixProjection);
};

void	ICamera::mFunction_UpdateViewMatrix()
{

	D3DXMATRIX	tmpMatrixTranslation;
	D3DXMATRIX	tmpMatrixRotation;
	//先对齐原点
	D3DXMatrixTranslation(&tmpMatrixTranslation, -m_pPosition->x, -m_pPosition->y, -m_pPosition->z);
	//然后用yawpitchroll的逆阵转到view空间
	D3DXMatrixRotationYawPitchRoll(&tmpMatrixRotation, mRotateY_Yaw, mRotateX_Pitch, mRotateZ_Roll);
	//正交矩阵的转置是逆
	D3DXMatrixTranspose(&tmpMatrixRotation,&tmpMatrixRotation);
	//先平移，再旋转
	D3DXMatrixMultiply(m_pMatrixView,&tmpMatrixTranslation,&tmpMatrixRotation);
	//要更新到GPU，TM居然要先转置
	D3DXMatrixTranspose(m_pMatrixView,m_pMatrixView);

};

void	ICamera::mFunction_UpdateRotation()
{
	//主要功能：这个函数主要是为了处理Direction改变带来的姿态角变化

	//更新direction
	D3DXVECTOR3	tmpDirection;
	//检查新direction是否为0
	D3DXVec3Subtract(&tmpDirection,m_pLookat,m_pPosition);
	float mLength = D3DXVec3Length(&tmpDirection);
	//注意浮点数误差，视点和位置不能重合
	if (mLength<0.001)
	{
		//重置摄像机啊 尼玛 这样都set得出来不如去死啊
		mRotateX_Pitch = 0;
		mRotateY_Yaw = 0;
		mRotateZ_Roll = 0;
		*m_pDirection = NVECTOR3(1.0f, 0, 0);
		*m_pLookat = *m_pPosition + *m_pDirection;
		return;
	}
	else
	//视点和位置不重合 ，再赋值
	{ *m_pDirection = tmpDirection; }
	;

	//临时变量，储存的比值用来arctan获得Pitch角度
	float tmpRatio;
	//pitch角： tan = y/sqr(x^2+z^2))
	/*	注意：	atan取值范围是 [-pi/2,pi/2]  
					atan2取值范围是 [-pi,pi] 		*/
	if((m_pDirection->x==0) && (m_pDirection->z==0))
	{
		//是否在原点的正上下方
		if(m_pDirection->y>=0)
		{mRotateX_Pitch=-MATH_PI/2;}
		else
		{mRotateX_Pitch=MATH_PI/2;}
	}
	else
	{
		//去你的 难道y负的时候是正角————对的没错，DX用左手系，左手系旋转正方向用左手决定
		//用大拇指指着旋转轴正向，四指指向正旋转方向
		tmpRatio =-m_pDirection->y /  sqrt(pow(m_pDirection->x,2.0f)+pow(m_pDirection->z,2.0f));
		mRotateX_Pitch = atan(tmpRatio);//返回[-0.5pi,0.5pi]
	}

	//yaw角： tan = -x/z
		mRotateY_Yaw = atan2(m_pDirection->x,m_pDirection->z);//俯视图yaw是顺时针正角

	//roll角：更新direction不会改变roll角 
	//roll逆时针转是正角
};

void	ICamera::mFunction_UpdateDirection()
{
	//主要功能：这个函数主要是为了处理姿态角改变带来的视线Direction变化

	//要更新Lookat
	float tmpDirectionLength = D3DXVec3Length(m_pDirection);
	//直接用三角函数表达Direction	3dscanner的世界变换一章里面有
	m_pDirection->x =- tmpDirectionLength* sin(mRotateY_Yaw)* cos(mRotateX_Pitch);
	m_pDirection->z =tmpDirectionLength* cos(mRotateY_Yaw)*cos(mRotateX_Pitch);
	m_pDirection->y =tmpDirectionLength* sin(mRotateX_Pitch);
	D3DXVec3Add(m_pLookat,m_pPosition,m_pDirection);
};
