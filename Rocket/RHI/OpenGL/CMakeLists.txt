message(STATUS "Add Module RHI OpenGL")
add_library( RendererOpenGL
    OpenGLShader.cpp
    OpenGLGraphicsManager.cpp
    OpenGLPipelineStateManager.cpp
    OpenGLVertexArray.cpp
    OpenGLVertexBuffer.cpp
    OpenGLIndexBuffer.cpp
    OpenGLUniformBuffer.cpp
    OpenGLFrameBuffer.cpp
    OpenGLTexture.cpp
)