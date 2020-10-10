////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3dx10math.h>

namespace Framework {

////////////////////////////////////////////////////////////////////////////////
// Class name: CameraClass
////////////////////////////////////////////////////////////////////////////////
class CameraClass
{
public:
	friend class Hero;
	friend class LevelEditor;
	friend class Graphics;
	friend class GravityGun;
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	//void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	//D3DXVECTOR3 GetRotation();

	void Render(float dt);
	void GetViewMatrix(D3DXMATRIX&);
	D3DXVECTOR3 Getlookat(){return m_lookAt;}
	D3DXVECTOR3 GetSideDirection(){return sideDirection;}
	D3DXVECTOR3 GetHeroUp(){return heroUp;}
	void resetCameraSpeed();
	void ResetCameraDirection();
private:
	void UpdateMovingDirectionAndLookAt(float);
	bool DebugCameraMode;
	D3DXVECTOR3 position;
	D3DXVECTOR3 m_lookAt;
	D3DXVECTOR3 heroUp;
	float CameraLookUpDownAngel;
	D3DXVECTOR3 movingDirection, sideDirection;
	bool bMousing;
	D3DXVECTOR2 ptLastFreeMousePos;
	//float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_viewMatrix;

	float fowardSpeed;
	float backwardSpeed;
	float SideMoveSpeed;
	float ElevateSpeed;
};

	extern CameraClass * CAMERA;
}

#endif
