#pragma once
#include <set>
namespace gearoenix {
    namespace nufrag {
        namespace core {
            class TreeObject {
            protected:
                std::set<TreeObject *> children;
                TreeObject *parent;
            public:
                TreeObject(TreeObject *p);
                virtual ~TreeObject();
                void add_child(TreeObject *c);
                void remove_child(TreeObject *c);
                void set_parent(TreeObject *p);
                TreeObject *get_parent();
            };
        }
    }
}