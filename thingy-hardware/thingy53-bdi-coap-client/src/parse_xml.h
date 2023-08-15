#ifndef PARSE_XML_H_
#define PARSE_XML_H_


/**************************************************************************************************
  Includes.
**************************************************************************************************/
#include <stdio.h>
#include <unistd.h>

int get_role_id(int role_number, std::string *role_id, const char * org_spec);

int get_group_and_role_id(int role_number, std::string *group_id, std::string *role_id, int *group_number, const char * org_spec);

int parse_xml(struct agent_role *agent_roles, const char *org_spec);

int count_roles(const char * org_spec);

int parse_test(const char * org_spec, size_t org_spec_lenght);

int get_role_property(std::string role_id, std::string property_id, int *property_value, const char *org_spec);

int get_group_spec(std::string role_id, std::string attribute, int *value, const char *org_spec);

#endif /* PARSE_XML_H_ */
