#ifndef __GROUPROLE_H__
#define __GROUPROLE_H__

#include <stdint.h>
#include <data/json.h>

struct FunctionalSpec
{
    int32_t measurementInterval;
    int32_t updateInterval;
    char* hasQuantityKind;
};

static const struct json_obj_descr FunctionalSpec_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, measurementInterval, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, updateInterval, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, hasQuantityKind, JSON_TOK_STRING),
};

struct GroupRoleInfo
{
    char* id;
    bool isActive;
    int32_t minAllocation;
    int32_t currentAllocation;
    int32_t minAgents;
    int32_t maxAgents;
    int32_t currentAgents;
    int32_t measurementInterval;
    int32_t updateInterval;
    char* hasQuantityKind;
    int32_t reward;
    //struct FunctionalSpec functionalSpec;
};

static const struct json_obj_descr GroupRoleInfo_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, id, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, minAllocation, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, currentAllocation, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, minAgents, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, maxAgents, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, currentAgents, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, measurementInterval, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, updateInterval, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, hasQuantityKind, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, reward, JSON_TOK_NUMBER),
  //JSON_OBJ_DESCR_OBJECT(struct GroupRoleInfo, functionalSpec, FunctionalSpec_descr),
};

struct GroupRoleInfos{
    struct GroupRoleInfo  elements[10];
    size_t num_elements;
};

static const struct json_obj_descr GroupRoleInfos_descr[] = {
	JSON_OBJ_DESCR_OBJ_ARRAY(struct GroupRoleInfos, elements, 10, num_elements,
				 GroupRoleInfo_descr, ARRAY_SIZE(GroupRoleInfo_descr)),
};

struct PlayerInfo
{
    char *id;
    int32_t  taskAllocation;
    int32_t  reward;
    int32_t  cost;
    int32_t  networkCost;
};

static const struct json_obj_descr PlayerInfo_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, id, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, taskAllocation, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, reward, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, cost, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, networkCost, JSON_TOK_NUMBER),
};

#endif