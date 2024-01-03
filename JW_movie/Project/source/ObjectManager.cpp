#include	"ObjectManager.hpp"
#include "MainScene/Object/MovieObj.hpp"

const FPS_n2::Sceneclass::ObjectManager* SingletonBase<FPS_n2::Sceneclass::ObjectManager>::m_Singleton = nullptr;
namespace FPS_n2 {
	namespace Sceneclass {
		SharedObj*		ObjectManager::MakeObject(ObjType ModelType) noexcept {
			switch (ModelType) {
			case ObjType::Human:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<MovieObjClass>();
				break;
			case ObjType::MovieObj:
				this->m_Object.resize(this->m_Object.size() + 1);
				this->m_Object.back() = std::make_shared<MovieObjClass>();
				break;
			default:
				break;
			}
			return &(this->m_Object[this->m_Object.size() - 1]);
		}
		void			ObjectManager::LoadObjectModel(ObjectBaseClass* pObj, const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
			for (auto& o : this->m_Object) {
				if (!o->GetIsBaseModel()) { continue; }
				if (!o->GetPathCompare(filepath, objfilename, colfilename)) { continue; }
				pObj->CopyModel(o);
				return;
			}
			pObj->LoadModel(m_DefaultType, filepath, objfilename, colfilename);
			pObj->SaveModel(m_UseToonWhenCreateFile);
		}
		//
		SharedObj*		ObjectManager::GetObj(ObjType ModelType, int num) noexcept {
			int cnt = 0;
			for (auto&o : this->m_Object) {
				if (o->GetobjType() == ModelType) {
					if (cnt == num) {
						return &o;
					}
					cnt++;
				}
			}
			return nullptr;
		}
		//
		void			ObjectManager::DelObj(ObjType ModelType, int num) noexcept {
			int cnt = 0;
			for (auto&o : this->m_Object) {
				if (o->GetobjType() == ModelType) {
					if (cnt == num) {
						//‡”Ô‚ÌˆÛŽ‚Ì‚½‚ß‚±‚±‚Íerase
						o->Dispose();
						this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
						break;
					}
					cnt++;
				}
			}
		}
		//
		void			ObjectManager::DelObj(SharedObj* ptr) noexcept {
			for (auto&o : this->m_Object) {
				if (o == *ptr) {
					//‡”Ô‚ÌˆÛŽ‚Ì‚½‚ß‚±‚±‚Íerase
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
					break;
				}
			}
		}
		//
		void			ObjectManager::DelObjAll(ObjType ModelType) noexcept {
			for (auto&o : this->m_Object) {
				if (o->GetobjType() == ModelType) {
					o->Dispose();
					this->m_Object.erase(this->m_Object.begin() + (&o - &this->m_Object.front()));
				}
			}
		}
	};
};
