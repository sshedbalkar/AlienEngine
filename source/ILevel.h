#ifndef ILEVEL_H
#define ILEVEL_H
//
namespace Framework
{
	enum GAME_STATE
	{
		LEVEL_RESTART,
		LEVEL_QUIT
	};
	//
	class ILevel
	{
	public:
		virtual int load(void) = 0;
		virtual int initialize(void) = 0;
		virtual int update(float dt) = 0;
		virtual int draw(void) = 0;
		virtual int free(void) = 0;
		virtual int unload(void) = 0;
		virtual int id(void) = 0;
		virtual int restart(void) = 0;
	};
	//
}
//
#endif