//
// CS488 - Introduction to Computer Graphics
//
// scene_lua.cpp
//
// Everything that's needed to parse a scene file using Lua.
// You don't necessarily have to understand exactly everything that
// goes on here, although it will be useful to have a reasonable idea
// if you wish to add new commands to the scene format.
//
// Lua interfaces with C/C++ using a special stack. Everytime you want
// to get something from lua, or pass something back to lua (e.g. a
// return value), you need to use this stack. Thus, most of the lua_
// and luaL_ functions actually manipulate the stack. All the
// functions beginning with "lua_" are part of the Lua C API itself,
// whereas the "luaL_" functions belong to a library of useful
// functions on top of that called lauxlib.
//
// This file consists of a bunch of C function declarations which
// implement functions callable from Lua. There are also two tables
// used to set up the interface between Lua and these functions, and
// the main "driver" function, import_lua, which calls the lua
// interpreter and sets up all the state.
//
// Note that each of the function declarations follow the same format:
// they take as their only argument the current state of the lua
// interpreter, and return the number of values returned back to lua.
//
// For more information see the book "Programming In Lua," available
// online at http://www.lua.org/pil/, and of course the Lua reference
// manual at http://www.lua.org/manual/5.0/.
//
// http://lua-users.org/wiki/LauxLibDocumentation provides a useful
// documentation of the "lauxlib" functions (beginning with luaL_).
//
// -- University of Waterloo Computer Graphics Lab 2005

#include "scene_lua.hpp"
#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdio>
#include "lua488.hpp"

// Uncomment the following line to enable debugging messages
// #define GRLUA_ENABLE_DEBUG

#ifdef GRLUA_ENABLE_DEBUG
#  define GRLUA_DEBUG(x) do { std::cerr << x << std::endl; } while (0)
#  define GRLUA_DEBUG_CALL do { std::cerr << __FUNCTION__ << std::endl; } while (0)
#else
#  define GRLUA_DEBUG(x) do { } while (0)
#  define GRLUA_DEBUG_CALL do { } while (0)
#endif

// You may wonder, for the following types, why we use special "_ud"
// types instead of, for example, just allocating entities directly
// from lua. Part of the answer is that Lua is a C api. It doesn't
// call any constructors or destructors for you, so it's easier if we
// let it just allocate a pointer for the node, and handle
// allocation/deallocation of the node itself. Another (perhaps more
// important) reason is that we will want SceneNodes to stick around
// even after lua is done with them, after all, we want to pass them
// back to the program. If we let Lua allocate SceneNodes directly,
// we'd lose them all when we are done parsing the script. This way,
// we can easily keep around the data, all we lose is the extra
// pointers to it.

// The "userdata" type for an entity. Objects of this type will be
// allocated by Lua to represent nodes.
struct gr_entity_ud {
  Entity* ent;
};

// The "userdata" type for a material. Objects of this type will be
// allocated by Lua to represent materials.

struct gr_draw_component_ud {
  DrawComponent* drawComp;
};

struct gr_ai_component_ud {
  AiComponent* aiComp;
};

struct gr_input_component_ud {
  InputComponent* inputComp;
};

struct gr_physics_component_ud {
  PhysicsComponent* physicsComp;
};

struct gr_light_component_ud {
  LightComponent* lightComp;
};

struct gr_camera_component_ud {
  CameraComponent* CameraComp;
};

// Create a node
extern "C"
int gr_entity_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_entity_ud* data = (gr_entity_ud*)lua_newuserdata(L, sizeof(gr_entity_ud));
  data->node = 0;

  const char* name = luaL_checkstring(L, 1);
  data->node = new Entity();

  luaL_getmetatable(L, "gr.entity");
  lua_setmetatable(L, -2);

  return 1;
}

// Create a draw component node
// extern "C"
// int gr_draw_component_cmd(lua_State* L)

// Create an ai component node
// extern "C"
// int gr_ai_component_cmd(lua_State* L)

// Create a input component node
// extern "C"
// int gr_input_component_cmd(lua_State* L)

// Create a physics component node
// extern "C"
// int gr_physics_component_cmd(lua_State* L)

// Create a light component node
// extern "C"
// int gr_light_component_cmd(lua_State* L)

// Create a camera component node
// extern "C"
// int gr_camera_component_cmd(lua_State* L)

// Add a child to a node
extern "C"
int gr_entity_add_component_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_entity_ud* selfdata = (gr_entity_ud*)luaL_checkudata(L, 1, "gr.entity");
  luaL_argcheck(L, selfdata != 0, 1, "entity expected");

  SceneNode* self = selfdata->node;
  
  gr_entity_ud* childdata = (gr_entity_ud*)luaL_checkudata(L, 2, "gr.entity");
  luaL_argcheck(L, childdata != 0, 2, "entity expected");

  SceneNode* child = childdata->node;

  self->add_component(child);

  return 0;
}

