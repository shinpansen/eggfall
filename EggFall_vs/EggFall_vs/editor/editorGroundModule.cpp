#include "editorGroundModule.hpp"

static const float SCALE = 30.f;

using namespace std;
using namespace sf;

blockNeighborhood::blockNeighborhood()
{
	this->left = false;
	this->right = false;
	this->top = false;
	this->bottom = false;
}

blockNeighborhood::blockNeighborhood(bool left, bool right, bool top, bool bottom)
{
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
}

bool blockNeighborhood::HasNeighbor()
{
	return left || right || top || bottom;
}

bool blockNeighborhood::IsSurrounded()
{
	return left && right && top && bottom;
}

//Thread
void TestValidPos(bool * posValid, bool* threadFree, vector<pair<Sprite*, int>> * tiles, vector<pair<Sprite*, int>> * tilesToMove)
{
	*posValid = true;
	for (int i = 0; i < tilesToMove->size(); i++)
	{
		//Outside screen
		if ((*tilesToMove)[i].first->getPosition().x < 0 ||
			(*tilesToMove)[i].first->getPosition().x + 32 > utils::VIEW_WIDTH ||
			(*tilesToMove)[i].first->getPosition().y < 0 ||
			(*tilesToMove)[i].first->getPosition().y + 32 > utils::VIEW_HEIGHT)
		{
			*posValid = false;
			break;
		}
		//Intersect other tiles
		for (int j = 0; j < tiles->size(); j++)
		{
			if ((*tiles)[j].first->getColor().a > 140 &&
				(*tilesToMove)[i].first->getGlobalBounds().intersects((*tiles)[j].first->getGlobalBounds()))
			{
				*posValid = false;
				break;
			}
		}
		if (!*posValid) { break; }
	}
	*threadFree = true;
}

editorGroundModule::editorGroundModule(editor* editor, maps* map, input* input, effects* effectBox)
{
	//Variables
	_groundType = 0.f;
	_groundTypeTimer = 0.f;
	_newPosValid = true;
	_threadValidMovePosWorkEnded = true;
	_threadValidMovePos = new Thread(bind(&TestValidPos, &_newPosValid, &_threadValidMovePosWorkEnded, &_tiles, &_tilesToMove));

	//Objects
	_editor = editor;
	_map = map;
	_groundTileTexture = &map->_groundTileTexture;
	_input = input;
	_effectBox = effectBox;

	//Graphics
	_groundTileColorTexture.loadFromImage(utils::ColorizeImage(files::groundTile, Color(255, 255, 255)));
	_spriteGroundType = new Sprite[3];
	for (int i = 0; i < 3; i++)
		_spriteGroundType[i].setTexture(*_groundTileTexture);
	_shapeGrassColor.setRadius(40);
	_shapeGrassColor.setOrigin(100, -20);
	_shapeGrassColor.setOutlineThickness(6);
	_shapeGrassColor.setPosition(utils::VIEW_WIDTH, 0);
	RefreshWrappingTiles(RECREATE_WRAPPING);
}

void editorGroundModule::ReloadMapReferences()
{
	//Delete all
	for (int i = 0; i < _tiles.size(); i++)
		if (_tiles[i].first != NULL)
			delete _tiles[i].first;
	_tiles.clear();
	for (int i = 0; i < _wrappingTiles.size(); i++)
		if (_wrappingTiles[i].first != NULL)
			delete _wrappingTiles[i].first;
	_wrappingTiles.clear();
	for (int i = 0; i < _grassBlocks.size(); i++)
		if (_grassBlocks[i] != NULL)
			delete _grassBlocks[i];
	_grassBlocks.clear();
	DeleteMovingTiles();

	//Recreating tiles and wrapping
	for (int i = 0; i < _map->_tiles.size(); i++)
	{
		pair<Sprite*, int> tilePair;
		tilePair.first = new Sprite(*_map->_tiles[i].first);
		tilePair.second = _map->_tiles[i].second;
		_tiles.push_back(tilePair);
	}
	for (int i = 0; i < _map->_wrappingTiles.size(); i++)
	{
		pair<Sprite*, int> wrappingPair;
		wrappingPair.first = new Sprite(*_map->_wrappingTiles[i].first);
		wrappingPair.second = _map->_wrappingTiles[i].second;
		_wrappingTiles.push_back(wrappingPair);
	}
	
	//Recreating grass
	for (int i = 0; i < _tiles.size(); i++)
		if (!GetTileNeighborhood(i).top)
			_grassBlocks.push_back(new grass(_map->_world, _tiles[i].first, &_map->_groundTileTexture, &_map->_grassClumpsTexture, _tiles[i].second, true, false));
}

