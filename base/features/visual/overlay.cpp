#include "overlay.h"

// used: [stl] sort
#include <algorithm>

// used: render functions
#include "../../utilities/draw.h"
// used: camera origin
#include "../visual.h"

// used: interface handles
#include "../../core/interfaces.h"
// used: interface declarations
#include "../../sdk/interfaces/ibaseclientdll.h"
#include "../../sdk/interfaces/icliententitylist.h"
#include "../../sdk/interfaces/iglobalvars.h"

using namespace F::VISUAL;

#pragma region visual_overlay_components
ImVec2 OVERLAY::CBaseComponent::GetBasePosition(const ImVec4& box) const
{
	return { box[this->nSide == SIDE_RIGHT ? SIDE_RIGHT : SIDE_LEFT], box[this->nSide == SIDE_BOTTOM ? SIDE_BOTTOM : SIDE_TOP] };
}

ImVec2 OVERLAY::CBaseDirectionalComponent::GetBasePosition(const ImVec4& box) const
{
	ImVec2 vecBasePosition = { };

	if (this->nSide == SIDE_TOP || this->nSide == SIDE_BOTTOM)
	{
		Q_ASSERT(this->nDirection != (this->nSide ^ SIDE_BOTTOM) + 1); // this direction isn't supported for this side
		vecBasePosition = { (box[SIDE_LEFT] + box[SIDE_RIGHT]) * 0.5f, box[this->nSide] };
	}
	else if (this->nSide == SIDE_LEFT || this->nSide == SIDE_RIGHT)
	{
		Q_ASSERT(this->nDirection != (this->nSide ^ SIDE_RIGHT)); // this direction isn't supported for this side
		vecBasePosition = { box[this->nSide], box[this->nDirection == DIR_TOP ? SIDE_BOTTOM : SIDE_TOP] };
	}
	else
	{
		Q_ASSERT(false); // this side isn't supported for this component
		return vecBasePosition;
	}

	if (this->nSide != SIDE_RIGHT && this->nDirection != DIR_RIGHT)
		vecBasePosition.x -= this->vecSize.x * ((static_cast<std::uint8_t>(this->nDirection) == static_cast<std::uint8_t>(this->nSide) && (this->nSide & 1U) == 1U) ? 0.5f : 1.0f);

	if (this->nSide == SIDE_TOP || this->nDirection == DIR_TOP)
		vecBasePosition.y -= this->vecSize.y;

	return vecBasePosition;
}

OVERLAY::CBarComponent::CBarComponent(const EAlignSide nAlignSide, const ImVec4& vecBox, const float flProgressFactor, const float flThickness, const Color_t& colPrimary, const Color_t& colBackground, const float flOutlineThickness, const Color_t& colOutline) :
	flProgressFactor(flProgressFactor), flThickness(flThickness), colPrimary(colPrimary), colBackground(colBackground), flOutlineThickness(flOutlineThickness), colOutline(colOutline)
{
	this->nSide = nAlignSide;

	const bool bIsHorizontal = ((nAlignSide & 1U) == 1U);
	const float flTotalThickness = flThickness + flOutlineThickness * 2.0f;
	this->vecSize = { (bIsHorizontal ? vecBox[SIDE_RIGHT] - vecBox[SIDE_LEFT] : flTotalThickness), (bIsHorizontal ? flTotalThickness : vecBox[SIDE_BOTTOM] - vecBox[SIDE_TOP]) };
}

void OVERLAY::CBarComponent::Render(const ImVec2& vecPosition)
{
	const float flHalfThickness = this->flThickness * 0.5f;
	const float flHalfOutlineThickness = this->flOutlineThickness * 0.5f;
	const ImVec2 vecThicknessOffset = { flHalfThickness + flHalfOutlineThickness, flHalfThickness + flHalfOutlineThickness };

	ImVec2 vecMin = vecPosition, vecMax = vecPosition + this->vecSize;

	// background
	D::pDrawListActive->AddRectFilled(vecMin, vecMax, this->colBackground.GetU32());
	// outline
	D::pDrawListActive->AddRect(vecMin, vecMax, this->colOutline.GetU32(), 0.0f, ImDrawCornerFlags_All, this->flOutlineThickness);

	// account outline offset
	vecMin += vecThicknessOffset;
	vecMax -= vecThicknessOffset;

	const ImVec2 vecLineSize = vecMax - vecMin;

	// modify active side axis by factor
	if ((this->nSide & 1U) == 0U)
		vecMin.y += vecLineSize.y * (1.0f - this->flProgressFactor);
	else
		vecMax.x -= vecLineSize.x * (1.0f - this->flProgressFactor);

	// bar
	D::pDrawListActive->AddRectFilled(vecMin, vecMax, this->colPrimary.GetU32());
}

OVERLAY::CTextComponent::CTextComponent(const EAlignSide nAlignSide, const EAlignDirection nAlignDirection, const ImFont* pFont, const float flFontSize, const char* szText, const Color_t& colPrimary, const float flOutlineThickness, const Color_t& colOutline) :
	pFont(pFont), flFontSize(flFontSize), colPrimary(colPrimary), flOutlineThickness(std::floorf(flOutlineThickness)), colOutline(colOutline)
{
	// allocate own buffer to safely store a copy of the string
	char* szTextCopy = static_cast<char*>(MEM::HeapAlloc(CRT::StringLength(szText) + 1U));
	CRT::StringCopy(szTextCopy, szText);
	this->szText = szTextCopy;

	this->nSide = nAlignSide;
	this->nDirection = nAlignDirection;
	this->vecSize = pFont->CalcTextSizeA(flFontSize, FLT_MAX, 0.0f, szText) + flOutlineThickness;
}

OVERLAY::CTextComponent::~CTextComponent()
{
	// deallocate buffer of the copied string
	MEM::HeapFree(this->szText);
}

void OVERLAY::CTextComponent::Render(const ImVec2& vecPosition)
{
	const ImVec2 vecOutlineOffset = { this->flOutlineThickness, this->flOutlineThickness };

	// @test: used for spacing debugging
	//D::pDrawListActive->AddRect(vecPosition, vecPosition + this->vecSize, IM_COL32(255, 255, 255, 255));

	// @todo: fix this cringe shit after gui merge
	if (this->flOutlineThickness >= 1.0f)
	{
		D::pDrawListActive->AddText(this->pFont, this->flFontSize, vecPosition, this->colOutline.GetU32(), this->szText);
		D::pDrawListActive->AddText(this->pFont, this->flFontSize, vecPosition + vecOutlineOffset * 2.0f, this->colOutline.GetU32(), this->szText);
	}

	D::pDrawListActive->AddText(this->pFont, this->flFontSize, vecPosition + vecOutlineOffset, this->colPrimary.GetU32(), this->szText);
}
#pragma endregion

