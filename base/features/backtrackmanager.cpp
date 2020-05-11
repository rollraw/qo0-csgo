#include "..\sdk.h"

bool CBackTrackManager::IsVisibleTick(RecordTick_t tick, bool smokecheck, bool bodycheck)
{
	CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(tick.iIndex);
	if (!pEntity || !pEntity->IsAlive() || !pEntity->IsPlayer())
		return false;

	bool bVisible = false;

	Vector vecPoints[] = {
	Vector(tick.vecMin.x, tick.vecMin.y, tick.vecMin.z),
	Vector(tick.vecMin.x, tick.vecMax.y, tick.vecMin.z),
	Vector(tick.vecMax.x, tick.vecMax.y, tick.vecMin.z),
	Vector(tick.vecMax.x, tick.vecMin.y, tick.vecMin.z),
	Vector(tick.vecMax.x, tick.vecMax.y, tick.vecMax.z),
	Vector(tick.vecMin.x, tick.vecMax.y, tick.vecMax.z),
	Vector(tick.vecMin.x, tick.vecMin.y, tick.vecMax.z),
	Vector(tick.vecMax.x, tick.vecMin.y, tick.vecMax.z)
	};
		for (Vector CheckPos : vecPoints)
		{
			Ray_t ray;
			ray.Init(G::LocalPlayer->GetEyePosition(), CheckPos);
			CTraceFilter filter;
			filter.pSkip = G::LocalPlayer;

			Trace_t trace;
			I::EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);
			if (trace.flFraction > .95f)
			{
				bVisible = true;
				break;
			}
		}

	if (bVisible && bodycheck)
	{
		Ray_t ray;
		ray.Init(G::LocalPlayer->GetEyePosition(), pEntity->GetBonePosition(8, tick.boneMatrix));
		CTraceFilter filter;
		filter.pSkip = G::LocalPlayer;

		Trace_t trace;
		I::EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &trace);
		bVisible = bVisible && trace.flFraction > .95f;
	}

	//if (smokecheck)
	//	bVisible = bVisible && !U::LineGoesThroughSmoke(G::LocalPlayer->GetEyePosition(), pEntity->GetBonePosition(8, tick.boneMatrix));

	return bVisible;
}

void CBackTrackManager::SetRecord(CBaseEntity * pEntity, RecordTick_t tick)
{
	if (pEntity->GetIndex() != tick.iIndex) 
		return;	
	pEntity->InvalidateBoneCache();
	pEntity->SetAbsOrigin(tick.vecOrigin);
	*pEntity->GetFlagsPointer() = tick.iFlags;
	*pEntity->GetEyeAnglesPointer() = tick.angEyeAngle;
	pEntity->SetAbsAngles(tick.angAbsAngle);
	*pEntity->GetLowerBodyYawPointer() = tick.flLowerBodyYaw;
	*pEntity->GetVelocityPointer() = tick.vecVelocity;
	//for (int i = 0; i < 24; i++)
	//	*(float*)((DWORD)pEntity + offsets.m_flPoseParameter + sizeof(float) * i) = tick.flPoseParameter[i];
	for (int i = 0; i < tick.iLayerCount; i++)
	{
		CAnimationLayer* Layer = pEntity->GetAnimationLayer(i);
		Layer->flCycle = tick.cAnimationLayer[i].flCycle;
		Layer->flWeight = tick.cAnimationLayer[i].flWeight;
		Layer->iOrder = tick.cAnimationLayer[i].iOrder;
		Layer->nSequence = tick.cAnimationLayer[i].nSequence;
	}
}

void CBackTrackManager::UpdateTicks()
{
	for (int i = 0; i < I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(i);
		if (!pEntity->IsValid())
			continue;
		RecordedTicks[pEntity->GetIndex()].push_back(RecordTick_t(pEntity));
		while (!RecordedTicks[pEntity->GetIndex()].empty() && abs(RecordedTicks[pEntity->GetIndex()][0].flSimulationTime - pEntity->GetSimulationTime()) > 1)
			RecordedTicks[pEntity->GetIndex()].erase(RecordedTicks[pEntity->GetIndex()].begin());
	}
}

int CBackTrackManager::GetEstimatedServerTickCount(float latency)
{
	return TIME_TO_TICKS(latency) + 1 + I::Globals->iTickCount;
}

float CBackTrackManager::GetLerpTime()
{
	static ConVar* minupdate, *maxupdate, *updaterate, *interprate, *cmin, *cmax, *interp;
	if (!minupdate)
		minupdate = I::CVar->FindVar(XorStr("sv_minupdaterate"));
	if (!maxupdate)
		maxupdate = I::CVar->FindVar(XorStr("sv_maxupdaterate"));
	if (!updaterate)
		updaterate = I::CVar->FindVar(XorStr("cl_updaterate"));
	if (!interprate)
		interprate = I::CVar->FindVar(XorStr("cl_interp_ratio"));
	if (!cmin)
		cmin = I::CVar->FindVar(XorStr("sv_client_min_interp_ratio"));
	if (!cmax)
		cmax = I::CVar->FindVar(XorStr("sv_client_max_interp_ratio"));
	if (!interp)
		interp = I::CVar->FindVar(XorStr("cl_interp"));

	float UpdateRate = updaterate->flValue;
	float LerpRatio = interprate->flValue;

	return std::min<float>(LerpRatio / UpdateRate, interp->flValue);
}

