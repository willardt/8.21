#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <memory>

#define EDITOR_EDIT_TERRAIN 0
#define EDITOR_PLACE_ENTITY 1
#define EDITOR_SELECT_ENTITY 2

/********************************************************************************************************************************************************/

class KeyPressOnce {
public:
	KeyPressOnce(int glfw_key);

	bool pressed(int key, int action);
private:
	int  _key;
	bool _released;
};

/********************************************************************************************************************************************************/

class Entity;

class EntitySelection {
public:
	EntitySelection();

	void select_all(glm::vec2 first, glm::vec2 last);
	void move_all(double xpos, double ypos);
protected:
	std::vector<std::shared_ptr<Entity>> _entities;
};

/********************************************************************************************************************************************************/

struct GLFWwindow;

class InputManager {
public:
	InputManager();
	~InputManager();

	virtual void update(bool* exit) = 0;

	double get_mouse_x();
	double get_mouse_y();

	glm::vec3 get_mouse_world_space_vector();
	glm::vec3 mouse_world_space_vector(glm::vec2 pos);

	int get_mode();
	void set_mode(int mode);
protected:
	double _xpos;
	double _ypos;

	glm::vec2 _first;
	glm::vec2 _last;

	int _mode;
};

/********************************************************************************************************************************************************/

class GameInputManager : public InputManager, public EntitySelection {
public:
	GameInputManager();
	~GameInputManager();

	void update(bool* exit);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
private:
};

/********************************************************************************************************************************************************/

class EditorInputManager : public InputManager {
public:
	EditorInputManager();
	~EditorInputManager();

	void update(bool* exit);

	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
private:
};

/********************************************************************************************************************************************************/

#endif