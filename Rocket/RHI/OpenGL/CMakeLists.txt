message(STATUS "Add Module RHI OpenGL")
add_library( RendererOpenGL
    OpenGLShader.cpp
    OpenGLGraphicsManager.cpp
    OpenGLPipelineStateManager.cpp
    OpenGLVertexArray.cpp
    OpenGLVertexBuffer.cpp
    OpenGLIndexBuffer.cpp
    OpenGLFrameBuffer.cpp
    OpenGLTexture.cpp
)
target_link_libraries( RendererOpenGL PRIVATE
    spdlog::spdlog_header_only
)