blockNeighborhood editorGroundModule::GetTileNeighborhood(int numTile, Vector2f customPos)
{
	//Searching adjacent tiles
	Vector2f tilePos = customPos != Vector2f(-1, -1) ? customPos : _tiles[numTile].first->getPosition();
	tilePos = Vector2f(((int)(tilePos.x / 16.f)) * 16, ((int)(tilePos.y / 16.f)) * 16);
	blockNeighborhood neighborhood;

	//Quick method - only usable if testing a tile and not a custom location
	if (numTile >= 0 && !Keyboard::isKeyPressed(Keyboard::T))
	{
		//Left and right neighbors are necessarily before and after current tile in the list, because tiles are sorted
		if (numTile > 0 && _tiles[numTile - 1].first->getPosition().y == tilePos.y &&
			_tiles[numTile - 1].first->getPosition().x == tilePos.x - 32)
		{
			neighborhood.left = true;
			neighborhood.leftRect = _tiles[numTile - 1].first->getTextureRect();
		}
		if(numTile < _tiles.size() - 1 && _tiles[numTile + 1].first->getPosition().y == tilePos.y &&
			_tiles[numTile + 1].first->getPosition().x == tilePos.x + 32)
			neighborhood.right = true;

		//Top and down neighbors
		for (int i = numTile; i >= 0; i--)
		{
			if (_tiles[i].first->getPosition().y == tilePos.y - 32 && _tiles[i].first->getPosition().x == tilePos.x)
			{
				neighborhood.top = true;
				neighborhood.topRect = _tiles[i].first->getTextureRect();
			}
			if (_tiles[i].first->getPosition().y < tilePos.y - 32) //Sorted list, all following tiles couldn't be neighbor
				break;
		}
		for (int i = numTile; i < _tiles.size(); i++)
		{
			if (_tiles[i].first->getPosition().y == tilePos.y + 32 && _tiles[i].first->getPosition().x == tilePos.x)
				neighborhood.bottom = true;
			if (_tiles[i].first->getPosition().y > tilePos.y + 32)
				break;
		}
	}
	else //Slow method : iterate through every tile 
	{
		for (int i = 0; i < _tiles.size(); i++)
		{
			if (i == numTile) { continue; }
			if (_tiles[i].first->getPosition().x == tilePos.x - 32 && _tiles[i].first->getPosition().y == tilePos.y)
			{
				neighborhood.left = true;
				neighborhood.leftRect = _tiles[i].first->getTextureRect();
			}
			if (_tiles[i].first->getPosition().x == tilePos.x + 32 && _tiles[i].first->getPosition().y == tilePos.y)
				neighborhood.right = true;
			if (_tiles[i].first->getPosition().y == tilePos.y - 32 && _tiles[i].first->getPosition().x == tilePos.x)
			{
				neighborhood.top = true;
				neighborhood.topRect = _tiles[i].first->getTextureRect();
			}
			if (_tiles[i].first->getPosition().y == tilePos.y + 32 && _tiles[i].first->getPosition().x == tilePos.x)
				neighborhood.bottom = true;
			if (neighborhood.IsSurrounded())
				break;
		}
	}
	return neighborhood;
}

int editorGroundModule::GetInsertPos(Vector2f tilePos)
{
	int id = 0;
	while (id < _tiles.size() && _tiles[id].first->getPosition().y < tilePos.y) { id++; }
	if (id < _tiles.size() && _tiles[id].first->getPosition().y == tilePos.y)
		while (id < _tiles.size() && _tiles[id].first->getPosition().y == tilePos.y &&
			_tiles[id].first->getPosition().x < tilePos.x)
			id++;
	return id;
}

