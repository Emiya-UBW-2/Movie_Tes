#pragma once
#include	"Header.hpp"
//
namespace FPS_n2 {
	//
	static float GameSpeed = 1.0f;

	template <class T>
	static void			easing_set_SetSpeed(T* first, const T& aim, const float& ratio) noexcept {
		if (ratio == 0.f) {
			*first = aim;
		}
		else {
			auto* DrawParts = DXDraw::Instance();
			*first = Lerp(*first, aim, (1.f - std::powf(ratio, 60.f / DrawParts->GetFps() * GameSpeed)));
		}
	};

	static void			easing_set_SetSpeed(Camera3DInfo* first, const Camera3DInfo& aim, const float& ratio) noexcept {
		if (ratio == 0.f) {
			*first = aim;
		}
		else {
			Vector3DX pos_t = first->GetCamPos();
			Vector3DX vec_t = first->GetCamVec();
			Vector3DX up_t = first->GetCamUp();
			float fov_t = first->GetCamFov();
			float near_t = first->GetCamNear();
			float far_t = first->GetCamFar();
			easing_set_SetSpeed(&pos_t, aim.GetCamPos(), ratio);
			easing_set_SetSpeed(&vec_t, aim.GetCamVec(), ratio);
			easing_set_SetSpeed(&up_t, aim.GetCamUp(), ratio);
			easing_set_SetSpeed(&fov_t, aim.GetCamFov(), ratio);
			easing_set_SetSpeed(&far_t, aim.GetCamFar(), ratio);
			easing_set_SetSpeed(&near_t, aim.GetCamNear(), ratio);
			first->SetCamPos(pos_t, vec_t, up_t);
			first->SetCamInfo(fov_t, near_t, far_t);
		}
	};
	//
	class TelopClass {
	private:
		class Cut_tex {
			int xpos = 0;
			int ypos = 0;
			int size = 0;
			int LMR = 1;
			std::string str;
			LONGLONG START_TIME = 0;
			LONGLONG END_TIME = 0;
		public:
			Cut_tex(void) noexcept {
				xpos = 0;
				ypos = 0;
				size = 12;
				str = "test";
				START_TIME = (LONGLONG)(1000000.f * 0.01f);
				END_TIME = (LONGLONG)(1000000.f * 1.01f);
			}
			void			Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
				this->xpos = xp;
				this->ypos = yp;
				this->size = Fontsize;
				this->str = mag;
				this->START_TIME = StartF;
				this->END_TIME = StartF + ContiF;;
				this->LMR = m_LMR;
			}
			void			Draw(LONGLONG nowTimeWait) const noexcept {
				if (this->START_TIME < nowTimeWait && nowTimeWait < this->END_TIME) {
					auto* Fonts = FontPool::Instance();
					switch (this->LMR) {
					case 0:
						Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					case 1:
						Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					case 2:
						Fonts->Get(FontPool::FontType::MS_Gothic, -1, 3)->DrawString(this->size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
						break;
					default:
						break;
					}
				}
			}
		};
	private:
		std::vector<Cut_tex> Texts;
		LONGLONG StartF = 0;
		LONGLONG ContiF = 0;
	public:
		void			Init(void) noexcept {
			StartF = 0;
			ContiF = 0;
		}
		void			LoadTelop(const std::string& func, const std::vector<std::string>& args) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (func.find("SetTelopTime") != std::string::npos) {
				StartF = (LONGLONG)(1000000.f * std::stof(args[0]));
				ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
			}
			else if (func.find("AddTelopTime") != std::string::npos) {
				StartF += (LONGLONG)(1000000.f * std::stof(args[0]));
				ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
			}
			else if (func.find("SetTelop") != std::string::npos) {
				int t = 0;
				if (args[4].find("LEFT") != std::string::npos) { t = 0; }
				else if (args[4].find("MIDDLE") != std::string::npos) { t = 1; }
				else if (args[4].find("RIGHT") != std::string::npos) { t = 2; }
				Texts.resize(Texts.size() + 1);
				Texts.back().Set(DrawParts->GetScreenY(std::stoi(args[0])), DrawParts->GetScreenY(std::stoi(args[1])), DrawParts->GetScreenY(std::stoi(args[2])), args[3], StartF, ContiF, t);
			}
		}
		void			Draw(LONGLONG nowTimeWait) const noexcept {
			for (auto& t : Texts) {
				t.Draw(nowTimeWait);
			}
		}
	};
	//
	struct Anims {
		std::vector<GraphHandle> Pic;
		int m_FrameID{ 0 };
		int anim{ 0 };
		int animcountper{ 0 };
		int animcountSpeed{ 4 };

		void			Set(int FrameID, int LayerID, int totalNum, int FramePer) noexcept {
			m_FrameID = FrameID;
			this->animcountSpeed = FramePer;
			for (int i = 0; i < totalNum; i++) {
				std::string FileName = "data/picture/Scene" + std::to_string(FrameID) + "_" + std::to_string(LayerID);
				if (i < 10) {
					FileName += "0" + std::to_string(i);
				}
				else {
					FileName += std::to_string(i);
				}
				this->Pic.resize(this->Pic.size() + 1);
				this->Pic.back().Load(FileName + ".bmp", true);
			}
		}

		void			Execute(void) noexcept {
			if (Pic.size() > 0) {
				this->animcountper++;
				if (this->animcountper > this->animcountSpeed) {
					this->animcountper = 0;
					++this->anim %= this->Pic.size();
				}
			}
		}
		void			Draw(int x, int y, float scale) noexcept {
			this->Pic[this->anim].DrawRotaGraph(x, y, scale, 0.f, true);
		}
	};
	//
	struct FogParam {
		int fog[3]{ -1,-1,-1 };
		float fog_range[2]{ -1.f,-1.f };
	public:
		void Reset(void) noexcept {
			fog[0] = 128;
			fog[1] = 128;
			fog[2] = 128;
			fog_range[0] = 200.f;
			fog_range[1] = 300000.f;
		}
		void SetFog() const noexcept {
			SetFogDensity(0.01f);
			SetFogColor(fog[0], fog[1], fog[2]);
			SetFogStartEnd(fog_range[0], fog_range[1]);
		}
	};
	//
	namespace Sceneclass {
		class CommonBattleResource : public SingletonBase<CommonBattleResource> {
		private:
			friend class SingletonBase<CommonBattleResource>;
		private:
		public:
			void			Load(void) noexcept;
			void			Set(void) noexcept;
			void			Dispose(void) noexcept;
		};
	};
};
