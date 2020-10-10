#pragma once
#include "Precompiled.h"
#include "System.h"
#include "Component.h"
#include "Vector3.h"
#include "Torus.h"
#include "Axis.h"

namespace Framework
{

class EditorFunctionality 
{
	friend class Axis;
	friend class LevelEditor;
private:
	void UpdatePosition();
	void UpdateRotation();
	void CopySelectedObjects();
	void DeleteSelectedObjects();
	void SelectObjects();
	void InitializeLightsIfDeleted();
public:
	EditorFunctionality();
	~EditorFunctionality();
	GOC* GetGOCFromIntersection(Vector3& point,Vector3& eye);
};

}