#pragma region visual_overlay_context
void OVERLAY::Context_t::AddBoxComponent(const ImVec4& vecBox, const VisualOverlayBox_t nType, float flThickness, const Color_t& colPrimary, float flOutlineThickness, const Color_t& colOutline)
{
	flThickness = std::floorf(flThickness);
	flOutlineThickness = std::floorf(flOutlineThickness);

	const float flHalfThickness = flThickness * 0.5f;
	const float flHalfOutlineThickness = flOutlineThickness * 0.5f;
	const ImVec2 vecThicknessOffset = { flHalfThickness + flHalfOutlineThickness, flHalfThickness + flHalfOutlineThickness };

	switch (nType)
	{
	case VISUAL_OVERLAY_BOX_FULL:
	{
		const ImVec2 vecBoxMin = { vecBox[SIDE_LEFT], vecBox[SIDE_TOP] };
		const ImVec2 vecBoxMax = { vecBox[SIDE_RIGHT], vecBox[SIDE_BOTTOM] };

		// inner outline
		D::pDrawListActive->AddRect(vecBoxMin + vecThicknessOffset * 2.0f, vecBoxMax - vecThicknessOffset * 2.0f, colOutline.GetU32(), 0.0f, ImDrawCornerFlags_All, flOutlineThickness);
		// primary box
		D::pDrawListActive->AddRect(vecBoxMin + vecThicknessOffset, vecBoxMax - vecThicknessOffset, colPrimary.GetU32(), 0.0f, ImDrawCornerFlags_All, flThickness);
		// outer outline
		D::pDrawListActive->AddRect(vecBoxMin, vecBoxMax, colOutline.GetU32(), 0.0f, ImDrawCornerFlags_All, flOutlineThickness);

		break;
	}
	case VISUAL_OVERLAY_BOX_CORNERS:
	{
		// corner part of the whole line
		constexpr float flPartRatio = 0.25f;

		const float flCornerWidth = ((vecBox[SIDE_RIGHT] - vecBox[SIDE_LEFT]) * flPartRatio);
		const float flCornerHeight = ((vecBox[SIDE_BOTTOM] - vecBox[SIDE_TOP]) * flPartRatio);

		const ImVec2 arrCornerPoints[4][3] =
		{
			// top-left
			{ ImVec2(vecBox[SIDE_LEFT], vecBox[SIDE_TOP] + flCornerHeight) + vecThicknessOffset, ImVec2(vecBox[SIDE_LEFT], vecBox[SIDE_TOP]) + vecThicknessOffset, ImVec2(vecBox[SIDE_LEFT] + flCornerWidth, vecBox[SIDE_TOP]) + vecThicknessOffset },

			// top-right
			{ ImVec2(vecBox[SIDE_RIGHT] - flCornerWidth - vecThicknessOffset.x, vecBox[SIDE_TOP] + vecThicknessOffset.y * 2.0f), ImVec2(vecBox[SIDE_RIGHT] - vecThicknessOffset.x, vecBox[SIDE_TOP] + vecThicknessOffset.y * 2.0f), ImVec2(vecBox[SIDE_RIGHT] - vecThicknessOffset.x, vecBox[SIDE_TOP] + flCornerHeight + vecThicknessOffset.y * 2.0f) },

			// bottom-left
			{ ImVec2(vecBox[SIDE_LEFT] + flCornerWidth + vecThicknessOffset.x, vecBox[SIDE_BOTTOM] - vecThicknessOffset.y * 2.0f), ImVec2(vecBox[SIDE_LEFT] + vecThicknessOffset.x, vecBox[SIDE_BOTTOM] - vecThicknessOffset.y * 2.0f), ImVec2(vecBox[SIDE_LEFT] + vecThicknessOffset.x, vecBox[SIDE_BOTTOM] - flCornerHeight - vecThicknessOffset.y * 2.0f) },

			// bottom-right
			{ ImVec2(vecBox[SIDE_RIGHT], vecBox[SIDE_BOTTOM] - flCornerHeight) - vecThicknessOffset, ImVec2(vecBox[SIDE_RIGHT], vecBox[SIDE_BOTTOM]) - vecThicknessOffset, ImVec2(vecBox[SIDE_RIGHT] - flCornerWidth, vecBox[SIDE_BOTTOM]) - vecThicknessOffset }
		};

		for (std::size_t i = 0U; i < Q_ARRAYSIZE(arrCornerPoints); i++)
		{
			const auto& arrLinePoints = arrCornerPoints[i];
			const ImVec2 vecHalfPixelOffset = ((i & 1U) == 1U ? ImVec2(-0.5f, -0.5f) : ImVec2(0.5f, 0.5f));

			// @todo: we can even do not clear path and reuse it
			D::pDrawListActive->PathLineTo(arrLinePoints[0] + vecHalfPixelOffset);
			D::pDrawListActive->PathLineTo(arrLinePoints[1] + vecHalfPixelOffset);
			D::pDrawListActive->PathLineTo(arrLinePoints[2] + vecHalfPixelOffset);
			D::pDrawListActive->PathStroke(colOutline.GetU32(), false, flThickness + flOutlineThickness + 1.0f);

			D::pDrawListActive->PathLineTo(arrLinePoints[0] + vecHalfPixelOffset);
			D::pDrawListActive->PathLineTo(arrLinePoints[1] + vecHalfPixelOffset);
			D::pDrawListActive->PathLineTo(arrLinePoints[2] + vecHalfPixelOffset);
			D::pDrawListActive->PathStroke(colPrimary.GetU32(), false, flThickness);
		}

		break;
	}
	default:
		break;
	}

	// accumulate spacing for next side/directional components
	for (float& flSidePadding : this->arrSidePaddings)
		flSidePadding += this->flComponentSpacing;
}

