#include "ResourceManager.h"

#include "../src/Utility/FileReader.h"
#include "../src/Resources/Program.h"
#include "../src/Resources/Model.h"

#include "../src/System/Environment.h"
#include "../src/Resources/Window.h"
#include "../src/Resources/Camera.h"

#include "../src/Network/Client.h"

#include "../src/Resources/Terrain.h"
#include "../src/Entities/Entity.h"

#include "../src/Resources/Icon.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#define SHADER_FILE "Data\\Shaders\\shaders.txt"
#define MODEL_FILE "Data\\Models\\models.txt"
#define ENTITY_FILE "Data\\Entities\\entities.txt"
#define TEXTURE_FILE "Data\\Textures\\textures.txt"

/********************************************************************************************************************************************************/

ProgramManager::ProgramManager()
{}

ProgramManager::~ProgramManager()
{}

void ProgramManager::load_programs() {
	FileReader file(SHADER_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Program -- " << itt->value << '\n';
			load_program(itt->key_val, itt->value);
		}
	}
}

bool ProgramManager::load_program(int key, std::string_view file_path) {
	if(_programs.count(key)) {
		std::cout << "Duplicate Program Key -- " << key << '\n';
		return false;
	}

	const auto program = std::make_shared<Program>(key, file_path.data());
	_programs[key] = program;

	Environment::get().get_window()->get_camera()->attach_shader(program->_id);

	return true;
}

std::shared_ptr<Program> ProgramManager::get_program(int key) {
	return _programs.at(key);
}

/********************************************************************************************************************************************************/

TextureManager::TextureManager()
{}

TextureManager::~TextureManager()
{}

void TextureManager::load_textures() {
	FileReader file(TEXTURE_FILE, FileReader::int_val);

	if (!file.is_read()) {
		//...
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {

			if (it->section == "Texture") {
				std::cout << "Load Texture -- " << itt->value << '\n';
				load_texture(itt->key_val, itt->value);
			}
			
			if(it->section == "Icons") {
				std::cout << "Load Icon -- " << itt->value << '\n';
				load_icon(itt->key_val, itt->value);
			}
		}
	}
}

bool TextureManager::load_texture(int key, std::string_view file_path) {
	if (_textures.count(key)) {
		std::cout << "Duplicate Texture Key -- " << key << '\n';
		return false;
	}

	const auto texture = std::make_shared<Texture>(key, file_path.data());
	_textures[key] = texture;

	return true;
}

std::shared_ptr<Texture> TextureManager::get_texture(int key) {
	return _textures.at(key);
}

bool TextureManager::load_icon(int key, std::string_view file_path) {
	if (_icons.count(key)) {
		std::cout << "Duplicate Icon Key -- " << key << '\n';
		return false;
	}

	const auto icon = std::make_shared<GUIIcon>(key, file_path.data());
	_icons[key] = icon;

	return true;
}

std::shared_ptr<GUIIcon> TextureManager::get_icon(int key) {
	return _icons.at(key);
}

std::map<int, std::shared_ptr<GUIIcon>>* TextureManager::get_icons() {
	return &_icons;
}

/********************************************************************************************************************************************************/

MapManager::MapManager()
{}

MapManager::~MapManager()
{}

void MapManager::load_map() {
	//_terrain = std::make_shared<Terrain>(100, 100, 1.0f, 1.0f);

	FileReader file("Data\\Map\\map.txt");
	TerrainData terrain_data;
	terrain_data.load(file);
	_terrain = std::make_shared<Terrain>(std::move(terrain_data));
}

std::shared_ptr<Terrain> MapManager::get_terrain() {
	return _terrain;
}

/********************************************************************************************************************************************************/

ModelManager::ModelManager()
{}

ModelManager::~ModelManager()
{}

void ModelManager::load_models() {
	FileReader file(MODEL_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	for(auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Model " << itt->value << '\n';
			load_model(itt->key_val, itt->value);
		}
	}
}

bool ModelManager::load_model(int id, std::string_view file_path) {
	if (_models.count(id)) {
		std::cout << "Duplicate Model ID -- " << id << '\n';
		return false;
	}

	FileReader file(file_path.data());
	if (!file.is_read()) {
		return false;
	}

	const auto model = std::make_shared<Model>(id, file_path);
	_models[id] = model;

	return true;
}

std::shared_ptr<Model> ModelManager::get_model(int key) {
	return _models.at(key);
}

/********************************************************************************************************************************************************/

EntityManager::EntityManager()
{}

