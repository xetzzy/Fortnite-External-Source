#pragma once
#include <d3d9.h>
#include <vector>
#define M_PI 3.14159265358979323846264338327950288419716939937510
namespace pointer
{
	inline uintptr_t uworld;
	inline uintptr_t game_instance;
	inline uintptr_t local_players;
	inline uintptr_t player_controller;
	inline uintptr_t local_pawn;
	inline uintptr_t root_component;
	inline uintptr_t player_state;
	inline uintptr_t game_state;
	inline uintptr_t player_array;
	inline uintptr_t closest_pawn;
	inline float closest_distance;
}

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f) {}
	Vector2(double _x, double _y) : x(_x), y(_y) {}
	~Vector2() {}
	double x;
	double y;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f) {}
	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z) {}
	~Vector3() {}
	double x;
	double y;
	double z;
	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}
	inline double Length()
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}
	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
	Vector3 operator*(double v)
	{
		return Vector3(x * v, y * v, z * v);
	}
};

struct FQuat
{
	double x;
	double y;
	double z;
	double w;
};

struct FTransform
{
	FQuat rot;
	Vector3 translation;
	char pad[4];
	Vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m{};
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;
		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;
		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;
		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;
		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;
		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;
		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;
		return m;
	}
};

D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut{};
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;
	return pOut;
}

D3DMATRIX Matrix(Vector3 rot, Vector3 origin)
{
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);
	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);
	D3DMATRIX matrix{};
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;
	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;
	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;
	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;
	return matrix;
}

Vector3 GetBoneWithRotation(uintptr_t mesh, int id)
{
	uintptr_t bone_array = driver.read<uintptr_t>(mesh + 0x5D0);
	int is_bone_array_cached = driver.read<int>(mesh + 0x618);
	if (is_bone_array_cached) bone_array = driver.read<uintptr_t>(mesh + 0x5E0);
	FTransform bone = driver.read<FTransform>(bone_array + (id * 0x60));
	FTransform component_to_world = driver.read<FTransform>(mesh + 0x240);
	D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
	return Vector3(matrix._41, matrix._42, matrix._43);
}

bool IsVisible(uintptr_t mesh)
{
	float last_sumbit_time = driver.read<float>(mesh + 0x338);
	float last_render_time_on_screen = driver.read<float>(mesh + 0x340);
	bool visible = last_render_time_on_screen + 0.06f >= last_sumbit_time;
	return visible;
}

struct Camera
{
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
};

Camera GetViewAngles()
{
	Camera local_camera{};
	uintptr_t chain69 = driver.read<uintptr_t>(pointer::uworld + 0x110);
	local_camera.Location = driver.read<Vector3>(chain69);
	uintptr_t view_matrix = driver.read<uintptr_t>(pointer::local_players + 0xD0);
	uintptr_t view_matrix_correct = driver.read<uintptr_t>(view_matrix + 0x8);
	local_camera.FieldOfView = 80.f / (driver.read<double>(view_matrix_correct + 0x7F0) / 1.19f);
	local_camera.Rotation.x = driver.read<double>(view_matrix_correct + 0x9C0);
	local_camera.Rotation.y = driver.read<double>(pointer::root_component + 0x148);
	local_camera.Rotation.x = (asin(local_camera.Rotation.x)) * (180.0 / M_PI);
	if (local_camera.Rotation.y < 0)
		local_camera.Rotation.y = 360 + local_camera.Rotation.y;
	return local_camera;
}

Vector2 ProjectWorldToScreen(Vector3 world_location)
{
	Camera local_camera = GetViewAngles();
	D3DMATRIX temp_matrix = Matrix(local_camera.Rotation, Vector3(0, 0, 0));
	Vector3 vaxisX = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
	Vector3 vaxisY = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
	Vector3 vaxisZ = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
	Vector3 vdelta = world_location - local_camera.Location;
	Vector3 vtransformed = Vector3(vdelta.Dot(vaxisY), vdelta.Dot(vaxisZ), vdelta.Dot(vaxisX));
	if (vtransformed.z < 1.f)
		vtransformed.z = 1.f;
	return Vector2((settings::width / 2.0f) + vtransformed.x * (((settings::width / 2.0f) / tanf(local_camera.FieldOfView * (float)M_PI / 360.f))) / vtransformed.z, (settings::height / 2.0f) - vtransformed.y * (((settings::width / 2.0f) / tanf(local_camera.FieldOfView * (float)M_PI / 360.f))) / vtransformed.z);
}