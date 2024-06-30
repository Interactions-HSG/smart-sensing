#ifndef __GROUPROLE_H__
#define __GROUPROLE_H__

#include <stdint.h>
#include <zephyr/data/json.h>
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

//**************************** concise version ******************************/
struct FunctionalSpecCon
{    
    int32_t qk;
    int32_t mi;
    int32_t ui;
    int32_t md;
};

static const struct json_obj_descr FunctionalSpecCon_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpecCon, qk, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpecCon, mi, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpecCon, ui, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct FunctionalSpecCon, md, JSON_TOK_NUMBER),    
};

struct GroupRoleInfoCon
{
    char* id;
    char* cid;
    bool act;
    int32_t acs;
    struct FunctionalSpecCon fs;
    int32_t mina;
    int32_t cura;
    int32_t curg;
    int32_t ming;
    int32_t maxg;   
    float rew;
    
};
static const struct json_obj_descr GroupRoleInfoCon_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, id, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, cid, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, act, JSON_TOK_TRUE),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, acs, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_OBJECT(struct GroupRoleInfoCon, fs, FunctionalSpecCon_descr),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, mina, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, cura, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, curg, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, ming, JSON_TOK_NUMBER),
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, maxg, JSON_TOK_NUMBER),  
  JSON_OBJ_DESCR_PRIM(struct GroupRoleInfoCon, rew, JSON_TOK_FLOAT),
  
};

struct GroupRoleInfosCon{
    struct GroupRoleInfoCon  e[5];
    int32_t n;
};

static const struct json_obj_descr GroupRoleInfosCon_descr[] = {
	JSON_OBJ_DESCR_OBJ_ARRAY(struct GroupRoleInfosCon, e, 5, n,
				 GroupRoleInfoCon_descr, ARRAY_SIZE(GroupRoleInfoCon_descr)),
};

struct PlayerInfoCon
{
    char *id;
    int32_t  ta;
    //float  rew;
    //float  cst;
    //float  ncs;
};

static const struct json_obj_descr PlayerInfoCon_descr[] = {
  JSON_OBJ_DESCR_PRIM(struct PlayerInfoCon, id, JSON_TOK_STRING),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfoCon, ta, JSON_TOK_NUMBER),
  /*JSON_OBJ_DESCR_PRIM(struct PlayerInfoCon, rew, JSON_TOK_FLOAT),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfoCon, cst, JSON_TOK_FLOAT),
  JSON_OBJ_DESCR_PRIM(struct PlayerInfoCon, ncs, JSON_TOK_FLOAT),*/
};

#endif