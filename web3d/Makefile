#
# w3d makefile
#

#
# "PHONY" targets that aren't real build targets, but instead do
# something interesting -- like build a collection of targets and
# then run a server.  Here is the list of them supported by file_system
#
.PHONY: all clean debug release run_server run_debug_server

#
# the default target is 'all'.  If you execute 'make' without specifying
# a target, this is the target it will build
#
all:

#
# set CONFIG for the convenience targets 'debug' and 'run_debug_server'
# we don't need to worry about either of the release targets because
# the default CONFIG is release
#
ifeq (debug,$(MAKECMDGOALS))
 CONFIG=debug
endif
ifeq (run_debug_server,$(MAKECMDGOALS))
 CONFIG=debug
endif

#
# these two phony targets depend on (and so build) 'all'
#
debug: all
release: all

#
# the list of C/C++ files to compile
#
SOURCES=$(wildcard ../other_repos/assimp/code/*.cpp)\
$(wildcard ../other_repos/assimp/contrib/ConvertUTF/*.c)\
$(wildcard ../other_repos/assimp/contrib/clipper/*.cpp)\
$(wildcard ../other_repos/assimp/contrib/irrXML/*.cpp)\
$(wildcard ../other_repos/assimp/contrib/poly2tri/poly2tri/common/*.cc)\
$(wildcard ../other_repos/assimp/contrib/poly2tri/poly2tri/sweep/*.cc)\
$(wildcard ../other_repos/assimp/contrib/unzip/*.c)\
$(wildcard ../other_repos/assimp/contrib/zlib/*.c)\
$(wildcard ../other_repos/assimp/contrib/openddlparser/code/*.cpp)\
../other_repos/assimp2json/assimp2json/cencode.c\
../other_repos/assimp2json/assimp2json/json_exporter.cpp\
../other_repos/assimp2json/assimp2json/mesh_splitter.cpp\
src/assimp_interface/main.cpp\
src/assimpjs-api.js6

INC_DIRS=\
../other_repos/assimp/code/BoostWorkaround\
../other_repos/assimp/include\
../other_repos/assimp/contrib/openddlparser/include\
../other_repos/assimp/contrib/rapidjson/include\
$(NACL_SDK_ROOT)/ports/include\
.

CFLAGS=-Wno-deprecated -DASSIMP_BUILD_NO_C4D_IMPORTER -Wno-c++11-narrowing

CFLAGS_pnacl+=--pnacl-exceptions=sjlj
LDFLAGS_pnacl+=--pnacl-exceptions=sjlj

CFLAGS_emcc+=-s DISABLE_EXCEPTION_CATCHING=0
LDFLAGS_emcc+=-s DISABLE_EXCEPTION_CATCHING=0


#
# where and what we are building
#
ms.BUILD_NAME:=assimp
ms.INTERMEDIATE_DIR:=ms_tmp/obj
ms.OUT_DIR:=ms_tmp/out
ms.UPLOAD_PREP_DIR:=ms_tmp/aws_prep
ms.POST_DESC:=scripts/aws_post.json
ms.API_FILE:=src/assimp_interface/assimp.json

#
# list of files which constitute a deployment
#
ms.EXTRA_DEPLOY_FILES=\
index.html\
w3d_boot.js\
../other_repos/WebGLDeveloperTools/src/debug/webgl-debug.js\
src/glMatrix-0.9.5.min.js

#
# List this one file as a "resource" so the C++ code can find
# it via fopen
#
RESOURCES=../other_repos/assimp/test/models/OBJ/WusonOBJ.obj

#
# this defines the functions we use below
#
include ../other_repos/mutantspider/mutantspider.mk


#
# this constructs all of the dependencies and build rules
# for everything we have defined in SOURCES.
#
# Debugging hint - if you want to see what rules this constructs
# change the 'eval' to 'info' and execute make again.
#
$(eval $(call ms.BUILD_RULES,$(ms.BUILD_NAME),$(SOURCES),$(INC_DIRS)))


#
# make clean deletes all built files
#
clean:
	rm -rf ms_tmp

#############################################################


#
# the files that mutantspider.mk will build
#
MS_TARGETS=$(call ms.TARGET_LIST,$(ms.BUILD_NAME),$(SOURCES))

#
# list of files which constitute a deployment
#
DEPLOY_FILES=\
$(foreach file,$(MS_TARGETS) $(ms.EXTRA_DEPLOY_FILES),ms_tmp/local/$(CONFIG)/$(notdir $(file)))

#
# make sure that 'all' builds all of the deploy files
#
all: $(DEPLOY_FILES)

#
# template for build rule to copy/link files from some src location into some deploy/$(CONFIG) location
#
#  $1 = full path to the file that needs to be copied to deploy/$(CONFIG)
#
define copy_template
ms_tmp/local/$(CONFIG)/$(notdir $(1)): $(1)
	@mkdir -p $$(@D)
	$(call ms.CALL_TOOL,ln,-f $$< $$@,$$@)

endef

#
# instantiate an instance of copy_template for each file in $(MS_TARGETS) and $(ms.EXTRA_DEPLOY_FILES)
#
$(foreach cp_file,$(MS_TARGETS) $(ms.EXTRA_DEPLOY_FILES),$(eval $(call copy_template,$(cp_file))))


########################################

#
# both of the "run server" targets build all and then
# run the nacl-provided http deamon tool on the resulting
# deployment directory
#
run_server run_debug_server: all
	@cd ms_tmp/local/$(CONFIG) && $(NACL_SDK_ROOT)/tools/httpd.py --no-dir-check
	


