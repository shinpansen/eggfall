#ifndef editorGroundModule_HPP
#define editorGroundModule_HPP

#include "editor.hpp"

using namespace std;
using namespace sf;

enum REFRESHING_MODE
{
	RECREATE_WRAPPING,
	REFRESH_TILES,
	REFRESH_FIXED_TILES
};

class blockNeighborhood
{
public:
	blockNeighborhood();
	blockNeighborhood(bool left, bool right, bool top, bool bottom);
	bool HasNeighbor();
	bool IsSurrounded();
	bool left;
	bool right;
	bool top;
	bool bottom;
	IntRect leftRect;
	IntRect topRect;
};

//Thread
void TestValidPos(bool* posValid, bool* threadFree, vector<pair<Sprite*, int>> * tiles, vector<pair<Sprite*, int>> * tilesToMove);

class maps;
class editor;
class editorPlatformModule;
class editorGroundModule
{
public:
	editorGroundModule(editor* editor, maps* map, input* input, effects* effectBox);
	void ReloadMapReferences();
	blockNeighborhood GetTileNeighborhood(int numTile, Vector2f customPos = Vector2f(-1, -1));
	int GetInsertPos(Vector2f tilePos);
	IntRect GetTileRect(int numTile);
	vector<pair<Sprite*, int>> GetTiles();
	void SortTiles();
	void RefreshWrappingTiles(REFRESHING_MODE refreshingMode);
	void AddWrappingTile(Vector2f pos, IntRect rect, int parentTileNum, bool shift = true);
	void DeleteMovingTiles();
	int GetTileByPos(Vector2f pos, bool contains = true);
	bool PosIntersectTile(Vector2f pos);
	vector<int> HighlightSelectedTiles(Color color);
	void Step(float deltaTime);
	void Display(RenderWindow* window);
	string SaveGrounds();
private:
	//Variables
	float _groundType;
	float _groundTypeTimer;
	bool _newPosValid;
	bool _threadValidMovePosWorkEnded;
	Thread* _threadValidMovePos;
	vector<int> _selectedTiles;

	//Objects
	editor* _editor;
	maps* _map;
	input* _input;
	effects* _effectBox;

	//Graphics
	Sprite * _spriteGroundType;
	CircleShape _shapeGrassColor;
	Texture* _groundTileTexture;
	Texture _groundTileColorTexture;
	RectangleShape _lastSelectCursor;
	vector<pair<Sprite*, int>> _tiles;
	vector<pair<Sprite*, int>> _wrappingTiles;
	vector<grass*> _grassBlocks;
	vector<pair<Sprite*, int>> _tilesToMove;
	vector<Sprite*> _tilesWrappingToMove;
};
#endif
