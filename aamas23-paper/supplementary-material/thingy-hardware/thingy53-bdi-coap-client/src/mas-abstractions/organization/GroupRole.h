#ifndef __GROUPROLE_H__
#define __GROUPROLE_H__

#include <stdint.h>
#include <data/json.h>

struct FunctionalSpec
{    
    int32_t hasQuantityKind;
    int32_t measurementInterval;
    int32_t updateInterval;
    int32_t measurementDuration;
};

static const struct json_obj_descr FunctionalSpec_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, hasQuantityKind, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, measurementInterval, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, updateInterval, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpec, measurementDuration, JSON_TOK_NUMBER),    
};

struct GroupRoleInfo
{
    char* id;
    char* creatorId;
    bool isActive;
    int32_t isActiveSince;
    struct FunctionalSpec functionalSpecification;
    int32_t minAllocation;
    int32_t currentAllocation;
    int32_t currentAgents;
    int32_t minAgents;
    int32_t maxAgents;   
    float reward;
    
};

static const struct json_obj_descr GroupRoleInfo_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, id, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, creatorId, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, isActive, JSON_TOK_TRUE),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, isActiveSince, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_OBJECT(struct GroupRoleInfo, functionalSpecification, FunctionalSpec_descr),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, minAllocation, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, currentAllocation, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, currentAgents, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, minAgents, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, maxAgents, JSON_TOK_NUMBER),  
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfo, reward, JSON_TOK_FLOAT),
  
};

struct GroupRoleInfos{
    struct GroupRoleInfo  elements[5];
    int32_t num_elements;
};

static const struct json_obj_descr GroupRoleInfos_descr[] = {
	JSON_OBJ_DESCR_OBJ_ARRAY(struct GroupRoleInfos, elements, 5, num_elements,
				 GroupRoleInfo_descr, ARRAY_SIZE(GroupRoleInfo_descr)),
};

struct PlayerInfo
{
    char *id;
    int32_t  taskAllocation;
    float  reward;
    float  cost;
    float  networkCost;
};

static const struct json_obj_descr PlayerInfo_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, id, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, taskAllocation, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, reward, JSON_TOK_FLOAT),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, cost, JSON_TOK_FLOAT),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfo, networkCost, JSON_TOK_FLOAT),
};

#endif