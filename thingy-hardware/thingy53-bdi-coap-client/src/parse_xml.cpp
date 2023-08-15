#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
//#include "tinyxml2/tinyxml2.h"
#include "tinyxml2/tinyxml2.cpp"
#include <main.h>

// using namespace std;
// using namespace tinyxml2;

LOG_MODULE_REGISTER(parse_xml, CONFIG_COAP_CLIENT_LOG_LEVEL);

/*************************************************************************************************/
/*!
 *  \brief  Counts the number of roles in the org-spec XML file.
 *
 *  \param  org_spec: pointer to .xml char array.
 * 
 *  \return numer of roles if success, -1 otherwise.
 */
/*************************************************************************************************/
int count_roles(const char * org_spec)
{
    int i = -1;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.Parse(org_spec, strlen(org_spec));
    if (eResult != tinyxml2::XML_SUCCESS) return -1;

    /* Get root Element <organisational-specification> */
    tinyxml2::XMLNode* root = doc.RootElement();
    if (NULL != root) {

        /* Get Child <structural-specification> */
        tinyxml2::XMLElement * structuralSpec = root -> FirstChildElement("structural-specification");
        if (NULL != structuralSpec) {

            /* Get Child <role-definitions> */
            tinyxml2::XMLElement * roleDef = structuralSpec -> FirstChildElement("role-definitions");
            if (NULL != roleDef) {
                
                /* Get all <role> elements */
                tinyxml2::XMLElement * role = roleDef -> FirstChildElement("role");
                i = 0;
                while (role) {
                    i++;
                    role = role -> NextSiblingElement("role");

                }
            }
        }
    }
    if (i == -1){
        LOG_WRN("Unexpected XML structure!\n");
        return -1;
    }
    return i;
}

/*************************************************************************************************/
/*!
 *  \brief  Gets the role Id for a given role number.
 *
 *  \param  role_number: number of the role in the list.
 *  \param  role_id: pointer to a string where the ID gets stored.
 *  \param  org_spec: pointer to .xml char array.
 * 
 *  \return 0 if success, -1 otherwise.
 */
/*************************************************************************************************/
int get_role_id(int role_number, std::string *role_id, const char * org_spec)
{
    int i = -1;
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.Parse(org_spec, strlen(org_spec));
    if (eResult != tinyxml2::XML_SUCCESS) return -1;

    *role_id = "";
    /* Get root Element <organisational-specification> */
    tinyxml2::XMLNode* root = doc.RootElement();
    if (NULL != root) {

        /* Get Child <structural-specification> */
        tinyxml2::XMLElement * structuralSpec = root -> FirstChildElement("structural-specification");
        if (NULL != structuralSpec) {

            /* Get Child <role-definitions> */
            tinyxml2::XMLElement * roleDef = structuralSpec -> FirstChildElement("role-definitions");
            if (NULL != roleDef) {
                
                /* Get all <role> elements */
                tinyxml2::XMLElement * role = roleDef -> FirstChildElement("role");
                i = 0;
                while (role) {

                    if (i == role_number){
                        *role_id = role->Attribute("id");
                    }
                    i++;
                    role = role -> NextSiblingElement("role");

                }
            }
        }
    }
    if (i == -1){
        LOG_WRN("Unexpected XML structure!\n");
        return -1;
    }
    if (*role_id == ""){
        LOG_WRN("Role not found!\n");
        return -1;
    }
    return 0;
}

/*************************************************************************************************/
/*!
 *  \brief  Returns the role ID, group ID and group number for a given role number.
 *
 *  \param  role_number: number of the role in the list.
 *  \param  group_id: pointer to a string where the group ID gets stored.
 *  \param  role_id: pointer to a string where the role ID gets stored.
 *  \param  group_number: pointer to an integer where the group number gets stored.
 *  \param  org_spec: pointer to .xml char array.
 * 
 *  \return 0 if success, -1 otherwise.
 */