IntRect editorGroundModule::GetTileRect(int numTile)
{
	//Adjacent tiles + ground type (y shift down)
	blockNeighborhood neighborhood = GetTileNeighborhood(numTile);
	int yShift = _groundType * 160;
	if (_tiles[numTile].first->getTextureRect().height != 0)
		yShift = (int)(_tiles[numTile].first->getTextureRect().top / 160.f)*160;

	//Choosing write tile regarding neighborhood
	if (!neighborhood.left && !neighborhood.top)
		return IntRect(32, 32 + yShift, 32, 32);
	else if(!neighborhood.right && !neighborhood.top)
		return IntRect(160, 32 + yShift, 32, 32);
	else if (!neighborhood.left && !neighborhood.bottom)
		return IntRect(32, 96 + yShift, 32, 32);
	else if (!neighborhood.right && !neighborhood.bottom)
		return IntRect(160, 96 + yShift, 32, 32);
	else
	{
		int leftRect = 32; 
		if (neighborhood.left && neighborhood.right)
			leftRect = neighborhood.leftRect.left < 128 ? neighborhood.leftRect.left + 32 : 64;
		else if (neighborhood.left && !neighborhood.right)
			leftRect = 160;
		int topRect = 32 + yShift;
		if (neighborhood.top && !neighborhood.left)
			topRect = neighborhood.bottom ? 64 + yShift : 96 + yShift;
		else if (neighborhood.left)
			topRect = neighborhood.leftRect.top + (fabs(neighborhood.leftRect.top - yShift) <= 96 ? 0 : yShift);
		return IntRect(leftRect, topRect, 32, 32);
	}
}

vector<pair<Sprite*, int>> editorGroundModule::GetTiles()
{
	return _tiles;
}

void editorGroundModule::SortTiles()
{
	Vector2f min = Vector2f(9999, 9999);
	int minIterator;
	int nbTiles = _tiles.size();
	vector<pair<Sprite*, int>> tilesSorted;

	//Sort
	while (tilesSorted.size() < nbTiles)
	{
		//Searching for min
		for (int i = 0; i < _tiles.size(); i++)
		{
			if ((_tiles[i].first->getPosition().x <= min.x && _tiles[i].first->getPosition().y <= min.y) ||
				(_tiles[i].first->getPosition().x > min.x && _tiles[i].first->getPosition().y < min.y))
			{
				min = Vector2f(_tiles[i].first->getPosition().x, _tiles[i].first->getPosition().y);
				minIterator = i;
			}
		}
		tilesSorted.push_back(_tiles[minIterator]);
		_tiles.erase(_tiles.begin() + minIterator);
		min = Vector2f(9999, 9999);
	}

	//List merge
	_tiles = tilesSorted;
}

