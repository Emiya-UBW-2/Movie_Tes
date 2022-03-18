#pragma once

namespace FPS_n2 {
	namespace Sceneclass {
		class LOADDATA : public TEMPSCENE {
			void Set(void) noexcept override {
			}
			bool Update(void) noexcept override {
				return false;
			}
			void Dispose(void) noexcept override {
			}
			//
			void UI_Draw(void) noexcept  override {
			}
			void BG_Draw(void) noexcept override {
			}
			void Shadow_Draw_NearFar(void) noexcept override {
			}
			void Shadow_Draw(void) noexcept override {
			}
			void Main_Draw(void) noexcept override {
			}
			void LAST_Draw(void) noexcept override {
			}
		};
	};
};