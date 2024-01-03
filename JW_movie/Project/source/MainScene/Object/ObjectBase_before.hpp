#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class ItemData {
		private:
			std::string		m_path;
			std::string		m_name;
		public://getter
			const auto&		GetPath(void) const noexcept { return this->m_path; }
			const auto&		GetName(void) const noexcept { return this->m_name; }
		protected:
			virtual void	Set_Sub(const std::string&, const std::string&) noexcept {}
		public:
			void			Set(std::string path_) {
				this->m_path = path_;
				int mdata = FileRead_open((this->m_path + "data.txt").c_str(), FALSE);
				while (true) {
					if (FileRead_eof(mdata) != 0) { break; }
					auto ALL = getparams::Getstr(mdata);
					//コメントアウト
					if (ALL.find("//") != std::string::npos) {
						ALL = ALL.substr(0, ALL.find("//"));
					}
					//
					if (ALL == "") { continue; }
					auto LEFT = getparams::getleft(ALL);
					auto RIGHT = getparams::getright(ALL);
					//アイテムデータ読みとり
					if (LEFT == "Name") {
						this->m_name = RIGHT;
					}
					Set_Sub(LEFT, RIGHT);
				}
				FileRead_close(mdata);
			}
		};
	};
};