void editorGroundModule::RefreshWrappingTiles(REFRESHING_MODE refreshingMode)
{
	//Deleting grass
	for (int i = 0; i < _grassBlocks.size(); i++)
		delete _grassBlocks[i];
	_grassBlocks.clear();

	//Refreshing tiles texture, rect and color
	vector<blockNeighborhood> neighbors;
	for (int i = 0; i < _tiles.size(); i++)
	{
		//Tile color reset
		_tiles[i].first->setTexture(*_groundTileTexture);
		_tiles[i].first->setTextureRect(GetTileRect(i));
		if (refreshingMode == REFRESH_TILES || (refreshingMode == REFRESH_FIXED_TILES &&
			_tiles[i].first->getColor().a > 140))
			_tiles[i].first->setColor(Color(255, 255, 255));

		//Adding grass
		neighbors.push_back(GetTileNeighborhood(i));
		if (!neighbors[i].top && _editor->_editorMode == EDITOR_ADD)
			_grassBlocks.push_back(new grass(_map->_world, _tiles[i].first, &_map->_groundTileTexture, &_map->_grassClumpsTexture, _tiles[i].second, true, false));
	}

	//Refreshing wrapping tiles texture, rect and color
	if (refreshingMode != RECREATE_WRAPPING)
	{
		for (int i = 0; i < _wrappingTiles.size(); i++)
		{
			_wrappingTiles[i].first->setTexture(*_groundTileTexture);
			if (refreshingMode == REFRESH_TILES || (refreshingMode == REFRESH_FIXED_TILES &&
				_wrappingTiles[i].first->getColor().a > 140))
				_wrappingTiles[i].first->setColor(Color(255, 255, 255));
		}
	}

	if (refreshingMode != RECREATE_WRAPPING)
		return;

	//Deleting wrapping
	for (int i = 0; i < _wrappingTiles.size(); i++)
		if (_wrappingTiles[i].first != NULL)
			delete _wrappingTiles[i].first;
	_wrappingTiles.clear();

	//Recreating wrapping
	for (int i = _tiles.size()-1; i >= 0; i--)
	{
		//Variables
		blockNeighborhood neighborhood = neighbors[i];
		Vector2f pos;
		IntRect rect;

		//Bottom wrapping tiles
		if (!neighborhood.bottom)
		{
			rect = _tiles[i].first->getTextureRect();
			pos = Vector2f(_tiles[i].first->getPosition().x, _tiles[i].first->getPosition().y);
			rect.top = 96;
			rect.height = 64;
			AddWrappingTile(pos, rect, i);
		}
		//Left wrapping tiles
		if (!neighborhood.left)
		{
			rect = _tiles[i].first->getTextureRect();
			rect.left = 0;
			rect.width = 32;
			pos = Vector2f(_tiles[i].first->getPosition().x - 16, _tiles[i].first->getPosition().y);
			if (!neighborhood.top)
			{
				rect.top -= 32;
				rect.height += 32;
				pos.y -= 32;
				if (!neighborhood.bottom)
					AddWrappingTile(Vector2f(pos.x, pos.y + 64), IntRect(0, 128, 32, 32), i, true);
			}
			else if(!neighborhood.bottom)
				rect.height += 32;

			AddWrappingTile(pos, rect, i, false);
		}
		//Right wrapping tiles
		if (!neighborhood.right)
		{
			rect = _tiles[i].first->getTextureRect();
			rect.left = 192;
			rect.width = 32;
			pos = Vector2f(_tiles[i].first->getPosition().x + 16, _tiles[i].first->getPosition().y);
			if (!neighborhood.top)
			{
				rect.top -= 32;
				rect.height += 32;
				pos.y -= 32;
				if (!neighborhood.bottom)
					AddWrappingTile(Vector2f(pos.x, pos.y + 64), IntRect(192, 128, 32, 32), i, true);
			}
			else if (!neighborhood.bottom)
				rect.height += 32;
			AddWrappingTile(pos, rect, i, false);
		}
		//top wrapping tiles
		if (!neighborhood.top)
		{
			rect = _tiles[i].first->getTextureRect();
			pos = Vector2f(_tiles[i].first->getPosition().x, _tiles[i].first->getPosition().y - 32);
			rect.top = 0;
			rect.height = 64;
			AddWrappingTile(pos, rect, i);
		}
	}
}

void editorGroundModule::AddWrappingTile(Vector2f pos, IntRect rect, int parentTileNum, bool shift)
{
	//Ground type(y shift down)
	if (shift)
	{
		int yShift = _groundType * 160;
		if (_tiles[parentTileNum].first->getTextureRect().height != 0)
			yShift = (int)(_tiles[parentTileNum].first->getTextureRect().top / 160.f) * 160;
		rect.top += yShift;
	}
	rect.left += 224;

	//Adding wrapping tile
	pair<Sprite*, int> wrap;
	wrap.first = new Sprite();
	wrap.second = parentTileNum;
	_wrappingTiles.push_back(wrap);
	_wrappingTiles[_wrappingTiles.size() - 1].first->setTexture(*_groundTileTexture);
	_wrappingTiles[_wrappingTiles.size() - 1].first->setTextureRect(rect);
	//_wrappingTiles[_wrappingTiles.size() - 1].first->setColor(Color::Blue);
	_wrappingTiles[_wrappingTiles.size() - 1].first->setPosition(pos);
}

void editorGroundModule::DeleteMovingTiles()
{
	for (int i = 0; i < _tilesToMove.size(); i++)
	{
		if (_tilesToMove[i].first != NULL)
		{
			delete _tilesToMove[i].first;
			_tilesToMove[i].first = NULL;
		}
	}
	_tilesToMove.clear();

	for (int i = 0; i < _tilesWrappingToMove.size(); i++)
	{
		if (_tilesWrappingToMove[i] != NULL)
		{
			delete _tilesWrappingToMove[i];
			_tilesWrappingToMove[i] = NULL;
		}
	}
	_tilesWrappingToMove.clear();
}