ImVec4 OVERLAY::Context_t::AddFrameComponent(const ImVec2& vecScreen, const EAlignSide nSide, const Color_t& colBackground, const float flRounding, const ImDrawCornerFlags nRoundingCorners)
{
	// calculate frame size by previously added components on active side
	const ImVec2 vecFrameSize = this->GetTotalDirectionalSize(nSide);

	ImVec2 vecFrameMin = { vecScreen.x - vecFrameSize.x * 0.5f, vecScreen.y - vecFrameSize.y };
	ImVec2 vecFrameMax = { vecScreen.x + vecFrameSize.x * 0.5f, vecScreen.y };

	D::pDrawListActive->AddRectFilled(vecFrameMin - this->flComponentSpacing, vecFrameMax + this->flComponentSpacing, colBackground.GetU32(), flRounding, nRoundingCorners);

	// accumulate spacing for next side/directional components
	for (float& flSidePadding : this->arrSidePaddings)
		flSidePadding += this->flComponentSpacing;

	return { vecFrameMin.x, vecFrameMin.y, vecFrameMax.x, vecFrameMax.y };
}

/*
 * @todo: currently not well designed, make it more flexible for use cases where we need e.g. previous frame bar factor etc
 * also to optimize this, allocate components at stack instead of heap + make all context units static and do not realloc components storage every frame, but reset (like memset idk) it at the end of frame
 */
void OVERLAY::Context_t::AddComponent(CBaseComponent* pComponent)
{
	// guarantee that first directional component on each side is in the primary direction
	if (pComponent->IsDirectional())
	{
		CBaseDirectionalComponent* pDirectionalComponent = static_cast<CBaseDirectionalComponent*>(pComponent);

		// check if it's not an exception direction and there are no components in the primary direction
		if (((pDirectionalComponent->nSide & 1U) == 1U || pDirectionalComponent->nDirection != DIR_TOP) && this->arrSideDirectionPaddings[pDirectionalComponent->nSide][pDirectionalComponent->nSide] == 0.0f)
			pDirectionalComponent->nDirection = static_cast<EAlignDirection>(pDirectionalComponent->nSide);
	}

	float& flSidePadding = this->arrSidePaddings[pComponent->nSide];

	if (pComponent->IsDirectional())
	{
		CBaseDirectionalComponent* pDirectionalComponent = static_cast<CBaseDirectionalComponent*>(pComponent);
		float (&arrDirectionPaddings)[DIR_MAX] = this->arrSideDirectionPaddings[pDirectionalComponent->nSide];

		// directional components don't change side paddings, but take them into account
		pComponent->vecOffset[pDirectionalComponent->nSide & 1U] += ((pDirectionalComponent->nSide < 2U) ? -flSidePadding : flSidePadding);

		// check if the component is in the same direction as the side and it's the first component in this direction
		if (static_cast<std::uint8_t>(pDirectionalComponent->nDirection) == static_cast<std::uint8_t>(pDirectionalComponent->nSide) && arrDirectionPaddings[pDirectionalComponent->nDirection] == 0.0f)
		{
			// accumulate paddings for sub-directions
			for (std::uint8_t nSubDirection = DIR_LEFT; nSubDirection < DIR_MAX; nSubDirection++)
			{
				/*
				 * exclude conflicting sub-directions
				 *
				 * SIDE_LEFT[0]: DIR_LEFT[0], DIR_BOTTOM[3] | ~2 & ~1
				 * SIDE_TOP[1]: DIR_LEFT[0], DIR_TOP[1], DIR_RIGHT[2] | ~3
				 * SIDE_RIGHT[2]: DIR_RIGHT[2], DIR_BOTTOM[3] | ~0 & ~1
				 * SIDE_BOTTOM[3]: DIR_LEFT[0], DIR_RIGHT[2], DIR_BOTTOM[3] | ~1
				 */
				if (nSubDirection == pDirectionalComponent->nSide || nSubDirection == ((pDirectionalComponent->nSide + 2U) & 3U) || (nSubDirection == DIR_TOP && (pDirectionalComponent->nSide & 1U) == 0U))
					continue;

				arrDirectionPaddings[nSubDirection] += (pDirectionalComponent->vecSize[nSubDirection == DIR_BOTTOM ? SIDE_TOP : SIDE_LEFT] * (((pDirectionalComponent->nSide & 1U) == 1U) ? 0.5f : 1.0f) + this->flComponentSpacing);
			}
		}

		float& flSideDirectionPadding = arrDirectionPaddings[pDirectionalComponent->nDirection];

		// append direction padding to offset
		pComponent->vecOffset[pDirectionalComponent->nDirection & 1U] += ((pDirectionalComponent->nDirection < 2U) ? -flSideDirectionPadding : flSideDirectionPadding);

		// accumulate direction padding for next component
		flSideDirectionPadding += pDirectionalComponent->vecSize[pDirectionalComponent->nDirection & 1U];

		// accumulate spacing for next directional components
		flSideDirectionPadding += this->flComponentSpacing;
	}
	else
	{
		// append side padding to offset
		pComponent->vecOffset[pComponent->nSide & 1U] += ((pComponent->nSide < 2U) ? -(flSidePadding + pComponent->vecSize[pComponent->nSide]) : flSidePadding);

		// accumulate side padding for next component
		flSidePadding += pComponent->vecSize[pComponent->nSide & 1U];

		// accumulate spacing for next components
		flSidePadding += this->flComponentSpacing;
	}

	this->vecComponents.push_back(pComponent);
}

ImVec2 OVERLAY::Context_t::GetTotalDirectionalSize(const EAlignSide nSide) const
{
	ImVec2 vecSideSize = { };

	// @todo: we should peek max of bottom + side or top directions at horizontal sides
	const float (&arrDirectionPaddings)[DIR_MAX] = this->arrSideDirectionPaddings[nSide];
	for (std::uint8_t nSubDirection = DIR_LEFT; nSubDirection < DIR_MAX; nSubDirection++)
		vecSideSize[nSubDirection & 1U] += arrDirectionPaddings[nSubDirection];

	return vecSideSize;
}

