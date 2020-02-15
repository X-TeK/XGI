cmake_minimum_required (VERSION 2.8.11)

add_executable(xgi_example
    main.c
    ../XGI/EventHandler.c
    ../XGI/File.c
    ../XGI/FrameBuffer.c
    ../XGI/Graphics.c
    ../XGI/Input.c
    ../XGI/LinearMath.c
    ../XGI/List.c
    ../XGI/log.c
    ../XGI/Pipeline.c
    ../XGI/Random.c
    ../XGI/spirv_reflect.c
    ../XGI/stb_image.c
    ../XGI/Texture.c
    ../XGI/UniformBuffer.c
    ../XGI/VertexBuffer.c
    ../XGI/vk_mem_alloc.cpp
    ../XGI/Window.c
    ../XGI/XGI.c
)

target_include_directories(xgi_example PUBLIC ../Include)

target_link_directories(xgi_example PRIVATE ../../shaderc/build/libshaderc)
target_link_libraries(xgi_example SDL2 vulkan m shaderc_shared)
