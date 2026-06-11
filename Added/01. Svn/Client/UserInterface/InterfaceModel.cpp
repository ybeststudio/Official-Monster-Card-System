#include "StdAfx.h"

#if defined(ENABLE_MONSTER_CARD)

#include <algorithm>
#include <chrono>
#include <cstring>

#include "../EterLib/Camera.h"
#include "../../extern/include/d3dx8math.h"

#include "InstanceBase.h"
#include "InterfaceModel.h"

void kalisto::InterfaceModel::ValidateTargetModel()
{
	if (m_targetModel == nullptr)
		CreateRace(m_race);
}

void kalisto::InterfaceModel::CreateRace(DWORD race)
{
	if (m_targetModel != nullptr)
		CInstanceBase::Delete(m_targetModel);

	m_race = race;
	static DWORD vidCounter = 2343223;
	m_targetModel = CInstanceBase::New();
	m_targetModel->DisableTextTail();
	m_targetModel->SetInterfaceModel(this);

	CInstanceBase::SCreateData data;
	std::memset(&data, 0, sizeof(CInstanceBase::SCreateData));
	data.m_dwRace = race;
	data.m_bType = CActorInstance::TYPE_NPC;
	data.m_dwVID = ++vidCounter;
	m_targetModel->Create(data);
	m_targetModel->Refresh(1, true);
	Show();
}

kalisto::InterfaceModel::~InterfaceModel() noexcept
{
	if (m_targetModel != nullptr)
	{
		m_targetModel->SetInterfaceModel(this);
		m_targetModel = nullptr;
	}
}

void kalisto::InterfaceModel::SetRace(DWORD race)
{
	CreateRace(race);
	RestoreStartPosition();
}

void kalisto::InterfaceModel::SetLoopMotion(WORD motionIndex)
{
	ValidateTargetModel();
	m_isLoopMotion = true;
	m_targetModel->SetLoopMotion(motionIndex, 0.0f, 0.0f);
}

void kalisto::InterfaceModel::StopLoopMotion()
{
	m_isLoopMotion = false;
}

void kalisto::InterfaceModel::Refresh()
{
	ValidateTargetModel();
	m_targetModel->Refresh(1, m_isLoopMotion);
	m_targetModel->GetGraphicThingInstanceRef().RefreshActorInstance();
}

#undef min
void kalisto::InterfaceModel::ScaleToFitInViewport(int width, int height, const CCamera* camera)
{
	ValidateTargetModel();
	D3DXVECTOR3 centerPos = {};
	float radius = 0.0f;
	(void)camera;
	m_targetModel->GetGraphicThingInstanceRef().GetBoundingSphere(centerPos, radius);
	float modelheight = m_targetModel->GetGraphicThingInstanceRef().GetHeight();
	float yScaleFactor = (std::min)(1.0f, static_cast<float>(height) / modelheight) + m_zoom;
	float xScaleFactor = (std::min)(yScaleFactor, static_cast<float>(width) / radius) + m_zoom;
	float zScaleFactor = xScaleFactor;
	m_targetModel->GetGraphicThingInstanceRef().SetScale(xScaleFactor, yScaleFactor, zScaleFactor);
	m_targetModel->GetGraphicThingInstanceRef().Scale(xScaleFactor, yScaleFactor, zScaleFactor);
}

void kalisto::InterfaceModel::SetInstanceBase(CInstanceBase* ptr)
{
	m_targetModel = ptr;
}

void kalisto::InterfaceModel::Render()
{
	ValidateTargetModel();
	if (!m_isShow)
	{
		Hide();
		return;
	}

	Show();
	if (m_rotationSpeed != 0.0f)
	{
		static constexpr const int WAITTIME_ROTATION = 16;
		static auto lastRotationStamp = std::chrono::high_resolution_clock::now();
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastRotationStamp).count();
		if (elapsedTime >= WAITTIME_ROTATION)
		{
			lastRotationStamp = now;
			m_xRotation += m_rotationSpeed;
			if (m_xRotation >= 360.0f)
				m_xRotation = 0.0f;
			m_targetModel->SetRotation(m_xRotation);
			m_targetModel->GetGraphicThingInstanceRef().RotationProcess();
		}
	}

	m_targetModel->Transform();
	m_targetModel->Deform();
	m_targetModel->Render();
}

void kalisto::InterfaceModel::RestoreStartPosition()
{
	ValidateTargetModel();
	static const D3DXMATRIX& identityRef = CGraphicBase::GetIdentityMatrix();
	CActorInstance& modelRef = m_targetModel->GetGraphicThingInstanceRef();
	modelRef.SetRotationMatrix(identityRef);
	m_zoom = 0.0f;
	m_xRotation = 0.0f;
	m_yRotation = 0.0f;
	m_targetModel->SetRotation(0.0f);
	m_targetModel->GetGraphicThingInstanceRef().SetXYRotation(0.0f, 0.0f);
}

void kalisto::InterfaceModel::RotateLeft(float rotation)
{
	ValidateTargetModel();
	m_xRotation += rotation;
	m_targetModel->SetRotation(m_xRotation);
	m_targetModel->GetGraphicThingInstanceRef().RotationProcess();
}

void kalisto::InterfaceModel::RotateRight(float rotation)
{
	ValidateTargetModel();
	m_xRotation -= rotation;
	m_targetModel->SetRotation(m_xRotation);
	m_targetModel->GetGraphicThingInstanceRef().RotationProcess();
}

void kalisto::InterfaceModel::RotateUp(float rotation)
{
	ValidateTargetModel();
	m_yRotation -= rotation;
	float verticalRotation = m_targetModel->GetRotation();
	m_targetModel->GetGraphicThingInstanceRef().SetXYRotation(verticalRotation, m_yRotation);
	m_targetModel->GetGraphicThingInstanceRef().RotationProcess();
}

void kalisto::InterfaceModel::RotateDown(float rotation)
{
	ValidateTargetModel();
	m_yRotation += rotation;
	float verticalRotation = m_targetModel->GetRotation();
	m_targetModel->GetGraphicThingInstanceRef().SetXYRotation(verticalRotation, m_yRotation);
	m_targetModel->GetGraphicThingInstanceRef().RotationProcess();
}

void kalisto::InterfaceModel::ZoomIn(float zoom)
{
	static const float MAX_ZOOM = 4.0f;
	if (m_zoom >= MAX_ZOOM)
		return;
	m_zoom += zoom;
}

void kalisto::InterfaceModel::ZoomOut(float zoom)
{
	static const float MAX_ZOOM = -0.3f;
	if (m_zoom <= MAX_ZOOM)
		return;
	m_zoom -= zoom;
}

void kalisto::InterfaceModel::EnableAutoRotation(float speed)
{
	m_rotationSpeed = speed;
}

void kalisto::InterfaceModel::DisableAutoRotation()
{
	m_rotationSpeed = 0.0f;
}

void kalisto::InterfaceModel::Scale(float x, float y, float z)
{
	ValidateTargetModel();
	m_targetModel->GetGraphicThingInstancePtr()->Scale(x, y, z);
}

void kalisto::InterfaceModel::Hide()
{
	ValidateTargetModel();
	m_targetModel->Hide();
	m_targetModel->SetAlwaysRender(false);
	m_isShow = false;
}

void kalisto::InterfaceModel::Show()
{
	ValidateTargetModel();
	m_targetModel->Show();
	m_targetModel->SetAlwaysRender(true);
	m_isShow = true;
}

CInstanceBase* kalisto::InterfaceModel::GetInstanceBase() noexcept
{
	return m_targetModel;
}

#endif // ENABLE_MONSTER_CARD