void OVERLAY::Context_t::Render(const ImVec4& vecBox) const
{
	bool bCenteredFirstSideDirectional[SIDE_MAX] = { };

	for (CBaseComponent* const pComponent : this->vecComponents)
	{
		ImVec2 vecPosition = pComponent->GetBasePosition(vecBox);

		// check if the component is in the supported centering side
		if (pComponent->nSide == SIDE_TOP || pComponent->nSide == SIDE_BOTTOM)
		{
			// check if the component is directional and if it is in one of the horizontal directions
			if (CBaseDirectionalComponent* const pDirectionalComponent = static_cast<CBaseDirectionalComponent*>(pComponent); pDirectionalComponent->IsDirectional())
			{
				const float (&arrDirectionPaddings)[DIR_MAX] = this->arrSideDirectionPaddings[pComponent->nSide];

				// @todo: lol does this branch really should look like this? i have no idea how to simplify it, but it looks kinda cringe
				// check if the component is in the same direction as the side and it's the first component in this direction
				if (static_cast<std::uint8_t>(pDirectionalComponent->nDirection) == static_cast<std::uint8_t>(pDirectionalComponent->nSide) && !bCenteredFirstSideDirectional[pDirectionalComponent->nSide])
				{
					// add centering offset to the component's offset
					pDirectionalComponent->vecOffset.x += (arrDirectionPaddings[DIR_LEFT] - arrDirectionPaddings[DIR_RIGHT]) * 0.5f;

					bCenteredFirstSideDirectional[pDirectionalComponent->nSide] = true;
				}
				else if (static_cast<std::uint8_t>(pDirectionalComponent->nDirection) != static_cast<std::uint8_t>(pDirectionalComponent->nSide))
					// add centering offset to the component's offset
					pDirectionalComponent->vecOffset.x += (arrDirectionPaddings[DIR_LEFT] - arrDirectionPaddings[DIR_RIGHT]) * 0.5f;
			}
		}

		// add final component offset to the base position
		vecPosition += pComponent->vecOffset;

		pComponent->Render(vecPosition);
	}
}
#pragma endregion

#pragma region visual_overlay_callbacks
void OVERLAY::OnDraw(CCSPlayer* pLocal)
{
	if (!C::Get<bool>(Vars.bVisual) || !C::Get<bool>(Vars.bVisualOverlay))
		return;

	enum ESortEntityType : int
	{
		SORT_ENTITY_BOMB = 0,
		SORT_ENTITY_BOMB_PLANTED,
		SORT_ENTITY_PLAYER,
		SORT_ENTITY_GRENADE,
		SORT_ENTITY_WEAPON
	};

	struct SortEntityObject_t
	{
		SortEntityObject_t(CBaseEntity* pEntity, const EClassIndex nClassIndex, const ESortEntityType nEntityType, const float flDistance) :
			pEntity(pEntity), nClassIndex(nClassIndex), nEntityType(nEntityType), flDistance(flDistance) { }

		CBaseEntity* pEntity = nullptr;
		EClassIndex nClassIndex = static_cast<EClassIndex>(-1);
		ESortEntityType nEntityType = static_cast<ESortEntityType>(-1);
		float flDistance = 0.0f;
	};
	std::vector<SortEntityObject_t> vecSortedEntities = { };

	const int nHighestEntityIndex = I::ClientEntityList->GetHighestEntityIndex();
	for (int i = 1; i < nHighestEntityIndex; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->Get<CBaseEntity>(i);

		if (pEntity == nullptr)
			continue;

		const CClientClass* pClientClass = pEntity->GetClientClass();

		if (pClientClass == nullptr)
			continue;

		const EClassIndex nClassIndex = pClientClass->nClassID;
		ESortEntityType nEntityType;

		// check for used entities
		if (nClassIndex == EClassIndex::CC4)
			nEntityType = SORT_ENTITY_BOMB;
		else if (nClassIndex == EClassIndex::CPlantedC4)
			nEntityType = SORT_ENTITY_BOMB_PLANTED;
		else if (nClassIndex == EClassIndex::CCSPlayer)
			nEntityType = SORT_ENTITY_PLAYER;
		else if (nClassIndex == EClassIndex::CBaseCSGrenadeProjectile || nClassIndex == EClassIndex::CDecoyProjectile || nClassIndex == EClassIndex::CSnowballProjectile || nClassIndex == EClassIndex::CSmokeGrenadeProjectile || nClassIndex == EClassIndex::CMolotovProjectile || nClassIndex == EClassIndex::CInferno)
			nEntityType = SORT_ENTITY_GRENADE;
		else if (pEntity->IsWeapon())
			nEntityType = SORT_ENTITY_WEAPON;
		else
			continue;

		// store entity and calculated distance for sorting
		vecSortedEntities.emplace_back(pEntity, nClassIndex, nEntityType, (pEntity->GetRenderOrigin() - vecCameraOrigin).Length());
	}

	// sort entities by distance to draw them from the farthest to the nearest
	std::ranges::sort(vecSortedEntities.begin(), vecSortedEntities.end(), std::ranges::greater{ }, &SortEntityObject_t::flDistance);

	for (const auto& [pEntity, nClassIndex, nEntityType, flDistance] : vecSortedEntities)
	{
		switch (nEntityType)
		{
		case SORT_ENTITY_BOMB:
		{
			CC4* pBomb = static_cast<CC4*>(pEntity);

			if (pBomb->IsDormant())
				break;

			// check only for dropped bomb, bomb carrier is handled as player overlay
			if (const IClientEntity* pOwner = I::ClientEntityList->Get(pBomb->GetOwnerEntityHandle()); pOwner != nullptr)
				break;

			Bomb(pBomb);
			break;
		}
		case SORT_ENTITY_BOMB_PLANTED:
		{
			// cast to planted bomb entity
			CPlantedC4* pBomb = reinterpret_cast<CPlantedC4*>(pEntity);

			if (!pBomb->IsPlanted() || pBomb->IsDormant())
				break;

			PlantedBomb(pBomb);
			break;
		}
		case SORT_ENTITY_PLAYER:
		{
			CCSPlayer* pCSPlayer = static_cast<CCSPlayer*>(pEntity);

			if (!pCSPlayer->IsAlive() || pCSPlayer->IsDormant())
				break;

			if (!pLocal->IsAlive())
			{
				// check is not spectating current entity
				if (const CBaseHandle& hObserver = pLocal->GetObserverTargetHandle(); hObserver.IsValid() && hObserver == pCSPlayer->GetRefEHandle() && pLocal->GetObserverMode() == OBS_MODE_IN_EYE)
					break;
			}

			Player(pLocal, pCSPlayer, flDistance);
			break;
		}
		case SORT_ENTITY_GRENADE:
		{
			if (pEntity->IsDormant())
				break;

			Grenade(pEntity, nClassIndex);
			break;
		}
		case SORT_ENTITY_WEAPON:
		{
			CBaseCombatWeapon* pWeapon = static_cast<CBaseCombatWeapon*>(pEntity);

			if (pWeapon->IsDormant())
				break;

			// check only for dropped weapons
			if (pEntity->GetOwnerEntityHandle().IsValid())
				break;

			DroppedWeapon(pWeapon, flDistance);
			break;
		}
		default:
			Q_ASSERT(false); // not handled sorted entity type
			break;
		}
	}
}
#pragma endregion

