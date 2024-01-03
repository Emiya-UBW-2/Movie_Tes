#pragma once
#include	"Header.hpp"
#include "MainScene/Object/ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		typedef std::shared_ptr<ObjectBaseClass> SharedObj;

		class ObjectManager : public SingletonBase<ObjectManager> {
		private:
			friend class SingletonBase<ObjectManager>;
		private:
			std::vector<SharedObj>	m_Object;
			switchs					m_ResetP;

			PHYSICS_SETUP			m_DefaultType{ PHYSICS_SETUP::DISABLE };
			bool					m_UseToonWhenCreateFile{ false };
		private:
			ObjectManager() {
				m_Object.reserve(256);
			}
			~ObjectManager() {
				m_Object.clear();
				m_Object.shrink_to_fit();
			}
		public:
			SharedObj*		MakeObject(ObjType ModelType) noexcept;
			void			LoadObjectModel(ObjectBaseClass* pObj, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") const noexcept;

			SharedObj*		GetObj(ObjType ModelType, int num) noexcept;

			void			DelObj(ObjType ModelType, int num) noexcept;
			void			DelObj(SharedObj* ptr) noexcept;
			void			DelObjAll(ObjType ModelType) noexcept;
		public:
			void			ExecuteObject(void) noexcept {
				//オブジェクトが増えた場合に備えて範囲forは使わない
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (!o->GetIsDelete()) {
						o->FirstExecute();
					}
				}
				//物理アップデート
				this->m_ResetP.Execute(CheckHitKeyWithCheck(KEY_INPUT_P) != 0);
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (!o->GetIsDelete()) {
						if (this->m_ResetP.trigger()) { o->SetResetP(true); }
						o->ExecuteCommon();
					}
				}
				//オブジェクトの排除チェック
				for (int i = 0; i < this->m_Object.size(); i++) {
					auto& o = this->m_Object[i];
					if (o->GetIsDelete()) {
						//順番の維持のためここはerase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + i);
						i--;
					}
				}
			}
			void			LateExecuteObject(void) noexcept {
				for (auto&o : this->m_Object) {
					o->LateExecute();
				}
			}
			void			DrawDepthObject(void) noexcept {
				for (auto& o : this->m_Object) {
					o->Depth_Draw();
				}
			}
			void			DrawObject() noexcept {
				for (auto& o : this->m_Object) {
					o->CheckDraw();
					o->Draw(false);
				}
				for (auto& o : this->m_Object) {
					o->Draw(true);
				}
			}
			void			DrawObject_Shadow(void) noexcept {
				for (auto& o : this->m_Object) {
					o->DrawShadow();
				}
			}
			void			DisposeObject(void) noexcept {
				for (auto& o : this->m_Object) {
					if (o) {
						o->Dispose();
					}
				}
				this->m_Object.clear();
			}
		};
	};
};
