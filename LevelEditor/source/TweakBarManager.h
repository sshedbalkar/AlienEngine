#pragma once
#include "AntTweakBar.h"
#include "Precompiled.h"
#include "Component.h"
#include <map>
#include "LevelEditor.h"
#include "TweakBarControl.h"

namespace Framework
{


typedef struct {
    float Object1;
    float Object2;
    float Object3;
	float Object4;

} ActivationPropertiesStruct;



class TweakBarManager
{

	typedef std::map<const char*, TweakBarControl*> TweakBarList;
private:
	int m_archEnumId;
	std::map<int, std::string> m_archetypes;
	std::map<int,bool> m_constraintIdsTaken;
	std::map<int,bool> m_ropeIdsTaken;
	TweakBarList m_twbarlist;
	GOC * m_selectedGOC;
	std::vector<GOC*> m_SelectedGOCs;
	enum MyEnum {Object,CreateObject, MultipleObjects,GenericProperties, LightProperties,Nothing};//add new types here
	const char* GetStringFromEnum(MyEnum);
	MyEnum GetEnumFromString(const char* s);
	TwType ActivationPropertiesStructType;
	int dominosNo;
	float dominosDistance;
	int xC, yC, zC;
	bool leftToRight;
	//std::map<int,GOC *> ObjectsThatHaveConstraints;
	char *s2;
public:
	TweakBarManager ();
	~TweakBarManager();
	static void TW_CALL CopyCDStringToClient(char **destPtr, const char *src);
	static void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString);
	void PushContraintId(int t){m_constraintIdsTaken[t]=true;}
	void PushRopeId(int t){m_ropeIdsTaken[t]=true;}
	void ShowObjectDetails ();
	void ShowCreateObjectDetails ();
	void ShowGenericProperties ();
	void ShowLightProperties ();
	static void TW_CALL BuildFromArchetype(void *clientData);
	static void TW_CALL CreateDominos(void *clientData);
	static void TW_CALL CreateDominosOnObject(void *clientData);
	static void TW_CALL Rotate90X(void *clientData);
	static void TW_CALL Rotate90Y(void *clientData);
	static void TW_CALL Rotate90Z(void *clientData);
	static void TW_CALL AlighX(void *clientData);
	static void TW_CALL AlighY(void *clientData);
	static void TW_CALL AlighZ(void *clientData);
	static void TW_CALL AlignDistanceX(void *clientData);
	static void TW_CALL AlignDistanceZ(void *clientData);
	static void TW_CALL AlignDistanceY(void *clientData);
	static void TW_CALL MakeSameSize(void *clientData);
	static void TW_CALL SetActivatedBy(const void *value, void *clientData);
	static void TW_CALL GetActivatedBy(void *value, void *clientData);
	static void TW_CALL SetRadius(const void *value, void *clientData);
	static void TW_CALL GetRadius(void *value, void *clientData);
	static void TW_CALL AddContraintComponent(void *clientData);
	static void TW_CALL RemoveContraintComponent(void *clientData);
	static void TW_CALL AddConstaintToObject(void *clientData);
	static void TW_CALL AddConstaint(void *clientData);
	static void TW_CALL SetAllGlowingParentId(const void *value, void *clientData);
	static void TW_CALL GetetAllGlowingParentId(void *value, void *clientData);

	void CreateConstraintObject(Vec3 & worldPos, Vec3 & RelPos, real err, int AttachedToId,bool t,Vec3 mul);
	bool CheckIfAllSelectedAreGlowingCylinders();
	bool Initialized;
	void InitializeTweakBar();
	void InitializeBars();
	TweakBarControl* GetTweakBarControl( const char* s );
	void HideAllBars();
	void ShowAllBars();
	//void SetSelectedGOC(GOC * g);
	void SetSelectedGOCs(std::vector<GOC*>  m_SelectedGOCs);
	void Update();
	int GetNextFreeActivationId();
	int GetNextConstraintId();
	int GetNextRopeId();

};

// A user defined structure

 
// Description of the structure (note that you are not required to describe all members, and that members can be reordered)

// Add elem to bar


}