#pragma region visual_overlay_get
bool OVERLAY::GetEntityBoundingBox(CBaseEntity* pEntity, ImVec4* pvecBox)
{
	const ICollideable* pCollideable = pEntity->GetCollideable();

	if (pCollideable == nullptr)
		return false;

	// get mins/maxs
	const Vector_t vecMin = pCollideable->OBBMins();
	const Vector_t vecMax = pCollideable->OBBMaxs();

	/*
	 * build AABB points
	 *
	 * points navigation:
	 * [N] [back/front][left/right][bottom/top]
	 *	0 - blb
	 *	1 - brb
	 *	2 - frb
	 *	3 - flb
	 *	4 - frt
	 *	5 - brt
	 *	6 - blt
	 *	7 - flt
	 */
	const Vector_t arrPoints[8] =
	{
		{ vecMin.x, vecMin.y, vecMin.z },
		{ vecMin.x, vecMax.y, vecMin.z },
		{ vecMax.x, vecMax.y, vecMin.z },
		{ vecMax.x, vecMin.y, vecMin.z },
		{ vecMax.x, vecMax.y, vecMax.z },
		{ vecMin.x, vecMax.y, vecMax.z },
		{ vecMin.x, vecMin.y, vecMax.z },
		{ vecMax.x, vecMin.y, vecMax.z }
	};

	// get transformation matrix
	const Matrix3x4_t& matTransformed = pEntity->GetCoordinateFrame();

	float flLeft = (std::numeric_limits<float>::max)();
	float flTop = (std::numeric_limits<float>::max)();
	float flRight = (std::numeric_limits<float>::lowest)();
	float flBottom = (std::numeric_limits<float>::lowest)();

	// get screen points position
	ImVec2 arrScreen[8] = { };
	for (std::size_t i = 0U; i < 8U; i++)
	{
		if (!D::WorldToScreen(arrPoints[i].Transform(matTransformed), &arrScreen[i]))
			return false;

		flLeft = std::floorf(CRT::Min(flLeft, arrScreen[i].x));
		flTop = std::floorf(CRT::Min(flTop, arrScreen[i].y));
		flRight = std::floorf(CRT::Max(flRight, arrScreen[i].x));
		flBottom = std::floorf(CRT::Max(flBottom, arrScreen[i].y));
	}

	// set calculated box
	(*pvecBox)[SIDE_LEFT] = flLeft;
	(*pvecBox)[SIDE_TOP] = flTop;
	(*pvecBox)[SIDE_RIGHT] = flRight;
	(*pvecBox)[SIDE_BOTTOM] = flBottom;
	return true;
}
#pragma endregion

