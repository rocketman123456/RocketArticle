#include "Render/DrawBasic/Shader.h"
#include "Utils/Hashing.h"

namespace Rocket
{
    class VullkanShader : implements Shader
    {
    public:
        VullkanShader();
        virtual ~VullkanShader();

        bool Initialize(const ShaderSourceList& list) final;
		void Finalize() final;

        void Bind() const final;
		void Unbind() const final;

		UniformData GetUniformBinding() final { return {}; }

		bool SetInt32(const String& name, int32_t value) final;
        bool SetUInt32(const String& name, uint32_t value) final;
		bool SetInt32Array(const String& name, int32_t* values, uint32_t count) final;
        bool SetUInt32Array(const String& name, uint32_t* values, uint32_t count) final;

		bool SetFloat(const String& name, float value) final;
        bool SetDouble(const String& name, double value) final;

		bool SetVector2f(const String& name, const Vector2f& value) final;
		bool SetVector3f(const String& name, const Vector3f& value) final;
		bool SetVector4f(const String& name, const Vector4f& value) final;

        bool SetMatrix2f(const String& name, const Matrix2f& value) final;
		bool SetMatrix3f(const String& name, const Matrix3f& value) final;
		bool SetMatrix4f(const String& name, const Matrix4f& value) final;

        const String& GetName() const { return AssetHashTable::GetStringFromId(m_NameId); }
		uint32_t GetRenderId() { return m_RendererId; }
    private:
        uint64_t m_NameId;
        uint32_t m_RendererId;
    };
}