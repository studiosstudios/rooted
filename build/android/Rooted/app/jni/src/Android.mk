LOCAL_PATH  := $(call my-dir)
CURR_DEPTH  := ..
CUGL_OFFSET := ../../../../../../cugl

########################
#
# Main Application Entry
#
########################
CUGL_PATH := $(LOCAL_PATH)/$(CURR_DEPTH)/$(CUGL_OFFSET)
SDL2_PATH := $(CUGL_PATH)/sdlapp
PROJ_PATH := $(LOCAL_PATH)/${CURR_DEPTH}/../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(CUGL_PATH)/include
LOCAL_C_INCLUDES += $(SDL2_PATH)/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/box2d/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/poly2tri
LOCAL_C_INCLUDES += $(PROJ_PATH)/source/objects
LOCAL_C_INCLUDES += $(PROJ_PATH)/source
LOCAL_C_INCLUDES += $(PROJ_PATH)/source/events
LOCAL_C_INCLUDES += $(PROJ_PATH)/source/scenes
LOCAL_C_INCLUDES += $(PROJ_PATH)/source/shaders
LOCAL_C_INCLUDES += $(PROJ_PATH)/source/controllers


# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(LOCAL_PATH)/source/scenes/MenuScene.cpp \
	$(LOCAL_PATH)/source/scenes/LoadingScene.cpp \
	$(LOCAL_PATH)/source/scenes/GameScene.cpp \
	$(LOCAL_PATH)/source/scenes/ClientScene.cpp \
	$(LOCAL_PATH)/source/scenes/HostScene.cpp \
	$(LOCAL_PATH)/source/objects/Farmer.cpp \
	$(LOCAL_PATH)/source/objects/PlantingSpot.cpp \
	$(LOCAL_PATH)/source/objects/BabyCarrot.cpp \
	$(LOCAL_PATH)/source/objects/Map.cpp \
	$(LOCAL_PATH)/source/objects/Carrot.cpp \
	$(LOCAL_PATH)/source/objects/Wheat.cpp \
	$(LOCAL_PATH)/source/objects/EntityModel.cpp \
	$(LOCAL_PATH)/source/shaders/WheatRenderer.cpp \
	$(LOCAL_PATH)/source/shaders/ShaderNode.cpp \
	$(LOCAL_PATH)/source/events/CaptureBarrotEvent.cpp \
	$(LOCAL_PATH)/source/events/UnrootEvent.cpp \
	$(LOCAL_PATH)/source/events/CaptureEvent.cpp \
	$(LOCAL_PATH)/source/events/ResetEvent.cpp \
	$(LOCAL_PATH)/source/events/RootEvent.cpp \
	$(LOCAL_PATH)/source/controllers/InputController.cpp \
	$(LOCAL_PATH)/source/controllers/NetworkController.cpp \
	$(LOCAL_PATH)/source/controllers/CollisionController.cpp \
	$(LOCAL_PATH)/source/controllers/AIController.cpp \
	$(LOCAL_PATH)/source/controllers/ActionController.cpp \
	$(LOCAL_PATH)/source/controllers/UIController.cpp \
	$(LOCAL_PATH)/source/controllers/CameraController.cpp \
	$(LOCAL_PATH)/source/RootedApp.cpp \
	$(LOCAL_PATH)/source/main.cpp)

# Link in SDL2
LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_SHARED_LIBRARIES += SDL2_image
LOCAL_SHARED_LIBRARIES += SDL2_ttf
LOCAL_SHARED_LIBRARIES += SDL2_atk
LOCAL_SHARED_LIBRARIES += SDL2_app

LOCAL_STATIC_LIBRARIES := CUGL

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid
include $(BUILD_SHARED_LIBRARY)
