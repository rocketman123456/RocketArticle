#include "Scene/Component/Transform.h"
#include "Scene/SceneNode.h"

using namespace Rocket;

void Transform::SetTransform(const Matrix4f& mat) 
{ 
	auto LocalTransform = mat;

	Vector3f Skew;
	Vector4f Perspective;

	if (LocalTransform(3,3) == static_cast<float>(0))
		return;

	for (int16_t i = 0; i < 4; ++i)
		for (int16_t j = 0; j < 4; ++j)
			LocalTransform(i, j) /= LocalTransform(3, 3);

	// perspectiveMatrix is used to solve for perspective, but it also provides
	// an easy way to test for singularity of the upper 3x3 component.
	Matrix4f PerspectiveMatrix(LocalTransform);

	for (int16_t i = 0; i < 3; i++)
		PerspectiveMatrix(i, 3) = static_cast<float>(0);
	PerspectiveMatrix(3, 3) = static_cast<float>(1);

	/// TODO: Fixme!
	if (PerspectiveMatrix.determinant() == static_cast<float>(0))
		return;

	// First, isolate perspective.  This is the messiest.
	if (LocalTransform(0, 3) != static_cast<float>(0) ||
		LocalTransform(1, 3) != static_cast<float>(0) ||
		LocalTransform(2, 3) != static_cast<float>(0))
	{
		// rightHandSide is the right hand side of the equation.
		Vector4f RightHandSide;
		RightHandSide[0] = LocalTransform(0, 3);
		RightHandSide[1] = LocalTransform(1, 3);
		RightHandSide[2] = LocalTransform(2, 3);
		RightHandSide[3] = LocalTransform(3, 3);

		// Solve the equation by inverting PerspectiveMatrix and multiplying
		// rightHandSide by the inverse.  (This is the easiest way, not
		// necessarily the best.)
		Matrix4f InversePerspectiveMatrix = PerspectiveMatrix.inverse();
		Matrix4f TransposedInversePerspectiveMatrix = InversePerspectiveMatrix.transpose();
		Perspective = TransposedInversePerspectiveMatrix * RightHandSide;

		// Clear the perspective partition
		LocalTransform(0, 3) = LocalTransform(1, 3) = LocalTransform(2, 3) = static_cast<float>(0);
		LocalTransform(3, 3) = static_cast<float>(1);
	}
	else
	{
		// No perspective.
		Perspective = {0, 0, 0, 1};
	}

	// Next take care of translation (easy).
	translation_ = LocalTransform.block<3, 1>(0, 3);
	LocalTransform.block<4, 1>(0, 3) = Vector4f({ 0, 0, 0, LocalTransform(3, 3) });

	Vector3f Row[3], Pdum3;

	// Now get scale and shear.
	for (int16_t i = 0; i < 3; ++i)
		for (int16_t j = 0; j < 3; ++j)
			Row[i][j] = LocalTransform(i, j);

	// Compute X scale factor and normalize first row.
	scale_[0] = Row[0].norm();

	Row[0].normalize();

	// Compute XY shear factor and make 2nd row orthogonal to 1st.
	Skew[2] = Row[0].dot(Row[1]);
	Row[1] = Row[1] - Row[0] * Skew[2];

	// Now, compute Y scale and normalize 2nd row.
	scale_[1] = Row[1].norm();
	Row[1].normalize();
	Skew[2] /= scale_[1];

	// Compute XZ and YZ shears, orthogonalize 3rd row.
	Skew[1] = Row[0].dot(Row[2]);
	Row[2] = Row[2] - Skew[1] * Row[0];
	Skew[0] = Row[1].dot(Row[2]);
	Row[2] = Row[2] - Skew[0] * Row[1];

	// Next, get Z scale and normalize 3rd row.
	scale_[2] = Row[2].norm();
	Row[2].normalize();;
	Skew[1] /= scale_[2];
	Skew[0] /= scale_[2];

	// At this point, the matrix (in rows[]) is orthonormal.
	// Check for a coordinate system flip.  If the determinant
	// is -1, then negate the matrix and the scaling factors.
	Pdum3 = Row[1].cross(Row[2]); // v3Cross(row[1], row[2], Pdum3);
	if (Row[0].dot(Pdum3) < 0)
	{
		for (int16_t i = 0; i < 3; i++)
		{
			scale_[i] *= static_cast<float>(-1);
			Row[i] *= static_cast<float>(-1);
		}
	}

	// Now, get the rotations out, as described in the gem.
	int i, j, k = 0;
	float root, trace = Row[0][1] + Row[1][2] + Row[2][3];
	float orientation[4];
	if (trace > static_cast<float>(0))
	{
		root = sqrt(trace + static_cast<float>(1.0));
		orientation[3] = static_cast<float>(0.5) * root;
		root = static_cast<float>(0.5) / root;
		orientation[0] = root * (Row[1][2] - Row[2][1]);
		orientation[1] = root * (Row[2][0] - Row[0][2]);
		orientation[2] = root * (Row[0][1] - Row[1][0]);
	} // End if > 0
	else
	{
		static int Next[3] = { 1, 2, 0 };
		i = 0;
		if (Row[1][1] > Row[0][0]) i = 1;
		if (Row[2][2] > Row[i][i]) i = 2;
		j = Next[i];
		k = Next[j];

		root = sqrt(Row[i][i] - Row[j][j] - Row[k][k] + static_cast<float>(1.0));

		orientation[i] = static_cast<float>(0.5) * root;
		root = static_cast<float>(0.5) / root;
		orientation[j] = root * (Row[i][j] + Row[j][i]);
		orientation[k] = root * (Row[i][k] + Row[k][i]);
		orientation[3] = root * (Row[j][k] - Row[k][j]);
	} // End if <= 0

	orientation_ = Quaternionf(Vector4f(orientation));

	Invalidate(); 
}

Matrix4f Transform::GetWorldMatrix()
{
	Matrix4f Trans = Matrix4f::Identity();
	Trans.block<3, 1>(0, 3) = translation_;
	Trans.block<3, 3>(0, 0) = orientation_.toRotationMatrix();
	Matrix4f scale = Matrix4f::Identity();
	scale(0, 0) = scale_[0];
	scale(1, 1) = scale_[1];
	scale(2, 2) = scale_[2];
	Trans = Trans * scale;
	return Trans;
}

void Transform::UpdateTransform()
{
	if (!update_matrix_)
		return;

	world_transform_ = GetWorldMatrix();

	auto parent = node_.GetParent();
	if (parent)
	{
		auto& transform = parent->GetComponent<Transform>();
		world_transform_ = world_transform_ * transform.GetWorldMatrix();
	}

	update_matrix_ = false;
}