#include "crafter.h"

void Crafter::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_main_station"), &Crafter::get_main_station);
	ClassDB::bind_method(D_METHOD("set_main_station", "main_station"), &Crafter::set_main_station);
	ClassDB::bind_method(D_METHOD("get_inventories_for_stations"), &Crafter::get_inventories_for_stations);
	ClassDB::bind_method(D_METHOD("set_inventories_for_stations", "inventories_for_stations"), &Crafter::set_inventories_for_stations);
	ClassDB::bind_method(D_METHOD("get_opened_stations"), &Crafter::get_opened_stations);
	ClassDB::bind_method(D_METHOD("set_opened_stations", "opened_stations"), &Crafter::set_opened_stations);
	ClassDB::bind_method(D_METHOD("open", "craft_station"), &Crafter::open);
	ClassDB::bind_method(D_METHOD("close", "craft_station"), &Crafter::close);
	ClassDB::bind_method(D_METHOD("is_open_main_craft_station"), &Crafter::is_open_main_craft_station);
	ClassDB::bind_method(D_METHOD("is_open_any_station"), &Crafter::is_open_any_station);
	ClassDB::bind_method(D_METHOD("is_open", "craft_station"), &Crafter::is_open);
	ClassDB::bind_method(D_METHOD("open_main_craft_station"), &Crafter::open_main_craft_station);
	ClassDB::bind_method(D_METHOD("close_main_craft_station"), &Crafter::close_main_craft_station);
	ClassDB::bind_method(D_METHOD("close_all_craft_stations"), &Crafter::close_all_craft_stations);
	ADD_SIGNAL(MethodInfo("opened", PropertyInfo(Variant::NODE_PATH, "craft_station", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "CraftStation")));
	ADD_SIGNAL(MethodInfo("closed"));
	ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "main_station", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "CraftStation"), "set_main_station", "get_main_station");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "inventories_for_stations", PROPERTY_HINT_ARRAY_TYPE, vformat("%s/%s:%s", Variant::NODE_PATH, PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Inventory")), "set_inventories_for_stations", "get_inventories_for_stations");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "opened_stations", PROPERTY_HINT_ARRAY_TYPE, vformat("%s/%s:%s", Variant::NODE_PATH, PROPERTY_HINT_NODE_PATH_VALID_TYPES, "CraftStation")), "set_opened_stations", "get_opened_stations");
}

Crafter::Crafter() {
}

Crafter::~Crafter() {
}

void Crafter::set_main_station(const NodePath &new_main_station) {
	main_station = new_main_station;
}

NodePath Crafter::get_main_station() const {
	return main_station;
}

TypedArray<NodePath> Crafter::get_inventories_for_stations() const {
	return inventories_for_stations;
}

void Crafter::set_inventories_for_stations(const TypedArray<NodePath> &new_inventories_for_stations) {
	inventories_for_stations = new_inventories_for_stations;
}

void Crafter::set_opened_stations(const TypedArray<NodePath> &new_opened_stations) {
	opened_stations = new_opened_stations;
}

TypedArray<NodePath> Crafter::get_opened_stations() const {
	return opened_stations;
}

bool Crafter::open(CraftStation *craft_station) {
	NodePath craft_station_path = get_path_to(craft_station);
	if (opened_stations.has(craft_station_path))
		return false;
	if (!craft_station->open())
		return false;
	if (craft_station_path != main_station) {
		for (size_t i = 0; i < inventories_for_stations.size(); i++) {
			craft_station->add_input_inventory(get_input_inventory_for_stations(inventories_for_stations[i]));
		}
	}
	opened_stations.append(craft_station_path);
	emit_signal("opened", craft_station);
	return true;
}

bool Crafter::close(CraftStation *craft_station) {
	NodePath craft_station_path = get_path_to(craft_station);
	uint32_t index = opened_stations.find(craft_station_path);
	if (index == -1)
		return false;
	if (!craft_station->close()) {
		return false;
	}
	if (get_path_to(craft_station) != main_station) {
		for (size_t i = 0; i < inventories_for_stations.size(); i++) {
			craft_station->remove_input_inventory(get_input_inventory_for_stations(inventories_for_stations[i]));
		}
	}
	opened_stations.remove_at(index);
	emit_signal("closed", craft_station);
	return true;
}

bool Crafter::is_open_main_craft_station() const {
	CraftStation *craft_station = get_craft_station(main_station);
	if (craft_station == nullptr) {
		ERR_PRINT("Passed object is not a CraftStation!");
		return false;
	}
	return is_open(craft_station);
}

bool Crafter::is_open_any_station() const {
	return opened_stations.size() > 0;
}

bool Crafter::is_open(const CraftStation *craft_station) const {
	return craft_station->get_is_open();
}

bool Crafter::open_main_craft_station() {
	CraftStation *craft_station = get_craft_station(main_station);
	if (craft_station == nullptr) {
		ERR_PRINT("Passed object is not a CraftStation!");
		return false;
	}
	return open(craft_station);
}

bool Crafter::close_main_craft_station() {
	CraftStation *craft_station = get_craft_station(main_station);
	if (craft_station == nullptr) {
		ERR_PRINT("Passed object is not a CraftStation!");
		return false;
	}
	return close(craft_station);
}

void Crafter::close_all_craft_stations() {
	int opened_stations_size = opened_stations.size();
	for (size_t i = opened_stations_size; i > 0; i--) {
		NodePath station_path = opened_stations[i - 1];
		CraftStation *craft_station = get_craft_station(station_path);
		if (craft_station == nullptr) {
			ERR_PRINT("Passed object is not a CraftStation!");
			opened_stations.remove_at(i - 1);
			continue;
		}
		close(craft_station);
	}
}

CraftStation *Crafter::get_craft_station(const NodePath craft_station_path) const {
	Node *node_station = get_node_or_null(craft_station_path);
	CraftStation *craft_station = Object::cast_to<CraftStation>(node_station);
	if (craft_station == nullptr) {
		ERR_PRINT("Get Craft Station is null!");
	}
	return craft_station;
}

Inventory *Crafter::get_input_inventory_for_stations(const NodePath inventory_path) const {
	Node *node_inventory = get_node_or_null(inventory_path);
	Inventory *inventory = Object::cast_to<Inventory>(node_inventory);
	if (inventory == nullptr) {
		ERR_PRINT("Get Input Inventory is null!");
	}
	return inventory;
}
