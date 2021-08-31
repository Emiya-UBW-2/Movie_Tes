#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	//ƒ|ƒCƒ“ƒ^‹¤’Ê
	class PTR_COMMON {
	protected:
		std::shared_ptr<DXDraw> DrawPts{ nullptr };			//ˆø‚«Œp‚®
	public:
		virtual void Set_Ptr_Common(std::shared_ptr<DXDraw>& DrawPts_t) noexcept {
			DrawPts = DrawPts_t;
		}
		virtual void Dispose_Ptr_Common(void) noexcept {
			DrawPts.reset();
		}
	};
};