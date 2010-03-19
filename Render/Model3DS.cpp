/*
 *  Model3DS.cpp
 *  Engine
 *
 *  Created by Brent Wilson on 10/15/05.
 *  Copyright 2005 Brent Wilson. All rights reserved.
 *
 */

#include "Model3DS.h"
#include <Base/FileSystem.h>
#include <Base/Exception.h>
#include <algorithm>

Model3DS::Factory::Factory() {}
Model3DS::Factory::~Factory() {}

bool Model3DS::Factory::canLoad(const std::string &name) {
    std::string ext;
    FileSystem::ExtractExtension(name, ext);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext == "3ds";
}

Model *Model3DS::Factory::load(const std::string &name) {
    THROW(NotImplementedError, "3DS model loading has not been implemented, yet!");
}


//#include <sys/stat.h>
//#include <stdio.h>
//
////>------ Primary chunk
//#define MAIN_CHUNK       0x4D4D
//
////>------ Main Chunks
//#define EDITOR3D_CHUNK   0x3D3D
//#define KEYFRAME_CHUNK   0xB000
//#define MATERIAL_CHUNK   0xAFFF
//
////>------ sub defines of EDITOR3D_CHUNK
//#define OBJECT    0x4000
//#define LIGHT     0x4600
//#define CAMERA    0x4700
//
//#define OBJ_TRIMESH   0x4100
//#define OBJ_VERTS     0x4110
//#define OBJ_FACES     0x4120
//#define OBJ_FACEMATS  0x4130
//#define OBJ_MAPCOORDS 0x4140
//#define OBJ_SMOOTH    0x4150
//#define OBJ_LOCAL     0x4160
//
////>------ sub defines of LIGHT
//#define LIT_OFF       0x4620
//#define LIT_SPOT      0x4610
//#define LIT_UNKNWN01  0x465A
//
////>------ sud defs of MATERIAL_CHUNK
//#define MAT_NAME        0xA000
//#define MAT_AMBIENT     0xA010
//#define MAT_DIFFUSE     0xA020
//#define MAT_SPECULAR    0xA030
//#define MAT_TEX_MAP     0xA200
//#define MAT_REFLECT_MAP 0xA220
//#define MAT_BUMP_MAP    0xA230
//
////>------ sub defs of *_MAP
//#define MAP_FILENAME
//#define MAP_PARAMS
//
////>------ sub defs of KEYFRAME_CHUNK
//#define KEY_MESHINFO   0xB002
//#define KEY_SPOTLIGHT  0xB007
//#define KEY_FRAMES     0xB008
//
////>------ sub defs of KEY_FRAMES
//#define FRM_NAME        0xB010
//#define FRM_PIVOT       0xB013
//#define FRM_POSTRACK    0xB020
//#define FRM_ROTTRACK    0xB021
//#define FRM_SCALESTRACK 0xB022
//#define FRM_HEIERARCHY  0xB030
//
////>------  these define the different color chunk types
//#define COL_RGB    0x0010
//#define COL_24BIT  0x0011
//#define COL_UNK    0x0013
//
//Model3DS::Model3DS():ModelStatic::ModelStatic() {}
//Model3DS::~Model3DS() {}
//
//bool Model3DS::loadModel(const char* directory, const char* model) {
//		  // while there is more to read
//		// read the chunk id
//		// read the chunck length
//		// switch on the chunk id
//			// usefull
//				// read the data
//			// not usefull
//				// scan to current position + chunk length
//	char filename[256];
//	strcpy(filename, directory);
//	strcat(filename, model);
//
//	word chunk_id;	  //2 byte hex number: Chunk ID
//	dword chunk_length;	//4 byte number: Length of data + sub-chunks
//	word num;			 //This is the length of the data
//	word face_flags;	//This holds flags for each polygon (visible|not)
//	byte single_char;	//Used for reading object names
//	FILE *file;			 //Pointer to the .obj File
//	
//	if ((file = fopen(filename, "rb")) == NULL) { //Open the file
//		cerr << "Could not open model file: " << filename << endl;
//		return false;		
//	}
//
//	while (ftell(file) < filelength(fileno(file))) { //Scan the whole file 
//		fread(&chunk_id, 2, 1, file); //Read the chunk header
//		fread(&chunk_length, 4, 1, file); //Read the length of the chunk
//
//		//Debug("ChunkID: %x", chunk_id); 
//		//Debug("ChunkLength: %x", chunk_length);
//
//		switch (chunk_id) {
//			case MAIN_CHUNK:
//			case EDITOR3D_CHUNK:
//			case OBJ_TRIMESH:
//			case MATERIAL_CHUNK:
//				break;
//			case MAT_AMBIENT:
//				
//			case MAT_DIFFUSE:
//			case MAT_SPECULAR:
//				break;
//			
//			case OBJECT:	//3DObject Block
//				//This block contains the object's name
//				//Read up to 20 chars or until the char* is null terminated
//				single_char = 'a'; //So it doesnt default to \0
//				for (int i = 0; i < 20 && single_char != '\0'; i++) {
//					//Read 1 char
//					fread(&single_char, 1, 1, file);
//					name[i] = single_char;
//				}
////				Debug("3DObject: %s", name);
//				break;
//				
//			case OBJ_VERTS:	//Vertices List
//				fread(&num, sizeof(word), 1, file);
//				m_nVertexCount = num;
//				m_vertex = new Vector3[m_nVertexCount];
////				Debug("Number of vertices: %d", num);
//				for (int i = 0; i < num; i++) {
//					fread(&m_vertex[i][0], sizeof(float), 1, file);
//					fread(&m_vertex[i][1], sizeof(float), 1, file);
//					fread(&m_vertex[i][2], sizeof(float), 1, file);
//				}
//			break;
//				
//			case OBJ_FACES:
//				fread(&num, sizeof (word), 1, file);
//				m_nPolygonCount = num;
//				m_polygon = new Poly3[m_nPolygonCount];
////				Debug("Number of polygons: %d",num); 
//				for (int i = 0; i < num; i++) {
//					fread(&m_polygon[i][0], sizeof(word), 1, file);
//					fread(&m_polygon[i][1], sizeof(word), 1, file);
//					fread(&m_polygon[i][2], sizeof(word), 1, file);
//					fread(&face_flags, sizeof(word), 1, file);
//				}
//			break;
//				
//			case OBJ_MAPCOORDS:	//mapping Coords
//				fread(&num, sizeof(word), 1, file);
//				m_texCoord = new Vector2f[num];
//				for (int i = 0; i < num; i++) {
//					fread(&m_texCoord[i][0], sizeof(float), 1, file);
//					fread(&m_texCoord[i][1], sizeof(float), 1, file);
//				}
//			break;
//				
//			//case OBJ_FACEMATS:
//				
//			//break;
//				
//			default:		 //Something we don't care about
//				//-6 because the ID is 2 bytes and the length is 4
//				fseek(file, chunk_length - 6, SEEK_CUR);
//				break;
//		}
//	}
//
//	//m_mat.tex_id = TextureManager::LoadTexture(texname);
//	init();
//	return true; // Returns ok
//}