int editorGroundModule::GetTileByPos(Vector2f pos, bool contains)
{
	for (int i = 0; i < _tiles.size(); i++)
		if ((contains && _tiles[i].first->getGlobalBounds().contains(pos)) ||
			_tiles[i].first->getPosition() == pos)
			return i;
	return -1;
}

bool editorGroundModule::PosIntersectTile(Vector2f pos)
{
	for (int i = 0; i < _tiles.size(); i++)
		if (_tiles[i].first->getGlobalBounds().intersects(FloatRect(pos.x, pos.y, 32, 32)))
			return true;
	return false;
}

vector<int> editorGroundModule::HighlightSelectedTiles(Color color)
{
	vector<int> selectedTiles;
	for (int i = 0; i < _tiles.size(); i++)
	{
		if (_tiles[i].first->getGlobalBounds().intersects(_editor->_selectCursor.getGlobalBounds()))
		{
			_tiles[i].first->setColor(color);
			selectedTiles.push_back(i);
		}
		else
			_tiles[i].first->setColor(Color::White);
	}
	for (int i = 0; i < _wrappingTiles.size(); i++)
		_wrappingTiles[i].first->setColor(_tiles[_wrappingTiles[i].second].first->getColor());
	return selectedTiles;
}

void editorGroundModule::Step(float deltaTime)
{
	try
	{
		//Editor mode switch
		if (!_editor->_keyPressed && Keyboard::isKeyPressed(Keyboard::S))
		{
			_editor->_keyPressed = true;
			EDITOR_MODE lastEditorMode = _editor->_editorMode;
			_editor->_editorMode = _editor->_editorMode == EDITOR_ADD ? EDITOR_SELECT : EDITOR_ADD;

			if (lastEditorMode == EDITOR_SELECT && _editor->_editorMode == EDITOR_ADD)
			{
				_threadValidMovePos->terminate();
				DeleteMovingTiles();
				RefreshWrappingTiles(REFRESH_TILES);
			}
		}

		//EDITOR ADD
		RectangleShape selectCursor = _editor->_selectCursor;
		if (_editor->_editorMode == EDITOR_ADD)
		{
			//Highlight tiles if deleting
			if (selectCursor.getSize() != Vector2f(0, 0) && selectCursor.getFillColor().r > 0)
			{
				_selectedTiles.clear();
				_selectedTiles = HighlightSelectedTiles(Color(255, 0, 0, 150));
			}

			//ADD - DELETE
			if(selectCursor.getSize() == Vector2f(0,0) && _lastSelectCursor.getSize() != Vector2f(0,0))
			{
				//ADD
				FloatRect areaBounds = _lastSelectCursor.getGlobalBounds();
				if(_lastSelectCursor.getFillColor().g > 0)
				{
					//Inserting tile at sorted position (from top to bottom y coord, left to right x coord)
					int nbBlocksAdded = 0;
					for (int x = areaBounds.left; x < areaBounds.left + areaBounds.width ; x+=32)
					{
						for (int y = areaBounds.top; y < areaBounds.top + areaBounds.height; y += 32)
						{
							//Check tile pos free
							if (PosIntersectTile(Vector2f(x, y)))
								continue;

							//Adding tile
							nbBlocksAdded++;
							int id = 0;
							pair<Sprite*, int> tilePair;
							tilePair.first = new Sprite();
							tilePair.second = _groundType;
							if (_tiles.size() == 0)
								_tiles.push_back(tilePair);
							else
							{
								id = GetInsertPos(Vector2f(x, y));
								if (id < _tiles.size())
								{
									vector<pair<Sprite*, int>>::iterator iterator = _tiles.begin();
									_tiles.insert(iterator + id, tilePair);
								}
								else
									_tiles.push_back(tilePair);
							}
							_tiles[id].first->setTexture(*_groundTileTexture);
							_tiles[id].first->setTextureRect(IntRect(0, 0, 0, 0));
							_tiles[id].first->setPosition(Vector2f(x, y));
						}
					}

					//Adding blocks effect
					if (nbBlocksAdded > 0)
					{
						Vector2f sfxPos = Vector2f(_lastSelectCursor.getPosition().x + _lastSelectCursor.getLocalBounds().width / 2.f,
							_lastSelectCursor.getPosition().y + _lastSelectCursor.getLocalBounds().height / 2.f);
						Vector2f sfxDir[4] = { Vector2f(-1, -1), Vector2f(1, -1), Vector2f(1, 1), Vector2f(-1, 1) };
						for (int i = 0; i < 4; i++)
							_effectBox->AddRectangleShape(drawOption(sfxPos, Vector2f(areaBounds.width, areaBounds.height),
								Vector2f(areaBounds.width / 2.f, areaBounds.height / 2.f),
								Color::Transparent, 0, 255, Color::White, 3), fadeOption(sfxDir[i], 10.f, 0.f, 60.f), physics(false));
					}
				}
				//DELETE
				else if(selectCursor.getSize() == Vector2f(0, 0) && _lastSelectCursor.getSize() != Vector2f(0, 0) &&_selectedTiles.size() > 0)
				{
					vector<pair<Sprite*, int>> cleanTiles;
					for (int i = 0; i < _tiles.size(); i++)
					{
						if (utils::ListContainsInt(_selectedTiles, i))// _tiles[i].first->getGlobalBounds().intersects(areaBounds))
						{
							//Removing blocks effect
							Vector2f sfxPos = Vector2f(_tiles[i].first->getPosition().x + 16.f, _tiles[i].first->getPosition().y + 16.f);
							_effectBox->AddRectangleShape(drawOption(sfxPos, Vector2f(32, 32), Vector2f(16, 16), Color::Red),
								fadeOption(Vector2f(0, 0), 10.f, 0.f, 60.f, 0.f, Vector2f(-0.03f, -0.03f)), physics(false));
							//Deallocate memory
							delete _tiles[i].first;
						}
						else
							cleanTiles.push_back(_tiles[i]);
					}
					_tiles = cleanTiles;
				}
				//Refresh texture rect + wrapping tiles
				if(_lastSelectCursor.getFillColor().g > 0 || (_lastSelectCursor.getFillColor().r > 0 && _selectedTiles.size() > 0))
					RefreshWrappingTiles(RECREATE_WRAPPING);
			}

			//Cursor color - test if bloc is allowed to be added at current location
			if (PosIntersectTile(_editor->_cursor.getPosition()))
				_editor->_cursor.setFillColor(Color(255, 0, 0, 100));
			else if (GetTileNeighborhood(-1, _editor->_cursor.getPosition()).HasNeighbor())
				_editor->_cursor.setFillColor(Color(0, 255, 0, 100));
			else
				_editor->_cursor.setFillColor(Color(0, 0, 255, 100));
		}

		//EDITOR SELECT
		if (_editor->_editorMode == EDITOR_SELECT)
		{
			if (Mouse::isButtonPressed(Mouse::Left) && !Mouse::isButtonPressed(Mouse::Right))
			{
				bool moving = false;
				if (!_editor->_keyPressed && _tilesToMove.size() > 0)
				{
					for(int i=0 ; i<_tilesToMove.size() ; i++)
						if (_tilesToMove[i].first->getGlobalBounds().contains(_editor->_mousePos))
						{
							moving = true;
							_editor->_mouseInitPosMove = Vector2f(((int)(_editor->_mousePos.x / 16.f)) * 16, ((int)(_editor->_mousePos.y / 16.f)) * 16);
							_editor->_editorMode = EDITOR_MOVE;
							break;
						}
				}
				_editor->_keyPressed = true;
				if (!moving)
				{
					if (selectCursor.getSize() != Vector2f(0, 0)) //Selection init
					{
						//Select - add to moving tiles list
						vector<int> selectedTiles = HighlightSelectedTiles(Color(200, 200, 200, 140));
						DeleteMovingTiles();
						for (int i = 0; i < selectedTiles.size(); i++)
						{
							pair<Sprite*, int> tileToMovePair;
							tileToMovePair.first = new Sprite(*_tiles[selectedTiles[i]].first);
							tileToMovePair.first->setTexture(_groundTileColorTexture);
							tileToMovePair.first->setColor(Color(0, 255, 0, 140));
							tileToMovePair.second = _tiles[selectedTiles[i]].second;
							_tilesToMove.push_back(tileToMovePair);
							for (int j = 0; j < _wrappingTiles.size(); j++)
							{
								if (_wrappingTiles[j].second == selectedTiles[i])
								{
									_tilesWrappingToMove.push_back(new Sprite(*_wrappingTiles[j].first));
									_tilesWrappingToMove[_tilesWrappingToMove.size() - 1]->setTexture(_groundTileColorTexture);
									_tilesWrappingToMove[_tilesWrappingToMove.size() - 1]->setColor(Color(0, 255, 0, 140));
								}
							}
						}
					}
				}
			}
		}

		//EDITOR MOVE
		if (_editor->_editorMode == EDITOR_MOVE)
		{
			Vector2f tileNewPos = Vector2f(((int)(_editor->_mousePos.x / 16.f)) * 16, ((int)(_editor->_mousePos.y / 16.f)) * 16);
			if (tileNewPos != _editor->_mouseInitPosMove)
			{
				//Moving
				for (int i = 0; i < _tilesToMove.size(); i++)
					_tilesToMove[i].first->move(tileNewPos.x - _editor->_mouseInitPosMove.x, tileNewPos.y - _editor->_mouseInitPosMove.y);
				for (int i = 0; i < _tilesWrappingToMove.size(); i++)
					_tilesWrappingToMove[i]->move(tileNewPos.x - _editor->_mouseInitPosMove.x, tileNewPos.y - _editor->_mouseInitPosMove.y);
				_editor->_mouseInitPosMove = tileNewPos;

				//Testing if new position is valid (i.e. doesn't intersect other tiles)
				if (_threadValidMovePosWorkEnded)
				{
					_threadValidMovePosWorkEnded = false;
					_threadValidMovePos->launch();
				}
			}

			//Coloration : green = move valid - red = move invalid
			Color colorMove = _newPosValid ? Color(0, 255, 0, 150) : Color(255, 0, 0, 150);
			for (int i = 0; i < _tilesToMove.size(); i++)
				_tilesToMove[i].first->setColor(colorMove);
			for (int i = 0; i < _tilesWrappingToMove.size(); i++)
				_tilesWrappingToMove[i]->setColor(colorMove);
		}

		//Ground type change UP/DOWN
		if (!_editor->_keyPressed && (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::Down)))
		{
			_editor->_keyPressed = true;
			_groundTypeTimer = 40.f;
			if (Keyboard::isKeyPressed(Keyboard::Up))
				_groundType += _groundType > 0 ? -1 : 3;
			else if (Keyboard::isKeyPressed(Keyboard::Down))
				_groundType += _groundType < 3 ? 1 : -3;
		}
		_groundTypeTimer = utils::StepCooldown(_groundTypeTimer, 1.f, deltaTime);

		//Click released - move ends
		if (!_editor->_keyPressed && _editor->_editorMode == EDITOR_MOVE)
		{
			//Sleep while parallel thread is calculating if new pos is valid
			while (!_threadValidMovePosWorkEnded)
			{
				Time time;
				sleep(time);
			}
			if (_newPosValid)
			{
				//Adding moved tiles in main list - ignoring old tiles
				for (int i = 0; i < _tiles.size(); i++)
				{
					if (_tiles[i].first->getColor().a > 140)
						_tilesToMove.push_back(_tiles[i]);
					else if(_tiles[i].first != NULL)
					{
						delete _tiles[i].first;
						_tiles[i].first = NULL;
					}
				}
					
				//Recreating tiles list
				_tiles = _tilesToMove;
				SortTiles();
				RefreshWrappingTiles(RECREATE_WRAPPING);
			}
			_tilesToMove.clear();
			for (int i = 0; i < _tilesWrappingToMove.size(); i++)
				if (_tilesWrappingToMove[i] != NULL)
					delete _tilesWrappingToMove[i];
			_tilesWrappingToMove.clear();

			//Color reset
			RefreshWrappingTiles(REFRESH_TILES);
			_editor->_editorMode = EDITOR_SELECT;
		}

		//Last select cursor update
		_lastSelectCursor = _editor->_selectCursor;
	}
	catch (exception e)
	{
		cout << e.what() << endl;
	}
}