EntityManager::~EntityManager()
{}

void EntityManager::load_default_entities() {
	FileReader file(ENTITY_FILE, FileReader::int_val);

	if(!file.is_read()) {
		//...
		assert(NULL);
	}

	for (auto it = file.begin(); it != file.end(); ++it) {
		for(auto itt = it->table.begin(); itt != it->table.end(); ++itt) {
			std::cout << "Load Entity: " << itt->value << '\n';

			if(_default_entities[it->section].count(itt->key_val)) {
				std::cout << "Duplicate Entity ID " << '\n';
			}

			const auto entity = std::make_shared<Entity>(it->section, itt->key_val);
			entity->load();
			_default_entities[it->section][itt->key_val] = entity;
		}
	}

}

void EntityManager::update() {
	auto it = _entities.begin();
	while (it != _entities.end()) {
		if ((it->second)->get_destroy()) {
			it = _entities.erase(it);
			continue;
		}

		(it->second)->update();
		++it;
	}
}

void EntityManager::save_entities(std::string_view folder) {
	std::filesystem::remove_all(folder);
	std::filesystem::create_directory(folder);

	for(const auto& e : _entities) {
		std::ofstream file;
		file.open(folder.data() + std::to_string(e.second->get_unique_id()) + ".txt", std::ios::in | std::ios::trunc);
		e.second->save(file);
	}
}

void EntityManager::load_entities() {
	for(auto& p : std::filesystem::directory_iterator("Data\\Map\\Entities")) {
		auto entity = std::make_shared<Entity>();
		entity->load(p.path().string());
		_entities.insert({ entity->get_unique_id(), entity });
	}
}

void EntityManager::add_entity(std::shared_ptr<Entity> entity) {
	std::lock_guard<std::mutex> lock(_em_mutex);
	if(_entities.count(entity->get_unique_id())) {
		assert(NULL);
	}
	_entities.insert({ entity->get_unique_id(), entity });
}

std::shared_ptr<Entity> EntityManager::new_entity(std::string_view type, int id) {
	std::lock_guard<std::mutex> lock(_em_mutex);
	const auto default_entity = _default_entities.at(type.data()).at(id);
	const auto new_entity = std::make_shared<Entity>(*default_entity);
	new_entity->copy(*default_entity);

	Environment::get().get_client()->s_new_entity(new_entity); // send to server

	//_entities.insert({ new_entity->get_unique_id(), new_entity });

	return new_entity;
}

void EntityManager::remove_entity(std::shared_ptr<Entity> entity) {
	std::lock_guard<std::mutex> lock(_em_mutex);
	for(auto it = _entities.begin(); it != _entities.end(); ++it) {
		if((it->second) == entity) {
			_entities.erase(it);
			return;
		}
	}
}

std::shared_ptr<Entity> EntityManager::get_default_entity(std::string_view type, unsigned int id) {
	std::lock_guard<std::mutex> lock(_em_mutex);
	return _default_entities.at(type.data()).at(id);
}

std::unordered_map<int, std::shared_ptr<Entity>>* EntityManager::get_entities() {
	return &_entities;
}

/********************************************************************************************************************************************************/

ResourceManager::ResourceManager()
{}

ResourceManager::~ResourceManager() 
{}

void ResourceManager::load_resources(bool programs, bool textures, bool models, bool entities, bool map) {
	if(programs)	load_programs();
	if(textures)	load_textures();
	if(models)		load_models();
	if(entities)	load_default_entities();
	if(map)			load_map();
	if (Environment::get().get_mode() == MODE_EDITOR) {
		load_entities();
	}
}

void ResourceManager::update() {
	EntityManager::update();
}

void ResourceManager::draw() {
	const auto mode = Environment::get().get_mode();
	_terrain->draw(GL_TRIANGLES, mode == MODE_EDITOR);

	std::lock_guard<std::mutex> lock(_em_mutex);
	for(const auto entity : _entities) {
		if (const auto transform = entity.second->get<TransformComponent>()) {
			_models.at(entity.second->get_model_id())->draw(transform->_transform);
		}
	}
}

void ResourceManager::save() {
	std::ofstream file;
	file.open("Data\\Map\\map.txt", std::ios::out | std::ios::trunc);

	if (!file.is_open()) {
		std::cout << "save()" << '\n';
		std::cout << "Couldn't open file Data\\Map\\map.txt" << '\n';
		return;
	}

	_terrain->save(file);

	save_entities("Data\\Map\\Entities\\");

	file.close();
}

/********************************************************************************************************************************************************/