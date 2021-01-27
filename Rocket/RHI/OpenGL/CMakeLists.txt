message(STATUS "Add Module RHI OpenGL")
add_library( RendererOpenGL
    OpenGLShader.cpp
    OpenGLGraphicsManager.cpp
    OpenGLPipelineStateManager.cpp
)
target_link_libraries( RendererOpenGL PRIVATE
    spdlog::spdlog_header_only
)