bool CBackTrackManager::IsValidTick(RecordTick_t tick)
{
	float outlatency;
	float inlatency;
	INetChannelInfo *nci = I::Engine->GetNetChannelInfo();
	if (nci)
	{
		inlatency = nci->GetLatency(FLOW_INCOMING);
		outlatency = nci->GetLatency(FLOW_OUTGOING);
	}
	else
		inlatency = outlatency = 0.0f;

	float totaloutlatency = outlatency;
	if (nci)
		totaloutlatency += nci->GetAvgLatency(FLOW_OUTGOING); //net_graph method

	float servertime = TICKS_TO_TIME(GetEstimatedServerTickCount(outlatency + inlatency));

	float correct = std::clamp<float>(totaloutlatency + GetLerpTime(), .0f, 1.0f);
	int iTargetTick = TIME_TO_TICKS(tick.flSimulationTime);
	float deltaTime = correct - (servertime - TICKS_TO_TIME(iTargetTick));
	return (fabsf(deltaTime) <= 0.2f);
}

RecordTick_t CBackTrackManager::GetLastValidRecord(CBaseEntity * pEntity)
{
	for (int i = 0; i < RecordedTicks[pEntity->GetIndex()].size(); i++)
	{
		if (IsValidTick(RecordedTicks[pEntity->GetIndex()][i]))
			return RecordedTicks[pEntity->GetIndex()][i];
	}
	return RecordTick_t(pEntity);
}

std::vector<RecordTick_t> CBackTrackManager::GetLastValidRecords()
{
	std::vector<RecordTick_t> records;
	for (int i = 0; i < I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(i);
		if (!pEntity->IsValid())
			continue;
		records.push_back(GetLastValidRecord(pEntity));
	}
	return records;
}

void CBackTrackManager::test(CUserCmd* cmd)
{
	for (int i = 0; i < I::Globals->nMaxClients; i++)
	{
		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(i);
		if (!pEntity->IsValid() || pEntity == G::LocalPlayer)
			continue;
		std::vector<RecordTick_t> Ticks = GetRecords(pEntity);
		RecordTick_t Backup(pEntity);
		studiohdr_t* pStudioHdr = I::ModelInfo->GetStudioModel(pEntity->GetModel());

		if (pStudioHdr == nullptr)
			return;

		mstudiohitboxset_t* pHitboxSet = pStudioHdr->GetHitboxSet(0);

		if (pHitboxSet == nullptr)
			return;

		for (auto Tick : Ticks)
		{
			SetRecord(pEntity, Tick);
			
			/*for (int i = 0; i < pHitboxSet->nHitboxes; i++)
			{
				mstudiobbox_t* pStudioHitbox = pHitboxSet->GetHitbox(i);
				if (pStudioHitbox == nullptr) continue;

				Vector vecMin = M::VectorTransform(pStudioHitbox->BBMin, Tick.boneMatrix[pStudioHitbox->iBone]);
				Vector vecMax = M::VectorTransform(pStudioHitbox->BBMax, Tick.boneMatrix[pStudioHitbox->iBone]);
				if (IsValidTick(Tick))
					I::DebugOverlay->AddCapsuleOverlay(vecMin, vecMax, pStudioHitbox->flRadius, 0, 255, 0, 255, .07);
				else
					I::DebugOverlay->AddCapsuleOverlay(vecMin, vecMax, pStudioHitbox->flRadius, 255, 0, 0, 255, .07);
			}*/
		}
		SetRecord(pEntity, Backup);
		pEntity->UpdateClientSideAnimation();
	}
}

RecordTick_t::RecordTick_t(CBaseEntity * pEntity)
{
	iIndex = pEntity->GetIndex();
	flSimulationTime = pEntity->GetSimulationTime();
	vecOrigin = pEntity->GetAbsOrigin();
	angEyeAngle = pEntity->GetEyeAngles();
	angAbsAngle = pEntity->GetAbsAngles();
	vecVelocity = pEntity->GetVelocity();
	iFlags = pEntity->GetFlags();
	flLowerBodyYaw = pEntity->GetLowerBodyYaw();
	iLayerCount = pEntity->GetNumAnimationOverlays();

	for (int i = 0; i < 24; i++)
		flPoseParameter[i] = *(float*)((DWORD)pEntity + offsets.m_flPoseParameter + sizeof(float) * i);
	pEntity->SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, I::Globals->flCurrentTime);
	
	for (int i = 0; i < iLayerCount; i++)
	{
		CAnimationLayer* Layer = pEntity->GetAnimationLayer(i);
		cAnimationLayer[i].flCycle = Layer->flCycle;
		cAnimationLayer[i].flWeight = Layer->flWeight;
		cAnimationLayer[i].iOrder = Layer->iOrder;
		cAnimationLayer[i].nSequence = Layer->nSequence;
	}
	pEntity->ComputeHitboxSurroundingBox(&vecMin, &vecMax);
}

