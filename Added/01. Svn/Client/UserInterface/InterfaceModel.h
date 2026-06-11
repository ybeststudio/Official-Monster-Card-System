#pragma once

#if defined(ENABLE_MONSTER_CARD)

#include <boost/noncopyable.hpp>

#include "StdAfx.h"

class CCamera;
class CInstanceBase;

namespace kalisto {

class InterfaceModel final : public boost::noncopyable
{
public:
	InterfaceModel() = default;
	~InterfaceModel() noexcept;

	CInstanceBase* GetInstanceBase() noexcept;
	void SetInstanceBase(CInstanceBase* ptr);

	void SetRace(DWORD race);
	void Render();
	void Refresh();
	void Hide();
	void Show();

	void SetLoopMotion(WORD motionIndex);
	void StopLoopMotion();

	void EnableAutoRotation(float speed);
	void DisableAutoRotation();

	void Scale(float x, float y, float z);
	void RestoreStartPosition();

	void RotateLeft(float rotation);
	void RotateRight(float rotation);
	void RotateUp(float rotation);
	void RotateDown(float rotation);

	void ZoomIn(float zoom);
	void ZoomOut(float zoom);

	void ScaleToFitInViewport(int width, int height, const CCamera* camera);

private:
	void CreateRace(DWORD race);
	void ValidateTargetModel();

private:
	DWORD m_race = 0;
	CInstanceBase* m_targetModel = nullptr;
	float m_rotationSpeed = 0.0f;
	float m_xRotation = 0.0f;
	float m_yRotation = 0.0f;
	float m_zoom = 0.0f;
	bool m_isLoopMotion = false;
	bool m_isShow = true;
};

} // namespace kalisto

#endif // ENABLE_MONSTER_CARD

