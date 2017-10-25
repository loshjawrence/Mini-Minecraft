#ifndef COMMON_H
#define COMMON_H

// Core
#include "engine.h"
#include "gameobject.h"
#include "component.h"
#include "material.h"
#include "input.h"
#include "assets/assetdatabase.h"

#define SHADOWMAPCOUNT 3
#define ShadowMapResolution01 2048
#define ShadowMapResolution02 2048
#define ShadowMapResolution03 2048

#define ShadowMapCameraSize01 16
#define ShadowMapCameraSize02 64
#define ShadowMapCameraSize03 256

// Assets
#include "./assets/mesh.h"
#include "./assets/shader.h"
#include "./assets/meshfactory.h"

// Components
#include "./components/camera.h"
#include "./components/renderer.h"

// UI
#include "./ui/uicomponent.h"
#include "./ui/uiimage.h"
#include "./ui/uibutton.h"
#include "./ui/uitext.h"

#endif // COMMON_H