#pragma region visual_overlay_main
void OVERLAY::Player(CCSPlayer* pLocal, CCSPlayer* pPlayer, const float flDistance)
{
	bool bIsLocal = (pPlayer == pLocal && pLocal->ShouldDraw());
	bool bIsEnemy = (pLocal->IsOtherEnemy(pPlayer));
	bool bIsAlly = (!bIsLocal && !bIsEnemy);

	bIsLocal &= C::Get<bool>(Vars.bVisualOverlayLocal);
	bIsEnemy &= C::Get<bool>(Vars.bVisualOverlayEnemies);
	bIsAlly &= C::Get<bool>(Vars.bVisualOverlayAllies);

	// check is at least one of filters enabled
	if (!bIsLocal && !bIsEnemy && !bIsAlly)
		return;

	PlayerInfo_t playerInfo = { };
	if (!I::Engine->GetPlayerInfo(pPlayer->GetIndex(), &playerInfo))
		return;

	ImVec4 vecBox = { };
	if (!GetEntityBoundingBox(pPlayer, &vecBox))
		return;

	Context_t context = { };

	if (C::Get<int>(Vars.iVisualOverlayPlayerBox) != VISUAL_OVERLAY_BOX_NONE)
	{
		// @todo: another way to determine visibility
		const bool bIsVisible = pLocal->IsOtherVisible(pPlayer, pPlayer->GetBonePosition(BONE_HEAD));

		// get box color based on visibility & enmity
		const Color_t colBoxVisible = (bIsLocal ? C::Get<Color_t>(Vars.colVisualOverlayBoxLocal) : (bIsEnemy ? C::Get<Color_t>(Vars.colVisualOverlayBoxEnemies) : C::Get<Color_t>(Vars.colVisualOverlayBoxAllies)));
		const Color_t colBoxHidden = (bIsLocal ? C::Get<Color_t>(Vars.colVisualOverlayBoxLocalHidden) : (bIsEnemy ? C::Get<Color_t>(Vars.colVisualOverlayBoxEnemiesHidden) : C::Get<Color_t>(Vars.colVisualOverlayBoxAlliesHidden)));

		context.AddBoxComponent(vecBox, C::Get<int>(Vars.iVisualOverlayPlayerBox), 1.0f, (bIsVisible ? colBoxVisible : colBoxHidden), 1.0f, Color_t(0, 0, 0, 150));
	}

	// info's master check
	if (!C::Get<bool>(Vars.bVisualOverlayPlayerInfo))
		return;

	// @note: distance font scale
	const float flFontSize = CRT::Clamp(90.f / (flDistance / 90.f), 10.f, 40.f);

	#pragma region visuals_player_top
	if (C::Get<bool>(Vars.bVisualOverlayPlayerFlash) && pPlayer->GetFlashDuration() > 0.2f)
		context.AddComponent(new CBarComponent(SIDE_TOP, vecBox, pPlayer->GetFlashAlpha() / pPlayer->GetFlashMaxAlpha(), 1.0f, Color_t(220, 220, 220, 255), Color_t(40, 40, 40, 100), 1.0f, Color_t(0, 0, 0, 150)));

	if (C::Get<bool>(Vars.bVisualOverlayPlayerName))
	{
		// @test: possible UB caused by using stack allocated buffer | check similar places for same shit
		// get player name
		char szNameBuffer[32];
		const char* szNameEnd = CRT::StringCopyN(szNameBuffer, playerInfo.szName, sizeof(szNameBuffer));

		// truncate name
		constexpr int nTruncateLength = 28;
		if ((szNameEnd - szNameBuffer) > nTruncateLength)
			szNameEnd = CRT::StringCat(szNameBuffer + nTruncateLength, Q_XOR("..."));

		context.AddComponent(new CTextComponent(SIDE_TOP, DIR_TOP, FONT::pVisual, flFontSize, szNameBuffer, Color_t(255, 255, 255, 255), 1.0f, Color_t(0, 0, 0, 220)));

		// add prefix for bots
		if (playerInfo.bFakePlayer)
			context.AddComponent(new CTextComponent(SIDE_TOP, DIR_LEFT, FONT::pVisual, flFontSize, Q_XOR("[BOT]"), Color_t(140, 140, 140), 1.0f, Color_t(0, 0, 0, 220)));
	}
	#pragma endregion

	#pragma region visuals_player_bottom
	// get active weapon
	if (CBaseCombatWeapon* pActiveWeapon = pPlayer->GetActiveWeapon(); pActiveWeapon != nullptr)
	{
		// ammo bar
		if (C::Get<bool>(Vars.bVisualOverlayPlayerAmmo))
		{
			// check is weapon gun
			if (const CCSWeaponData* pActiveWeaponData = I::WeaponSystem->GetWeaponData(pActiveWeapon->GetEconItemView()->GetItemDefinitionIndex()); pActiveWeaponData != nullptr && pActiveWeaponData->IsGun())
			{
				float flProgressFactor;

				// check for reloading animation
				if (const CAnimationLayer& layer = pPlayer->GetAnimationOverlays()[1]; pPlayer->GetSequenceActivity(layer.nSequence) == ACT_CSGO_RELOAD && layer.flWeight != 0.0f)
					// use reload animation cycle based factor
					flProgressFactor = layer.flCycle;
				else
					// use ammo count based factor
					flProgressFactor = static_cast<float>(pActiveWeapon->GetAmmo()) / static_cast<float>(pActiveWeaponData->iMaxClip1);

				context.AddComponent(new CBarComponent(SIDE_BOTTOM, vecBox, flProgressFactor, 1.0f, Color_t(80, 180, 200), Color_t(40, 40, 40, 100), 1.0f, Color_t(0, 0, 0, 150)));
			}
		}

		// get all other weapons
		if (C::Get<bool>(Vars.bVisualOverlayPlayerWeapons))
		{
			const CBaseHandle* hPlayerWeapons = pPlayer->GetWeaponsHandle();

			// -1 to prevent double active weapon
			for (int i = MAX_WEAPONS - 1; i > 0; --i)
			{
				CBaseCombatWeapon* pCurrentWeapon = I::ClientEntityList->Get<CBaseCombatWeapon>(hPlayerWeapons[i]);

				if (pCurrentWeapon == nullptr)
					continue;

				const ItemDefinitionIndex_t nDefinitionIndex = pCurrentWeapon->GetEconItemView()->GetItemDefinitionIndex();

				// do not draw some useless dangerzone weapons (lmao i just didnt add icons for them)
				if (nDefinitionIndex == WEAPON_SHIELD || nDefinitionIndex == WEAPON_BREACHCHARGE || nDefinitionIndex == WEAPON_BUMPMINE)
					continue;

				// pass only active grenades/fists/tablet
				if (const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nDefinitionIndex); pWeaponData == nullptr || ((pWeaponData->nWeaponType == WEAPONTYPE_GRENADE || nDefinitionIndex == WEAPON_FISTS || nDefinitionIndex == WEAPON_TABLET) && pCurrentWeapon != pActiveWeapon))
					continue;

				// draw weapons list
				context.AddComponent(new CTextComponent(SIDE_BOTTOM, DIR_BOTTOM, FONT::pIcons, flFontSize, reinterpret_cast<const char*>(D::GetWeaponIcon(nDefinitionIndex)), pCurrentWeapon == pActiveWeapon ? Color_t(255, 255, 255, 255) : Color_t(160, 160, 160), 1.0f, Color_t(0, 0, 0, 220)));
			}
		}
	}

	if (C::Get<bool>(Vars.bVisualOverlayPlayerDistance))
	{
		const int iDistance = static_cast<int>(flDistance * METERS_PER_INCH);
		char szDistanceBuffer[CRT::IntegerToString_t<int, 10>::MaxCount() + 1U];
		char* szDistance = CRT::IntegerToString(iDistance, szDistanceBuffer, sizeof(szDistanceBuffer), 10);

		const std::size_t nDistanceLength = szDistanceBuffer + sizeof(szDistanceBuffer) - szDistance - 1U;
		szDistance = static_cast<char*>(CRT::MemoryMove(szDistance - 1U, szDistance, nDistanceLength)); // @note: this relies on current 'CRT::IntegerToString' behaviour, be careful
		szDistance[nDistanceLength] = 'M';

		context.AddComponent(new CTextComponent(SIDE_BOTTOM, DIR_BOTTOM, FONT::pVisual, flFontSize, szDistance, Color_t(255, 255, 255, 255), 1.0f, Color_t(0, 0, 0, 220)));
	}
	#pragma endregion

	#pragma region visuals_player_left
	if (C::Get<bool>(Vars.bVisualOverlayPlayerHealth))
	{
		const float flProgressFactor = static_cast<float>(pPlayer->GetHealth()) / static_cast<float>(pPlayer->GetMaxHealth());
		context.AddComponent(new CBarComponent(SIDE_LEFT, vecBox, flProgressFactor, 1.0f, Color_t::FromHSB((flProgressFactor * 120.f) / 360.f, 1.0f, 1.0f), Color_t(40, 40, 40, 100), 1.0f, Color_t(0, 0, 0, 150)));
	}

	if (C::Get<bool>(Vars.bVisualOverlayPlayerMoney))
	{
		char szMoneyBuffer[CRT::IntegerToString_t<std::uint16_t, 10>::MaxCount() + 1U];
		char* szMoney = CRT::IntegerToString(pPlayer->GetMoney(), szMoneyBuffer, sizeof(szMoneyBuffer), 10);
		*--szMoney = '$'; // @note: this relies on current 'CRT::IntegerToString' behaviour, be careful

		context.AddComponent(new CTextComponent(SIDE_LEFT, DIR_BOTTOM, FONT::pVisual, flFontSize, szMoney, Color_t(140, 195, 75, 255), 1.0f, Color_t(0, 0, 0, 220)));
	}
	#pragma endregion

	#pragma region visuals_player_right
	if ((C::Get<unsigned int>(Vars.nVisualOverlayPlayerFlags) & VISUAL_OVERLAY_PLAYER_FLAG_HELMET) && pPlayer->HasHelmet())
		context.AddComponent(new CTextComponent(SIDE_RIGHT, DIR_BOTTOM, FONT::pIcons, flFontSize, Q_XOR("\xEE\x88\x8E"), Color_t(255, 255, 255, 255), 1.0f, Color_t(0, 0, 0, 220)));

	if ((C::Get<unsigned int>(Vars.nVisualOverlayPlayerFlags) & VISUAL_OVERLAY_PLAYER_FLAG_KEVLAR) && pPlayer->GetArmor() > 0)
		context.AddComponent(new CTextComponent(SIDE_RIGHT, DIR_BOTTOM, FONT::pIcons, flFontSize, Q_XOR("\xEE\x88\x90"), Color_t(255, 255, 255, 255), 1.0f, Color_t(0, 0, 0, 220)));

	if ((C::Get<unsigned int>(Vars.nVisualOverlayPlayerFlags) & VISUAL_OVERLAY_PLAYER_FLAG_KIT) && pPlayer->HasDefuser())
		context.AddComponent(new CTextComponent(SIDE_RIGHT, DIR_BOTTOM, FONT::pIcons, flFontSize, Q_XOR("\xEE\x88\x8F"), pPlayer->IsDefusing() ? Color_t(80, 180, 200) : Color_t(255, 255, 255, 255), 1.0f, Color_t(0, 0, 0, 220)));

	if ((C::Get<unsigned int>(Vars.nVisualOverlayPlayerFlags) & VISUAL_OVERLAY_PLAYER_FLAG_ZOOM) && pPlayer->IsScoped())
		context.AddComponent(new CTextComponent(SIDE_RIGHT, DIR_BOTTOM, FONT::pIcons, flFontSize, Q_XOR("\xEE\x88\x92"), Color_t(255, 255, 255, 255), 1.0f, Color_t(0, 0, 0, 220)));
	#pragma endregion

	context.Render(vecBox);
}

