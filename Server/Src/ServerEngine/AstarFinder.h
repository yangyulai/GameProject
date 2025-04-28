#pragma once
#include <stdint.h>
#define TILESIZE 1			// change this also to reflect tile size. 64x64.
#define MAPDATANOCOPY		//use the pointer for mapdata

//说明
/* 对TileMap来说， 每一位表示一块方各，当位为1表示不可通行， 0:表示可以通行*/

class AstarFinder
{
	struct NODE       // node structure
	{
		long f, h;
		int g, tmpg;
		int x, y;
		int NodeNum;
		NODE* Parent;
		NODE* Child[8];  // a node may have upto 8+(NULL) children.
		NODE* NextNode;  // for filing purposes
	};

	struct STACK        // the stack structure
	{
		NODE*		pNode;
		STACK*		pNextStack;
	};
public:
	AstarFinder(void);
	~AstarFinder();

	bool	InitAstarMap(uint8_t* pMap, int32_t w, int32_t h);
	bool	NewPath(int sx, int sy, int dx, int dy);
	bool	IsReached(void);
	bool	PathNextNode(void);
	int32_t	NodeGetX();
	int32_t	NodeGetY();
	int32_t	GetTileNum(int x, int y);
	int32_t	IsTileAviable(int x, int y); ////为真表示不能通过

private:
	void	FreeNodes(void);
	void	FindPath(int sx, int sy, int dx, int dy);
	NODE*	GetBestNode(void);
	void	GenerateSuccessors(NODE* BestNode, int dx, int dy);
	void	GenerateSucc(NODE* BestNode, int x, int y, int dx, int dy);
	NODE*	CheckOPEN(int tilenum);
	NODE*	CheckCLOSED(int tilenum);
	void	Insert(NODE* Successor);
	void	PropagateDown(NODE* Old);
	void	Push(NODE* Node); // stack functions
	NODE*	Pop(void);

	NODE*	m_pOpenList;    // the node list pointers
	NODE*	m_pClosedList;
	NODE*	m_pCurPath;    // pointer to the best path
	STACK*	m_pStack;

	int		m_nRowCnt;			// tilemap data members, need to be initialisize
	int		m_nColCnt;			// with current map's width and height
	int		m_nTotalTiles;	// to allocate memory for the
	uint8_t*	m_pTileMap;		// pointer to the A* own tilemap data array
};