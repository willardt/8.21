#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <array>
#include <vector>
#include <unordered_map>
#include <map>
#include <string>

#include <memory>
#include <mutex>

struct GUIIcon;
struct Texture;
struct Program;
class Model;
class Terrain;
class Entity;

/********************************************************************************************************************************************************/

class ProgramManager {
public:
public:
	ProgramManager();
	~ProgramManager();

	std::shared_ptr<Program> get_program(int key);
protected:
	void load_programs();
	bool load_program(int key, std::string_view file_path);
private:

	std::map<int, std::shared_ptr<Program>> _programs;
};

/********************************************************************************************************************************************************/

class MapManager {
public:
	MapManager();
	~MapManager();

	std::shared_ptr<Terrain> get_terrain();
protected:
	void load_map();
	std::shared_ptr<Terrain> _terrain;
private:
};

/********************************************************************************************************************************************************/

class TextureManager {
public:
	TextureManager();
	~TextureManager();
	
	std::shared_ptr<Texture> get_texture(int key);
	std::shared_ptr<GUIIcon> get_icon(int key);

	std::map<int, std::shared_ptr<GUIIcon>>* get_icons();
protected:
	void load_textures();
	bool load_texture(int key, std::string_view file_path);
	bool load_icon(int key, std::string_view file_path);
private:
	std::map<int, std::shared_ptr<Texture>> _textures;
	std::map<int, std::shared_ptr<GUIIcon>> _icons;
};

/********************************************************************************************************************************************************/

class ModelManager {
public:
	ModelManager();
	~ModelManager();

	std::shared_ptr<Model> get_model(int key);
protected:
	void load_models();
	bool load_model(int id, std::string_view file_path);
	std::map<int, std::shared_ptr<Model>> _models;
private:
};

/********************************************************************************************************************************************************/

class EntityManager {
public:
	EntityManager();
	~EntityManager();

	void load_default_entities();

	void update();

	void save_entities(std::string_view folder);
	void load_entities();

	void add_entity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> new_entity(std::string_view type, int id);
	std::shared_ptr<Entity> get_default_entity(std::string_view type, unsigned int id);

	void remove_entity(std::shared_ptr<Entity> entity);

	std::unordered_map<int, std::shared_ptr<Entity>>* get_entities();
protected:
	//std::vector<std::shared_ptr<Entity>> _entities;
	std::unordered_map<int, std::shared_ptr<Entity>> _entities;

	// <Type, <ID, Entity>>
	std::map<std::string, std::map<unsigned int, std::shared_ptr<Entity>>> _default_entities;

	std::mutex _em_mutex;
private:
};

/********************************************************************************************************************************************************/

class ResourceManager : public ProgramManager, public TextureManager, public ModelManager, public MapManager, public EntityManager {
public:
	ResourceManager();
	~ResourceManager();

	void load_resources(bool programs, bool textures, bool models, bool entites, bool map);
	void update();
	void draw();

	void save();
private:
};

/********************************************************************************************************************************************************/

#endif