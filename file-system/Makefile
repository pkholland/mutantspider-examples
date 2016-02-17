#
# file-system Makefile
#

#
# "PHONY" targets that aren't real build targets, but instead do
# something interesting -- like build a collection of targets and
# then run a server.  Here is the list of them supported by file-system
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
# the list of C/C++ files to compile -- all *.cpp files in our src directory
#
SOURCES=$(wildcard src/*.cpp)\
src/filesysjs-api.js6

#
# Specify what RESOURCES we want included in the build
# Note that excuting 'make display_rez' will show what resources
# it is including, and where they will be placed
#

#
# the list of resources (files we will fopen and fread during execution)
#
RESOURCES=$(wildcard rez/*)

#
# for testing purposes, make sure that file1.txt is in a custom
# directory -- not just the default "/resources" directory
#
rez/file1.txt_DST_DIR=/subdir1

#
# describe our build to mutantspider.mk
#
ms.BUILD_NAME:=filesys
ms.INTERMEDIATE_DIR:=ms_tmp/obj
ms.OUT_DIR:=ms_tmp/out
ms.UPLOAD_PREP_DIR:=ms_tmp/aws_prep
ms.POST_DESC:=aws_post.json
ms.API_FILE:=src/filesys.json

#
# list of files, beyond the ones that mutantspider.mk will produce
# via a normal 'make' command, which consititue a full build
#
ms.EXTRA_DEPLOY_FILES=\
index.html\
file-system.js

#
# this reads the various ms.XXX things we have defined above
# defines the functions we use below from them
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
# list of files we need to run a local dev server
#
DEPLOY_FILES=\
$(foreach file,$(MS_TARGETS) $(ms.EXTRA_DEPLOY_FILES),ms_tmp/local/$(CONFIG)/$(notdir $(file)))

#
# this just forces the default 'all' target to build the file set
# we build for running a local dev server
#
all: $(DEPLOY_FILES)

#
# template for build rule to copy/link files from some src location into some deploy/$(CONFIG) location
#
#  $1 = full path to the file that needs to be copied to deploy/$(CONFIG)
#
define copy_template
ms_tmp/local/$(CONFIG)/$(notdir $1): $1
	@mkdir -p $$(@D)
	$(call ms.CALL_TOOL,ln,-f $$< $$@,$$@)

endef

#
# instantiate an instance of copy_template for each file in $(MS_TARGETS) and $(ms.EXTRA_DEPLOY_FILES)
#
$(foreach file,$(MS_TARGETS) $(ms.EXTRA_DEPLOY_FILES),$(eval $(call copy_template,$(file))))

########################################

#
# both of the "run server" targets build all and then
# run the nacl-provided http deamon tool on the resulting
# deployment directory
#
run_server run_debug_server: all
	@cd ms_tmp/local/$(CONFIG) && $(NACL_SDK_ROOT)/tools/httpd.py --no-dir-check

