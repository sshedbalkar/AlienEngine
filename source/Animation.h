///////////////////////////////////////////////////////////////////////////////////////
//
//	Animation.hpp
//  Defines all classes need for skeletal animation and their support structures.
//	Animation - Keyframes and tracks for animation.
//	Skeleton - Bone Node Graph and bone space matrices.
//	AnimationController - Controls the animation for a animated model by tracking time and
//		using an animation and skeleton to generate a matrix buffer.
//	
//	Authors: 
//	Copyright 2012, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Asset.hpp"
#include "GraphicsVMath.h"
namespace Framework{
//A Keyframe is a snapshot of a node / bone
//at a point in time.
struct KeyFrame
{
	float time;
	Vec3 T;
	D3DXQUATERNION R;
};

//A Track is a set of keyframes that are in temporal
//order from 0 to the animation duration
struct Track
{
	std::vector< KeyFrame > KeyFrames;
};

//Track data is used to help process the animation in this
//case it is the last keyframe the track was on
struct TrackData
{
	unsigned int LastKey;
};
typedef std::vector<TrackData> TrackBuffer;

//The matrix buffer is the result of processing an animation and
//what is need to render the animated mesh
typedef std::vector<D3DXMATRIX> MatrixBuffer;

class ChunkReader;
//A Bone has two primary functions. It defines the parent/child
//relationships between animation tracks and stores the model
//to bone space transform used to perform vertex skinning.
struct Bone
{
	std::string Name;
	int BoneIndex;
	int ParentBoneIndex;

	//Not all the following data is needed but is
	//helpful to debug and understand how skinning works

	//Translation of bone in bind position
	Vec3 BindT;
	//Rotation of bone in bind position
	D3DXQUATERNION BindR;
	//Transform of the bone in bind position
	//Matrix4 BindTransform;

	//Transform from model space to bone space
	//this is the inverse of the BindTransform
	D3DXMATRIX ModelToBoneSpace;

	//Inverse Translation of bone in bind position
	Vec3 ModelToBoneSpaceT;
	//Inverse Rotation of bone in bind position
	D3DXQUATERNION ModelToBoneSpaceR;

	std::vector<Bone*> Children;
};

//Result of interpolating an animation track. Stored in a struct
//so when new data is added to the animation all the function declarations
//do not need to be updated.
struct AnimTransform
{
	Vec3 T;
	D3DXQUATERNION R;
	D3DXMATRIX GetMatrix();
};

//The animation stores the keyframes for each track and interpolate between them.
//It does not contain the graph data (parent/child) or match tracks to individual 
//transform nodes/bones (the skeleton contains this information). 
//This is because their can be many animations related to one skeleton.
class Animation : public Asset
{
public:
	float Duration;
	std::vector< Track > Tracks;
	void CalculateTransform(float animTime,unsigned int trackIndex,AnimTransform& atx,TrackData& data);
	void ReadFrom(ChunkReader& file);
};

//The skeleton contains the bones which have the graph (parent/child) relationships and what animation tracks
//correspond to what bones.
class Skel : public Asset
{
public:
	void Initialize();
	void ReadFrom(ChunkReader& file);
	void ProcessAnimationGraph(float time, MatrixBuffer& matrixBuffer, Animation& anim,TrackBuffer& trackData) const;
	//void RecursiveProcess(float time,Bone& bone,Animation& anim,MatrixBuffer& matrixBuffer,TrackBuffer& trackData,Matrix4 matrix);
	void ProcessBindPose(MatrixBuffer& buffer) const;
	D3DXMATRIX& GetModelToBoneSpaceTransform(int boneIndex);
	std::vector<Bone> bones;
	std::vector<Bone*> RootBones;
};

//Controls the animation for a animated model by tracking time and
//using an animation and skeleton to generate a matrix buffer.
class AnimationContent
{
public:
	AnimationContent();
	~AnimationContent();
	
	Skel * skel;
	std::vector<Animation*> Animations;

	void SetSkel(Skel * skel);
	void AddAnimation(Animation * anim);
};

//Controls the animation for a animated model by tracking time and
//using an animation and skeleton to generate a matrix buffer.
class AnimationController
{
public:
	AnimationController( );
	AnimationController( const AnimationContent* const _ac );
	~AnimationController();

	float animTime;
	float animSpeed;
	Animation* ActiveAnim;

	bool animPause;

	MatrixBuffer BoneMatrixBuffer;
	TrackBuffer AnimTrackData;

	void Process( const Skel* const _skel );
	void ProcessBindPose( const Skel* const _skel );
	void Update(float dt);
};
}