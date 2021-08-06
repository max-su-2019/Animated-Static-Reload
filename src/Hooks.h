#pragma once

class RefLoadHook
{

public:
	static void InstallHook()
	{
		REL::Relocation<std::uintptr_t> ObjRefVtbl{ REL::ID(235511) };

		_ShouldSaveAnimationOnSaving = ObjRefVtbl.write_vfunc(0x7B, ShouldSaveAnimationOnSaving);

		logger::info("ShouldSaveAnimationOnSaving Hook Install!");
	}

private:

	static bool ShouldSaveAnimaiton(RE::NiAVObject* root)
	{
		using CYCLE_TYPE = RE::NiTimeController::CycleType;

		auto t_ctrl = root ? root->GetControllers() : nullptr;
		auto NiCtrl = t_ctrl ? t_ctrl->AsNiControllerManager() : nullptr;

		if (NiCtrl)
			for (auto seq : NiCtrl->activeSequences)
				if (seq->Animating() && seq->cycleType == CYCLE_TYPE::kLoop)
					return true;
		
		return false;
	}

	static bool ShouldSaveAnimationOnSaving(RE::TESObjectREFR* ref)
	{	
		
		//logger::debug("ShouldSaveAnimationOnSaving Hook Trigger!");

		auto result = _ShouldSaveAnimationOnSaving(ref);

		if (ref) {
			auto baseObj = ref ? ref->GetBaseObject() : nullptr;
			if (baseObj && baseObj->IsMovableStatic() && ref->Get3D() && ShouldSaveAnimaiton(ref->Get3D())) {
				logger::debug(FMT_STRING("Find a Should Save Animation Movable Static refID {:x}, baseID {:x}, Default Should Save is {}"), ref->formID, baseObj->formID, result);
				if (!result) {
					logger::debug("Force Save Animation!");
					return true;
				}
			}
		}

		return result;
	}

	static inline REL::Relocation<decltype(ShouldSaveAnimationOnSaving)> _ShouldSaveAnimationOnSaving;
};