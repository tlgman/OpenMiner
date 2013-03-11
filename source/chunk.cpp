/*---------------------------------------------------------------------------------
	
	KubKraft
	Copyright (C) 2012 Quent42340 <quent42340@gmail.com>
	
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
---------------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <cmath>

#include <SDL/SDL.h>
#include <GL/glew.h>

#include "types.h"
#include "config.h"
#include "player.h"
#include "cube.h"
#include "chunk.h"
#include "map.h"
#include "game.h"

using namespace std;

Chunk::Chunk(int x, int y, int z, Textures textures) {
	srand(time(NULL));
	
	m_x = x;
	m_y = y;
	m_z = z;
	
	m_texture = textures["stone"];
	
	m_surroundingChunks = new Chunk*[6];
	for (int i = 0; i < 6; i++) m_surroundingChunks[i] = NULL;

	m_vboVertices = 0; glGenBuffers(1, &m_vboVertices);
	m_vboTexCoords = 0; glGenBuffers(1, &m_vboTexCoords);
	m_vboColors = 0; glGenBuffers(1, &m_vboColors);
	m_vboVertexCount = 0;
}

Chunk::~Chunk() {
	glDeleteBuffers(1, &m_vboVertices);
	glDeleteBuffers(1, &m_vboTexCoords);
	glDeleteBuffers(1, &m_vboColors);
	
	for(std::unordered_map<int, Cube*>::iterator it = m_cubes.begin() ; it != m_cubes.end() ; it++) {
		delete it->second;
	}
	
	m_cubes.clear();
	
	delete[] m_surroundingChunks;
}

void Chunk::setSurroundingChunk(unsigned char face, Chunk* chunk) {
	if ((face >= 0) && (face < 6)) {
		m_surroundingChunks[face] = chunk;
	}
}

void Chunk::deleteCube(Cube *cube) {
	for(std::unordered_map<int, Cube*>::iterator it = m_cubes.begin(); it != m_cubes.end() ; it++) {
		if(it->second == NULL) continue;
		if(it->second->x() == cube->x() && it->second->y() == cube->y() && it->second->z() == cube->z()) {
			if(cube->selected()) {
				Game::map->map()[MAP_POS(cube->x(), cube->y(), cube->z())] = 0;
				m_cubes.erase(it);
				refreshVBO();
				if((cube->x() - 1 <= m_x) && (m_surroundingChunks[0] != NULL)) m_surroundingChunks[0]->refreshVBO();
				if((cube->x() + 1 >= m_x + CHUNK_WIDTH - 1) && (m_surroundingChunks[1] != NULL)) m_surroundingChunks[1]->refreshVBO();
				if((cube->y() - 1 <= m_y) && (m_surroundingChunks[2] != NULL)) m_surroundingChunks[2]->refreshVBO();
				if((cube->y() + 1 >= m_y + CHUNK_DEPTH - 1) && (m_surroundingChunks[3] != NULL)) m_surroundingChunks[3]->refreshVBO();
				if((cube->z() + 1 >= m_z + CHUNK_HEIGHT - 1) && (m_surroundingChunks[4] != NULL)) m_surroundingChunks[4]->refreshVBO();
				if((cube->z() - 1 <= m_z) && (m_surroundingChunks[5] != NULL)) m_surroundingChunks[5]->refreshVBO();
				delete cube;
				break;
			}
		}
	}
}

void Chunk::addCube(Cube *selectedCube, int type) {
	int x = selectedCube->x();
	int y = selectedCube->y();
	int z = selectedCube->z();
	
	if((selectedCube == NULL) || (selectedCube->selectedFace() == -1)) {
		m_cubes[CUBE_POS(x, y, z)] = selectedCube;
		return;
	}
	else if(selectedCube->selectedFace() == 0) {
		if((MAP_POS(x, y + 1, z) >= 0) && (MAP_POS(x, y + 1, z) < Game::map->width() * Game::map->depth() * Game::map->height())) {
			Game::map->map()[MAP_POS(x, y + 1, z)] = type;
			m_cubes[CUBE_POS(x, y + 1, z)] = new Cube(x, y + 1, z, m_texture, type);
		}
	}
	else if(selectedCube->selectedFace() == 1) {
		if((MAP_POS(x + 1, y, z) >= 0) && (MAP_POS(x + 1, y, z) < Game::map->width() * Game::map->depth() * Game::map->height())) {
			Game::map->map()[MAP_POS(x + 1, y, z)] = type;
			m_cubes[CUBE_POS(x + 1, y, z)] = new Cube(x + 1, y, z, m_texture, type);
		}
	}
	else if(selectedCube->selectedFace() == 2) {
		if((MAP_POS(x - 1, y, z) >= 0) && (MAP_POS(x - 1, y, z) < Game::map->width() * Game::map->depth() * Game::map->height())) {
			Game::map->map()[MAP_POS(x - 1, y, z)] = type;
			m_cubes[CUBE_POS(x - 1, y, z)] = new Cube(x - 1, y, z, m_texture, type);
		}
	}
	else if(selectedCube->selectedFace() == 3) {
		if((MAP_POS(x, y - 1, z) >= 0) && (MAP_POS(x, y - 1, z) < Game::map->width() * Game::map->depth() * Game::map->height())) {
			Game::map->map()[MAP_POS(x, y - 1, z)] = type;
			m_cubes[CUBE_POS(x, y - 1, z)] = new Cube(x, y - 1, z, m_texture, type);
		}
	}
	else if(selectedCube->selectedFace() == 4) {
		if((MAP_POS(x, y, z + 1) >= 0) && (MAP_POS(x, y, z + 1) < Game::map->width() * Game::map->depth() * Game::map->height())) {
			Game::map->map()[MAP_POS(x, y, z + 1)] = type;
			m_cubes[CUBE_POS(x, y, z + 1)] = new Cube(x, y, z + 1, m_texture, type);
		}
	}
	else if(selectedCube->selectedFace() == 5) {
		if((MAP_POS(x, y, z - 1) >= 0) && (MAP_POS(x, y, z - 1) < Game::map->width() * Game::map->depth() * Game::map->height())) {
			Game::map->map()[MAP_POS(x, y, z - 1)] = type;
			m_cubes[CUBE_POS(x, y, z - 1)] = new Cube(x, y, z - 1, m_texture, type);
		}
	}
	
	refreshVBO();
	if((selectedCube->x() == m_x) && (m_surroundingChunks[0] != NULL)) m_surroundingChunks[0]->refreshVBO();
	if((selectedCube->x() == m_x + CHUNK_WIDTH - 1) && (m_surroundingChunks[1] != NULL)) m_surroundingChunks[1]->refreshVBO();
	if((selectedCube->y() == m_y) && (m_surroundingChunks[2] != NULL)) m_surroundingChunks[2]->refreshVBO();
	if((selectedCube->y() == m_y + CHUNK_DEPTH - 1) && (m_surroundingChunks[3] != NULL)) m_surroundingChunks[3]->refreshVBO();
	if((selectedCube->z() == m_z + CHUNK_HEIGHT - 1) && (m_surroundingChunks[4] != NULL)) m_surroundingChunks[4]->refreshVBO();
	if((selectedCube->z() == m_z) && (m_surroundingChunks[5] != NULL)) m_surroundingChunks[5]->refreshVBO();
}

Cube* Chunk::getCube(int x, int y, int z) {
	if ((x < 0) || (x >= CHUNK_WIDTH) || (y < 0) || (y >= CHUNK_DEPTH) || (z < 0) || (z >= CHUNK_HEIGHT)) {
		return NULL;
	}
	
	int coords = x + (y * CHUNK_WIDTH) + (z * CHUNK_WIDTH * CHUNK_DEPTH);
	Cube *cube = m_cubes[coords];
	if((cube == NULL) || (cube->type() == 0)) {
		m_cubes.erase(coords);
		return NULL;
	}
	else return cube;
}

float getTexOffsetU(int type) {
	return 0;
}

float getTexOffsetV(int type) {
	return 0.875;
}

void Chunk::refreshVBO() {
	m_vboVertexCount = 0;
	
	if (m_cubes.size() == 0) return;
	
	float cubeCoords[6 * 12] = {
		0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1,
		1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1,
		0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1,
		1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1,
		0, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1,
		1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0
	};
	
	unsigned char grey[6] = {159, 191, 127, 223, 255, 95};
	
	std::vector<float> vertices = std::vector<float>();
	std::vector<float> texCoords = std::vector<float>();
	std::vector<unsigned char> colors = std::vector<unsigned char>();
	
	for (int qc = 0 ; qc < CHUNK_WIDTH * CHUNK_DEPTH * CHUNK_HEIGHT ; qc++) {
		int x = qc % CHUNK_WIDTH;
		int y = (qc / CHUNK_WIDTH) % CHUNK_DEPTH;
		int z = (qc / CHUNK_WIDTH) / CHUNK_DEPTH;
		Cube* qe = getCube(x, y, z);
		if (qe == NULL) continue;
		Cube* cube = NULL;
		
		int coords[6 * 3] = {
			x-1, y, z,
			x+1, y, z,
			x, y-1, z,
			x, y+1, z,
			x, y, z+1,
			x, y, z-1
		};
		
		for (int i = 0 ; i < 6 ; i++) {
			cube = getCube(coords[i*3], coords[i*3 + 1], coords[i*3 + 2]);
			
			if ((coords[i*3] < 0) && (m_surroundingChunks[0] != NULL)) {
				cube = m_surroundingChunks[0]->getCube(coords[i*3] + CHUNK_WIDTH, coords[i*3 + 1], coords[i*3 + 2]);
			}
			else if ((coords[i*3] >= CHUNK_WIDTH) && (m_surroundingChunks[1] != NULL)) {
				cube = m_surroundingChunks[1]->getCube(coords[i*3] - CHUNK_WIDTH, coords[i*3 + 1], coords[i*3 + 2]);
			}
			
			if ((coords[i*3 + 1] < 0) && (m_surroundingChunks[2] != NULL)) {
				cube = m_surroundingChunks[2]->getCube(coords[i*3], coords[i*3 + 1] + CHUNK_DEPTH, coords[i*3 + 2]);
			}
			else if ((coords[i*3 + 1] >= CHUNK_DEPTH) && (m_surroundingChunks[3] != NULL)) {
				cube = m_surroundingChunks[3]->getCube(coords[i*3], coords[i*3 + 1] - CHUNK_DEPTH, coords[i*3 + 2]);
			}
			
			if ((coords[i*3 + 2] >= CHUNK_HEIGHT) && (m_surroundingChunks[4] != NULL)) {
				cube = m_surroundingChunks[4]->getCube(coords[i*3], coords[i*3 + 1], coords[i*3 + 2] - CHUNK_HEIGHT);
			}
			else if ((coords[i*3 + 2] < 0) && (m_surroundingChunks[5] != NULL)) {
				cube = m_surroundingChunks[5]->getCube(coords[i*3], coords[i*3 + 1], coords[i*3 + 2] + CHUNK_HEIGHT);
			}
			
			if ((cube == NULL) || (cube->type() == 0)) {
				for(int j = 0 ; j < 4 ; j++) {
					vertices.push_back(x + cubeCoords[(i * 12) + (j * 3)]);
					vertices.push_back(y + cubeCoords[(i * 12) + (j * 3) + 1]);
					vertices.push_back(z + cubeCoords[(i * 12) + (j * 3) + 2]);
					texCoords.push_back(getTexOffsetU(qe->type()) + (cubeCoords[48 + (j * 3)] * 0.125));
					texCoords.push_back(getTexOffsetV(qe->type()) + (cubeCoords[48 + (j * 3) + 1] * 0.125));
					colors.push_back(grey[i]);
					colors.push_back(grey[i]);
					colors.push_back(grey[i]);
					colors.push_back(255);
					
					m_vboVertexCount++;
				}
			}
		}
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoords);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(float), texCoords.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(unsigned char), colors.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	vertices.clear();
	texCoords.clear();
	colors.clear();
}

void Chunk::render() {
	if(m_vboVertexCount == 0) return;
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboVertices);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboTexCoords);
	glTexCoordPointer(2, GL_FLOAT, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboColors);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, 0);
	glDrawArrays(GL_QUADS, 0, m_vboVertexCount);
}
