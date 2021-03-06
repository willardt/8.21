#ifndef COMPONENT_H
#define COMPONENT_H

#include <memory>
#include <fstream>

#include "../src/Network/Packet.h"

enum
{
	TRANSFORM_COMPONENT,
	TOTAL_COMPONENTS
};

class Entity;

class Component {
public:
	Component(std::shared_ptr<Entity> entity) :
		_entity		(entity)
	{}
	~Component() {}

	virtual std::shared_ptr<Component> copy(std::shared_ptr<Entity> new_entity) const = 0;

	virtual void update() = 0;

	virtual const int get_type() const = 0;

	virtual void save(std::ofstream& file) = 0;

	virtual PacketData packet_data() = 0;

	virtual int load_buffer(void* buf) = 0;

	std::shared_ptr<Entity> _entity;
};

#endif