// Set a node's material
extern "C"
int gr_entity_set_material_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* selfdata = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, selfdata != 0, 1, "Node expected");

  GeometryNode* self = dynamic_cast<GeometryNode*>(selfdata->node);

  luaL_argcheck(L, self != 0, 1, "Geometry node expected");
  
  gr_material_ud* matdata = (gr_material_ud*)luaL_checkudata(L, 2, "gr.material");
  luaL_argcheck(L, matdata != 0, 2, "Material expected");

  Material* material = matdata->material;

  self->set_material(material);

  return 0;
}

// Add a translation to a Entity.
extern "C"
int gr_Entity_translate_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_entity_ud* selfdata = (gr_entity_ud*)luaL_checkudata(L, 1, "gr.Entity");
  luaL_argcheck(L, selfdata != 0, 1, "Entity expected");

  SceneEntity* self = selfdata->Entity;

  double values[3];
  
  for (int i = 0; i < 3; i++) {
    values[i] = luaL_checknumber(L, i + 2);
  }

  self->translate(Vector3D(values[0], values[1], values[2]));

  return 0;
}

// Rotate a Entity.
extern "C"
int gr_Entity_rotate_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_Entity_ud* selfdata = (gr_Entity_ud*)luaL_checkudata(L, 1, "gr.Entity");
  luaL_argcheck(L, selfdata != 0, 1, "Entity expected");

  SceneEntity* self = selfdata->Entity;

  const char* axis_string = luaL_checkstring(L, 2);

  luaL_argcheck(L, axis_string
                && std::strlen(axis_string) == 1, 2, "Single character expected");
  char axis = std::tolower(axis_string[0]);
  
  luaL_argcheck(L, axis >= 'x' && axis <= 'z', 2, "Axis must be x, y or z");
  
  double angle = luaL_checknumber(L, 3);

  self->rotate(axis, angle);

  return 0;
}

// Garbage collection function for lua.
extern "C"
int gr_Entity_gc_cmd(lua_State* L)
{
  GRLUA_DEBUG_CALL;
  
  gr_node_ud* data = (gr_node_ud*)luaL_checkudata(L, 1, "gr.node");
  luaL_argcheck(L, data != 0, 1, "Node expected");

  // Note that we don't delete the node here. This is because we still
  // want the scene to be around when we close the lua interpreter,
  // but at that point everything will be garbage collected.
  //
  // If data->node happened to be a reference-counting pointer, this
  // will in fact just decrease lua's reference to it, so it's not a
  // bad thing to include this line.
  data->node = 0;

  return 0;
}

// This is where all the "global" functions in our library are
// declared.
// If you want to add a new non-member function, add it here.
static const luaL_reg grlib_functions[] = {
  {"entity", gr_entity_cmd},
  {"draw_component", gr_draw_component_cmd},
  {"ai_component", gr_ai_component_cmd},
  {"input_component", gr_input_component_cmd},
  {"physics_component", gr_physics_component_cmd},
  {"light_component", gr_light_component_cmd},
  {"camera_component", gr_camera_component_cmd},
  {0, 0}
};

// This is where all the member functions for "gr.node" objects are
// declared. Since all the other objects (e.g. materials) are so
// simple, we only really need to make member functions for nodes.
//
// If you want to add a new member function for gr.node, add it
// here.
//
// We could have used inheritance in lua to match the inheritance
// between different node types, but it's easier to just give all
// nodes the same Lua type and then do any additional type checking in
// the appropriate member functions (e.g. gr_node_set_material_cmd
// ensures that the node is a GeometryNode, see above).
static const luaL_reg grlib_node_methods[] = {
  {"add_component", gr_entity_add_component_cmd},
  {"set_material", gr_entity_set_material_cmd},
  {"scale", gr_entity_scale_cmd},
  {"rotate", gr_entity_rotate_cmd},
  {"translate", gr_entity_translate_cmd},
  {0, 0}
};

// This function calls the lua interpreter to do the actual importing
SceneNode* import_lua(const std::string& filename)
{
  GRLUA_DEBUG("Importing scene from " << filename);
  
  // Start a lua interpreter
  lua_State* L = lua_open();

  GRLUA_DEBUG("Loading base libraries");
  
  // Load some base library
  luaL_openlibs(L);


  GRLUA_DEBUG("Setting up our functions");

  // Set up the metatable for gr.node
  // luaL_newmetatable(L, "gr.node");
  // lua_pushstring(L, "__index");
  // lua_pushvalue(L, -2);
  // lua_settable(L, -3);

  // Load the gr.node methods
  luaL_openlib(L, 0, grlib_node_methods, 0);

  // Load the gr functions
  luaL_openlib(L, "gr", grlib_functions, 0);

  GRLUA_DEBUG("Parsing the scene");
  // Now parse the actual scene
  if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 1, 0)) {
    std::cerr << "Error loading " << filename << ": " << lua_tostring(L, -1) << std::endl;
    return 0;
  }

  GRLUA_DEBUG("Closing the interpreter");
  
  // Close the interpreter, free up any resources not needed
  lua_close(L);

  // And return the node
  // return node;
}
