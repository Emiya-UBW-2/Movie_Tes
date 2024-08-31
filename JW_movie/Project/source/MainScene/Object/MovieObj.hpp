#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MovieObjClass : public ObjectBaseClass
		{
		public:
			MovieObjClass(void) noexcept { this->m_objType = static_cast<int>(ObjType::MovieObj); }
			~MovieObjClass(void) noexcept {}
		public:
			void			DrawShadow(void) noexcept override {
				this->m_obj.DrawModel();
			}
		};
		
	};
};