void editorGroundModule::Display(RenderWindow * window)
{
	//Drawing Tiles blocks
	for (int i = 0; i < _tiles.size(); i++)
		if (_tiles[i].first != NULL)
			window->draw(*_tiles[i].first);
	for (int i = 0; i < _wrappingTiles.size(); i++)
		if (_wrappingTiles[i].first != NULL)
			utils::DrawLoopSprite(window, _wrappingTiles[i].first);
	if (_editor->_editorMode == EDITOR_ADD || _editor->_editorModule != EDITOR_GROUND_MODULE)
		for (int i = 0; i < _grassBlocks.size(); i++)
			if (_grassBlocks[i] != NULL)
				_grassBlocks[i]->DisplayGrass(window);

	//Drawing moving block
	for (int i = 0; i < _tilesToMove.size(); i++)
		if(_tilesToMove[i].first != NULL)
			window->draw(*_tilesToMove[i].first);
	for (int i = 0; i < _tilesWrappingToMove.size(); i++)
		if(_tilesWrappingToMove[i] != NULL)
			window->draw(*_tilesWrappingToMove[i]);

	//Showing ground type
	if (_groundTypeTimer > 0.f)
	{
		for (int i = 0; i < 3; i++)
		{
			_spriteGroundType[i].setTexture(*_groundTileTexture);
			_spriteGroundType[i].setTextureRect(IntRect(i*224, _groundType * 160, 224, 160));
			_spriteGroundType[i].setColor(Color(255, 255, 255, (_groundTypeTimer > 15.f ? 255 : _groundTypeTimer * 17)));
			window->draw(_spriteGroundType[i]);
		}
	}
}

