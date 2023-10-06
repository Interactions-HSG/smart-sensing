#
# This is a project Makefile. It is assumed the directory this Makefile resides
# in is a project subdirectory.
#
############# MAKE AGENTSPEAK CODE TRANSLATION AND FLASH TO BOARD #############


translate:
	javac -cp embedded-bdi/lib/parser/lib/jason-2.6.jar                       \
        embedded-bdi/lib/parser/src/translator/As2Json.java                   \
        embedded-bdi/lib/parser/src/translator/PlanSkeleton.java              \
        embedded-bdi/lib/parser/src/translator/BodyInstruction.java           \
        embedded-bdi/lib/parser/src/translator/EventOperatorType.java         \
        embedded-bdi/lib/parser/src/translator/HeaderCreator.java

	java -cp                                                                  \
	    embedded-bdi/lib/parser/lib/jason-2.6.jar:embedded-bdi/lib/parser/src \
        translator.As2Json                                                    \
        src/mas-abstractions/agent/bdi-agent/agentspeak.asl                                          \
        src/mas-abstractions/agent/bdi-agent/functions.h                                             \
        src/mas-abstractions/agent/bdi-agent/configuration.h                                      	  \
        $(EVENT_BASE_SIZE) $(INTENTION_BASE_SIZE) $(INTENTION_STACK_SIZE)


clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(DOCS_DIR)/html

build:
	./build.sh

all: translate
	./build.sh

-include agent.config

######################## DEFAULT IDF MAKEFILE #################################

PROJECT_NAME := thingy53-bdi-home

#include $(IDF_PATH)/make/project.mk