void OVERLAY::Bomb(CC4* pBomb)
{
	if (!C::Get<bool>(Vars.bVisualOverlayBomb))
		return;

	ImVec2 vecScreen = { };
	if (!D::WorldToScreen(pBomb->GetOrigin(), &vecScreen))
		return;

	Context_t context = { };

	// label
	context.AddComponent(new CTextComponent(SIDE_TOP, DIR_TOP, FONT::pExtra, 14.f, Q_XOR("C4"), Color_t(255, 255, 255, 255)));
	// icon
	context.AddComponent(new CTextComponent(SIDE_TOP, DIR_LEFT, FONT::pIcons, 14.f, reinterpret_cast<const char*>(D::GetWeaponIcon(WEAPON_C4)), Color_t(255, 255, 255, 255)));
	// frame
	const ImVec4 vecFrameBox = context.AddFrameComponent(vecScreen, SIDE_TOP, Color_t(40, 40, 40, 200), 5.0f, ImDrawCornerFlags_All);

	context.Render({ vecFrameBox.x, vecFrameBox.w, vecFrameBox.z, vecFrameBox.w });
}

void OVERLAY::PlantedBomb(CPlantedC4* pBomb)
{
	if (!C::Get<bool>(Vars.bVisualOverlayBomb))
		return;

	ImVec2 vecScreen = { };
	if (!D::WorldToScreen(pBomb->GetOrigin(), &vecScreen))
		return;

	Context_t context = { };

	// get defuser entity
	const IClientEntity* pDefuser = I::ClientEntityList->Get(pBomb->GetDefuserHandle());

	// label
	context.AddComponent(new CTextComponent(SIDE_TOP, DIR_TOP, FONT::pExtra, 14.f, Q_XOR("PLANTED C4"), Color_t(255, 255, 255, 255)));
	// icon
	context.AddComponent(new CTextComponent(SIDE_TOP, DIR_LEFT, FONT::pIcons, 14.f, reinterpret_cast<const char*>(D::GetWeaponIcon(WEAPON_C4)), pDefuser != nullptr ? Color_t(80, 180, 200, 200) : Color_t(255, 255, 255, 255)));
	// frame
	const ImVec4 vecFrameBox = context.AddFrameComponent(vecScreen, SIDE_TOP, Color_t(40, 40, 40, 200), 5.0f, pBomb->IsBombActive() ? ImDrawCornerFlags_Top : ImDrawCornerFlags_All);

	if (pBomb->IsBombActive())
	{
		const float flBlowLength = pBomb->GetBlowLength();
		const float flBlowTime = CRT::Clamp(pBomb->GetBlowCountDown() - I::Globals->flCurrentTime, 0.0f, flBlowLength);

		// calculate bomb timer-based width factor
		const float flBlowFactor = flBlowTime / flBlowLength;
		context.AddComponent(new CBarComponent(SIDE_BOTTOM, vecFrameBox, flBlowFactor, 1.0f, Color_t::FromHSB((flBlowFactor * 120.f) / 360.f, 1.0f, 1.0f), Color_t(0, 0, 0, 100), 1.0f, Color_t(0, 0, 0, 100)));

		// check does someone is defusing the bomb at the moment
		if (pDefuser != nullptr)
		{
			const float flDefuseLength = pBomb->GetDefuseLength();
			const float flDefuseTime = CRT::Clamp(pBomb->GetDefuseCountDown() - I::Globals->flCurrentTime, 0.0f, flDefuseLength);

			context.AddComponent(new CBarComponent(SIDE_BOTTOM, vecFrameBox, flDefuseTime / flDefuseLength, 1.0f, (flDefuseTime < flBlowTime) ? Color_t(80, 180, 200, 200) : Color_t(255, 100, 100, 255), Color_t(0, 0, 0, 100), 1.0f, Color_t(0, 0, 0, 100)));
		}
	}

	context.Render({ vecFrameBox.x, vecFrameBox.w, vecFrameBox.z, vecFrameBox.w });
}

