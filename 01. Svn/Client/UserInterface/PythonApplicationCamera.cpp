// Add the following `CPythonApplication::SetIllustratedCameraPosition` function anywhere in this file:
#if defined(ENABLE_MONSTER_CARD)
void CPythonApplication::SetIllustratedCameraPosition()
{
	CCamera* pCurrentCamera = CCameraManager::Instance().GetCurrentCamera();
	if (!pCurrentCamera)
		return;

	const D3DXVECTOR3& v3Target = pCurrentCamera->GetTarget();

	float fDistance, fPitch, fRotation, fHeight;
	GetCamera(&fDistance, &fPitch, &fRotation, &fHeight);

	m_pyGraphic.SetPositionCamera(v3Target.x, v3Target.y, v3Target.y + pCurrentCamera->GetTargetHeight(), fDistance, fPitch, fRotation);
}
#endif
