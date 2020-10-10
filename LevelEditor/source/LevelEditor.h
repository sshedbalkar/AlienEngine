#pragma once
#include "AntTweakBar.h"
#include "System.h"
#include "Component.h"
#include "Vector3.h"
#include "Torus.h"
#include "Axis.h"
#include "EditorFunctionality.h"

namespace Framework
{

class TweakBarManager;
class LevelEditor : public ISystem
{
	friend class Axis;
	friend class EditorFunctionality ;
	friend class TweakBarManager;
	friend class LoadLevelClass;
private:
	enum EditorLevelState
	{
		select,
		move,
		rotate,
		scale,
		CreateObjectetype,
		genericProperties,
		numberOfStates
	};
	Axis * axisObject;
	EditorFunctionality * editorFunc;
	EditorLevelState state;
	Vector3 position,direction;
	TweakBarManager * twbmanager;
	Vec3 center;
	Vector3 clickedpoint,currentpoint,topPoint,downPoint;
	Vector3 clickedPosition,unClickedPosition,MousePosition,prevMousePosition,screenMousePosition,prevScreenMousePosition;
	std::vector<GOC *> m_gocsToCopy;
	bool isContolHeld,drawbox;
	bool leftclicked;
	void LeftClick();
	void LeftUnClick();
	typedef std::vector<GOC*> GameObjects;
	typedef std::vector<GOC*>::iterator gameObjectIterator;
	GameObjects selectedObjects;
	GOC * hoveredGOC;
	void ChangeTexturesBackToNormal();
	void ChangeTexturesToDebug();
	void AddLightToGame(GOC* g);
	void RemoveLightFromGame(GOC* g);
	bool m_showLevelBoxes;
	bool m_showBoxExcluded;
public:
	bool ShowWhatIsExcludedFromBoxes(){return m_showBoxExcluded;}
	bool GetShowLevelBoxes(){return m_showLevelBoxes;}
	Vector3 CenterDir,CenterPos;
	LevelEditor();
	~LevelEditor();
	virtual void Update( float dt );
	virtual void Initialize();
	virtual void Free();
	virtual void SendMessage( Message* m );
	virtual void Unload();
	virtual std::string GetName() { return "LevelEditor"; }
	void enterEditMode(){inEditor=true;}
	void Draw();
	bool inEditor;
	int maxSelectDistance;



};

extern LevelEditor* EDITOR;
}
