#include "sdk.h"

bool ClientControllableEntity::IsAlive()
{
	if (!IsValidPtr(m_pHealthComp))
		return false;

	return (m_pHealthComp->m_Health > 0.0f);
}