void OVERLAY::Grenade(CBaseEntity* pGrenade, const EClassIndex nIndex)
{
	if (!C::Get<bool>(Vars.bVisualOverlayGrenades))
		return;

	ImVec2 vecScreen = { };
	if (!D::WorldToScreen(pGrenade->GetOrigin(), &vecScreen))
		return;

	const char* szName = nullptr;
	float flExpireFactor = 0.0f;
	Color_t colGrenade = Color_t(255, 255, 255);

	// get info of the specific grenade
	// @test: is it ok to store string by ptr with xoring? | probably it is and behave same as any other stack allocated variable
	switch (nIndex)
	{
	case EClassIndex::CBaseCSGrenadeProjectile:
	{
		const char* szModelName = I::ModelInfo->GetModelName(pGrenade->GetModel());

		if (CRT::StringString(szModelName, Q_XOR("fraggrenade")) != nullptr)
			szName = Q_XOR("HIGH-EXPLOSIVE");
		else if (CRT::StringString(szModelName, Q_XOR("flashbang")) != nullptr)
			szName = Q_XOR("FLASH");
		else
			// not supported grenade type
			return;

		break;
	}
	case EClassIndex::CDecoyProjectile:
		szName = Q_XOR("DECOY");
		break;
	case EClassIndex::CSnowballProjectile:
		szName = Q_XOR("SNOWBALL");
		break;
	case EClassIndex::CSmokeGrenadeProjectile:
	{
		CSmokeGrenadeProjectile* pSmoke = reinterpret_cast<CSmokeGrenadeProjectile*>(pGrenade);
		flExpireFactor = 1.0f - ((I::Globals->flCurrentTime - TICKS_TO_TIME(pSmoke->GetSmokeEffectTickBegin())) / CSmokeGrenadeProjectile::GetMaxTime());
		colGrenade = Color_t(230, 130, 0);
		szName = Q_XOR("SMOKE");
		break;
	}
	case EClassIndex::CMolotovProjectile:
	{
		CMolotovProjectile* pMolotov = reinterpret_cast<CMolotovProjectile*>(pGrenade);
		szName = (pMolotov->IsIncendiaryGrenade() ? Q_XOR("INCENDIARY") : Q_XOR("MOLOTOV"));
		break;
	}
	case EClassIndex::CInferno:
	{
		CInferno* pInferno = reinterpret_cast<CInferno*>(pGrenade);
		flExpireFactor = 1.0f - ((I::Globals->flCurrentTime - TICKS_TO_TIME(pInferno->GetEffectTickBegin())) / CInferno::GetMaxTime());
		colGrenade = Color_t(255, 100, 100);
		szName = Q_XOR("FIRE");
		break;
	}
	default:
		// not supported grenade type
		return;
	}

	Context_t context = { };

	// label
	context.AddComponent(new CTextComponent(SIDE_TOP, DIR_TOP, FONT::pExtra, 14.f, szName, Color_t(255, 255, 255, 255)));
	// frame
	const ImVec4 vecFrameBox = context.AddFrameComponent(vecScreen, SIDE_TOP, Color_t(20, 20, 20, 150), 5.0f, flExpireFactor > 0.0f ? ImDrawCornerFlags_Top : ImDrawCornerFlags_All);

	if (flExpireFactor > 0.0f)
		context.AddComponent(new CBarComponent(SIDE_TOP, vecFrameBox, flExpireFactor, 1.0f, colGrenade, Color_t(40, 40, 40, 100), 1.0f, Color_t(0, 0, 0, 100)));

	context.Render({ vecFrameBox.x, vecFrameBox.w, vecFrameBox.z, vecFrameBox.w });
}

void OVERLAY::DroppedWeapon(CBaseCombatWeapon* pWeapon, const float flDistance)
{
	if (!C::Get<bool>(Vars.bVisualOverlayWeapons))
		return;

	ImVec4 vecBox = { };
	if (!GetEntityBoundingBox(pWeapon, &vecBox))
		return;

	constexpr Color_t colInfo = Color_t(255, 255, 255, 200);
	constexpr Color_t colOutline = Color_t(0, 0, 0, 150);

	Context_t context = { };

	if (C::Get<int>(Vars.iVisualOverlayWeaponBox) != VISUAL_OVERLAY_BOX_NONE)
		context.AddBoxComponent(vecBox, C::Get<int>(Vars.iVisualOverlayWeaponBox), 1.0f, C::Get<Color_t>(Vars.colVisualOverlayBoxWeapons), 1.0f, colOutline);

	// info's master check
	if (C::Get<bool>(Vars.bVisualOverlayWeaponInfo))
	{
		const ItemDefinitionIndex_t nItemDefinitionIndex = pWeapon->GetEconItemView()->GetItemDefinitionIndex();

		// @note: for text weapon names
		//wchar_t* wszWeaponName = I::Localize->Find(pWeaponData->szHudName);

		if (C::Get<bool>(Vars.bVisualOverlayWeaponAmmo))
		{
			// check is weapon gun
			if (const CCSWeaponData* pWeaponData = I::WeaponSystem->GetWeaponData(nItemDefinitionIndex); pWeaponData != nullptr && pWeaponData->IsGun())
				context.AddComponent(new CBarComponent(SIDE_BOTTOM, vecBox, static_cast<float>(pWeapon->GetAmmo()) / static_cast<float>(pWeaponData->iMaxClip1), 1.0f, Color_t(80, 180, 200), Color_t(40, 40, 40, 100), 1.0f, colOutline));
		}

		if (C::Get<bool>(Vars.bVisualOverlayWeaponIcon))
			context.AddComponent(new CTextComponent(SIDE_BOTTOM, DIR_BOTTOM, FONT::pIcons, 10.f, reinterpret_cast<const char*>(D::GetWeaponIcon(nItemDefinitionIndex)), colInfo, 1.0f, colOutline));

		if (C::Get<bool>(Vars.bVisualOverlayWeaponDistance))
		{
			const int iDistance = static_cast<int>(flDistance * METERS_PER_INCH);

			char szDistanceBuffer[CRT::IntegerToString_t<int, 10>::MaxCount() + 1U];
			char* szDistance = CRT::IntegerToString(iDistance, szDistanceBuffer, sizeof(szDistanceBuffer), 10);

			const std::size_t nDistanceLength = szDistanceBuffer + sizeof(szDistanceBuffer) - szDistance - 1U;
			szDistance = static_cast<char*>(CRT::MemoryMove(szDistance - 1U, szDistance, nDistanceLength)); // @note: this relies on current 'CRT::IntegerToString' behaviour, be careful
			szDistance[nDistanceLength] = 'M';

			context.AddComponent(new CTextComponent(SIDE_BOTTOM, DIR_BOTTOM, FONT::pVisual, 12.f, szDistance, colInfo, 1.0f, colOutline));
		}
	}

	context.Render(vecBox);
}
#pragma endregion
