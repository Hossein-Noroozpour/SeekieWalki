#include "tree-object.hpp"
gearoenix::nufrag::core::TreeObject::TreeObject(TreeObject *p): parent(p) {
    if(p != nullptr) p->add_child(this);
}

gearoenix::nufrag::core::TreeObject::~TreeObject() {
    if(parent != nullptr) parent->remove_child(this);
    while(children.size() != 0) {
        delete *(children.begin());
    }
}

void gearoenix::nufrag::core::TreeObject::add_child(TreeObject *c) {
    children.insert(c);
}

void gearoenix::nufrag::core::TreeObject::remove_child(TreeObject *c) {
    children.erase(c);
}

void gearoenix::nufrag::core::TreeObject::set_parent(TreeObject *p) {
    if(parent != nullptr) parent->remove_child(this);
    if(p != nullptr) p->add_child(this);
    parent = p;
}

gearoenix::nufrag::core::TreeObject *gearoenix::nufrag::core::TreeObject::get_parent() {
    return parent;
}