string editorGroundModule::SaveGrounds()
{
	//Writing file
	stringstream mapFile;
	mapFile << "<ground>" << endl;

	//Saving tiles
	for (int i = 0; i < _tiles.size(); i++)
	{
		//Useful variables
		blockNeighborhood neighbor = GetTileNeighborhood(i);
		bool leftGrass = false, rightGrass = false;
		if (!neighbor.left || (i > 0 && neighbor.left && GetTileNeighborhood(i - 1).top))
			leftGrass = true;
		if (!neighbor.right || (i < _tiles.size()-1 && neighbor.right && GetTileNeighborhood(i + 1).top))
			rightGrass = true;

		//Writing stream
		mapFile << "\t<tile top=\"" << _tiles[i].first->getTextureRect().top << "\" ";
		mapFile << "left=\"" << _tiles[i].first->getTextureRect().left << "\" ";
		mapFile << "width=\"" << _tiles[i].first->getTextureRect().width << "\" ";
		mapFile << "height=\"" << _tiles[i].first->getTextureRect().height << "\" ";
		mapFile << "x=\"" << _tiles[i].first->getPosition().x << "\" ";
		mapFile << "y=\"" << _tiles[i].first->getPosition().y << "\" ";
		mapFile << "hook=\"" << (!neighbor.top && (!neighbor.left || !neighbor.right)) << "\" ";
		mapFile << "grass=\"" << !neighbor.top << "\" ";
		mapFile << "leftGrass=\"" << leftGrass << "\" ";
		mapFile << "rightGrass=\"" << rightGrass << "\" ";
		mapFile << "surrounded=\"" << neighbor.IsSurrounded() << "\" ";
		mapFile << "type=\"" << _tiles[i].second << "\"></tile>" << endl;
	}
	//Saving wrapping
	for (int i = 0; i < _wrappingTiles.size(); i++)
	{
		mapFile << "\t<wrapping parent=\"" << _wrappingTiles[i].second << "\" ";
		mapFile << "top=\"" << _wrappingTiles[i].first->getTextureRect().top << "\" ";
		mapFile << "left=\"" << _wrappingTiles[i].first->getTextureRect().left << "\" ";
		mapFile << "width=\"" << _wrappingTiles[i].first->getTextureRect().width << "\" ";
		mapFile << "height=\"" << _wrappingTiles[i].first->getTextureRect().height << "\" ";
		mapFile << "x=\"" << _wrappingTiles[i].first->getPosition().x << "\" ";
		mapFile << "y=\"" << _wrappingTiles[i].first->getPosition().y << "\"></wrapping>" << endl;
	}

	mapFile << "</ground>" << endl;
	return mapFile.str();
}
