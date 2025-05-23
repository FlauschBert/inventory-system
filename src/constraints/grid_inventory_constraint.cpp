#include "grid_inventory_constraint.h"

void GridInventoryConstraint::_bind_methods() {
    GDVIRTUAL_BIND(_can_add_on_position, "inventory", "position", "item_id", "amount", "properties", "is_rotated");
}

GridInventoryConstraint::GridInventoryConstraint() {
}

GridInventoryConstraint::~GridInventoryConstraint() {
}

bool GridInventoryConstraint::can_add_on_position(const Node* inventory_node, const Vector2i position, const String item_id, const int amount, const Dictionary properties, const bool is_rotated) {
    bool ret;
    if (GDVIRTUAL_CALL(_can_add_on_position, inventory_node, position, item_id, amount, properties, is_rotated, ret)) {
		return ret;
	}
	return true;
}