/*************************************************************************************************/
int get_group_and_role_id(int role_number, std::string *group_id, std::string *role_id, int *group_number, const char *org_spec)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.Parse(org_spec, strlen(org_spec));
    if (eResult != tinyxml2::XML_SUCCESS) return -1;

    int i = -1;
    int j = -1;
    *role_id = "";
    *group_id = "";

    /* Get root Element <organisational-specification> */
    tinyxml2::XMLNode* root = doc.RootElement();
    if (NULL != root) {

        /* Get Child <structural-specification> */
        tinyxml2::XMLElement * structuralSpec = root -> FirstChildElement("structural-specification");
        if (NULL != structuralSpec) {

            /* Get Child <role-definitions> */
            tinyxml2::XMLElement * roleDef = structuralSpec -> FirstChildElement("role-definitions");
            
            /* Get Sibling <group-specification> */
            tinyxml2::XMLElement * groupSpec = roleDef -> NextSiblingElement("group-specification");
            i = 0;
            while(groupSpec) {

                /* Get Chile <roles> */
                tinyxml2::XMLElement * g_roles = groupSpec -> FirstChildElement("roles");
                if (NULL != g_roles) {
                    
                    /* Get all <role> elements */
                    tinyxml2::XMLElement * g_role = g_roles -> FirstChildElement("role");
                    j = 0;
                    while (g_role) {
                        if (j == role_number){
                            *role_id = g_role->Attribute("id");
                            *group_id = groupSpec->Attribute("id");
                            *group_number = i+1;
                        }
                        j++;
                        g_role = g_role -> NextSiblingElement("role");
                    }
                }
                i++;
                groupSpec = groupSpec -> NextSiblingElement("group-specification");
            }
        }
    }
    if (i == -1 || j == -1){
        LOG_WRN("Unexpected XML structure!\n");
        return -1;
    }
    if (*role_id == ""){
        LOG_WRN("Role not found!\n");
        return -1;
    }
    if (*group_id == ""){
        LOG_WRN("Group not found!\n");
        return -1;
    }
    return 0;
}

/*************************************************************************************************/
/*!
 *  \brief  Gets the value of a given property ID for a given role ID.
 *
 *  \param  role_id: role ID.
 *  \param  property_id: Property ID.
 *  \param  property_value: pointer to an integer where the value gets stored.
 *  \param  org_spec: pointer to .xml char array.
 * 
 *  \return 0 if success, -1 otherwise.
 */
/*************************************************************************************************/
int get_role_property(std::string role_id, std::string property_id, 
                      int *property_value, const char *org_spec)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.Parse(org_spec, strlen(org_spec));
    if (eResult != tinyxml2::XML_SUCCESS) return -1;

    *property_value = -1;
    /* Get root Element <organisational-specification> */
    tinyxml2::XMLNode* root = doc.RootElement();
    if (NULL != root) {

        /* Get Child <structural-specification> */
        tinyxml2::XMLElement * structuralSpec = root -> FirstChildElement("structural-specification");
        if (NULL != structuralSpec) {

            /* Get Child <role-definitions> */
            tinyxml2::XMLElement * roleDef = structuralSpec -> FirstChildElement("role-definitions");
            if (NULL != roleDef) {
                
                /* Get all <role> elements */
                tinyxml2::XMLElement * role = roleDef -> FirstChildElement("role");
                while (role) {
                    
                    //role->Attribute("id");
                    
                    /* Get Child <properties> */
                    tinyxml2::XMLElement * p_properties = role -> FirstChildElement("properties");
                    if (NULL != p_properties) {
                        
                        /* Get all <property> elements */
                        tinyxml2::XMLElement * p_property = p_properties -> FirstChildElement("property");
                        while (p_property) {
                            
                            if (role_id == role->Attribute("id") && property_id == p_property->Attribute("id")){
                                *property_value = p_property->IntAttribute("value");
                            }
                            p_property = p_property -> NextSiblingElement("property");
                        }
                    }
                    role = role -> NextSiblingElement("role");
                }
            }
        }
    }
    if (*property_value == -1){
        printf("Property not found!\n");
        return -1;
    }
    return 0;
}

int get_group_spec(std::string role_id, std::string attribute, int *value, const char *org_spec)
{
    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError eResult = doc.Parse(org_spec, strlen(org_spec));
    if (eResult != tinyxml2::XML_SUCCESS) return -1;

    *value = -1;

    /* Get root Element <organisational-specification> */
    tinyxml2::XMLNode* root = doc.RootElement();
    if (NULL != root) {

        /* Get Child <structural-specification> */
        tinyxml2::XMLElement * structuralSpec = root -> FirstChildElement("structural-specification");
        if (NULL != structuralSpec) {

            /* Get Child <role-definitions> */
            tinyxml2::XMLElement * roleDef = structuralSpec -> FirstChildElement("role-definitions");
            
            /* Get Sibling <group-specification> */
            tinyxml2::XMLElement * groupSpec = roleDef -> NextSiblingElement("group-specification");
            if (NULL != groupSpec) {

                /* Get Chile <roles> */
                tinyxml2::XMLElement * g_roles = groupSpec -> FirstChildElement("roles");
                if (NULL != g_roles) {
                    
                    /* Get all <role> elements */
                    tinyxml2::XMLElement * g_role = g_roles -> FirstChildElement("role");
                    while (g_role) {
                        if (role_id == g_role->Attribute("id") && attribute == "min"){
                                *value = g_role->IntAttribute("min");
                        }
                        if (role_id == g_role->Attribute("id") && attribute == "max"){
                                *value = g_role->IntAttribute("max");
                        }
                        g_role = g_role -> NextSiblingElement("role");
                    }
                }
            }
        }
    }
    if (*value == -1){
        printf("Attribute not found!\n");
        return -1;
    }
    return 0;
}