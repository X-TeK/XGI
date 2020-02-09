#ifndef XGI_h
#define XGI_h

#include "EventHandler.h"
#include "File.h"
#include "FrameBuffer.h"
#include "Graphics.h"
#include "LinearMath.h"
#include "List.h"
#include "Pipeline.h"
#include "Random.h"
#include "Swapchain.h"
#include "Texture.h"
#include "UniformBuffer.h"
#include "VertexBuffer.h"
#include "Window.h"

void XGIInitialize(WindowConfigure windowFlags, GraphicsConfigure graphicsFlags);
void XGIDeinitialize(void);

#endif
