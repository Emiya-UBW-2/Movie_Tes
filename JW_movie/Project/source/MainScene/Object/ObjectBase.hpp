#pragma once
#include	"../../Header.hpp"
#include	"../../MainScene/BackGround/BackGround.hpp"

#include	"CharacterEnum.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		enum class PHYSICS_SETUP {
			DISABLE,
			LOADCALC,
			REALTIME,
		};

		class ObjectBaseClass {
		protected:
			PHYSICS_SETUP								m_PHYSICS_SETUP{ PHYSICS_SETUP::DISABLE };
			bool										m_objActive{ false };							//
			bool										m_ColActive{ false };							//
			MV1											m_obj;
			MV1											m_col;

			moves										m_move;
			MATRIX_ref									m_PrevMat;//物理更新のため
			std::vector<std::pair<int, MATRIX_ref>>		m_Frames;
			std::vector<std::pair<int, float>>			m_Shapes;
			ObjType										m_objType{ (ObjType)0 };
			std::string									m_FilePath;
			std::string									m_ObjFileName;
			std::string									m_ColFileName;

			bool										m_IsActive{ true };
			bool										m_IsDelete{ false };

			bool										m_IsResetPhysics{ true };
			bool										m_IsFirstLoop{ true };
			bool										m_IsDraw{ true };
			float										m_DistanceToCam{ 0.f };
			bool										m_IsBaseModel{ false };
			VECTOR_ref									m_CameraPosition;
			float										m_CameraSize{ 0.f };
			PlayerID									m_MyID{ 0 };									//

			ShaderUseClass*								m_UseShader{ nullptr };
			std::array<int, 2>							m_ShaderTex{ -1 , -1 };
		public:
			auto&			GetObj(void) noexcept { return this->m_obj; }
			//const auto&		GetObj_const(void) const noexcept { return this->m_obj; }
			const auto		GetMatrix(void) const noexcept { return this->m_obj.GetMatrix(); }
			const auto		GetFrameLocalMatrix(int frame) const noexcept { return this->m_obj.GetFrameLocalMatrix(frame); }
			const auto		GetFrameWorldMatrix(int frame) const noexcept { return this->m_obj.GetFrameLocalWorldMatrix(frame); }
			const auto		GetParentFrameWorldMatrix(int frame) const noexcept { return GetFrameWorldMatrix((int)this->m_obj.frame_parent(frame)); }
		public:
			const auto&		GetIsBaseModel(void) const noexcept { return this->m_IsBaseModel; }
			const auto		GetPathCompare(const char* filepath, const char* objfilename, const char* colfilename) const noexcept {
				return (
					(this->m_FilePath == filepath) &&
					(this->m_ObjFileName == objfilename) &&
					(this->m_ColFileName == colfilename));
			}
			const auto&		GetobjType(void) const noexcept { return this->m_objType; }
			const auto&		GetCameraPosition(void) const noexcept { return this->m_CameraPosition; }
			const auto&		GetCameraSize(void) const noexcept { return this->m_CameraSize; }
			const auto&		GetMove(void) const noexcept { return this->m_move; }
			const auto&		IsActive(void) const noexcept { return this->m_IsActive; }
			const auto&		GetIsDelete(void) const noexcept { return this->m_IsDelete; }
			const auto&		GetMyPlayerID(void) const noexcept { return this->m_MyID; }
			const auto&		GetFilePath(void) const noexcept { return this->m_FilePath; }
		public:
			void			SetPlayerID(PlayerID value) noexcept { this->m_MyID = value; }
			void			SetActive(bool value) noexcept { this->m_IsActive = value; }
			void			SetIsDelete(bool value) noexcept { this->m_IsDelete = value; }
			void			SetResetP(bool value) { this->m_IsResetPhysics = value; }
			void			SetCameraPosition(const VECTOR_ref& value) { this->m_CameraPosition = value; }
			void			SetCameraSize(float value) { this->m_CameraSize = value; }
			void			SetUseShader(ShaderUseClass* value) noexcept { this->m_UseShader = value; }
			void			SetShaderTexHandle(int id, int value) noexcept { this->m_ShaderTex[id] = value; }
			void			SetAnimOnce(int ID, float speed) {
				this->GetObj().get_anime(ID).time += 30.f / FPS * speed;
				if (this->GetObj().get_anime(ID).TimeEnd()) { this->GetObj().get_anime(ID).GoEnd(); }
			}
			void			SetAnimLoop(int ID, float speed) {
				this->GetObj().get_anime(ID).time += 30.f / FPS * speed;
				if (this->GetObj().get_anime(ID).TimeEnd()) { this->GetObj().get_anime(ID).GoStart(); }
			}
			void			ResetMove(const MATRIX_ref& mat, const VECTOR_ref& pos) noexcept {
				this->m_move.vec.clear();
				SetMove(mat, pos);
			}
			void			SetMove(const MATRIX_ref& mat, const VECTOR_ref& pos) noexcept {
				this->m_move.mat = mat;
				this->m_move.pos = pos;
				UpdateMove();
			}
			void			UpdateMove(void) noexcept {
				this->m_PrevMat = this->GetObj().GetMatrix();
				this->GetObj().SetMatrix(this->m_move.MatIn());
				if (this->m_col.IsActive()) {
					this->m_col.SetMatrix(this->m_move.MatIn());
					//this->m_col.RefreshCollInfo();
				}
			}
			//判定起動
			const auto		RefreshCol(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, float pRange) noexcept {
				if (this->m_ColActive) { return true; }				//すでに起動しているなら無視
				if (GetMinLenSegmentToPoint(StartPos, EndPos, m_move.pos) <= pRange) {
					//判定起動
					this->m_ColActive = true;
					for (int i = 0; i < this->m_col.mesh_num(); i++) {
						this->m_col.RefreshCollInfo(-1, i);
					}
					return true;
				}
				return false;
			}
			//判定取得
			const auto		GetColLine(const VECTOR_ref& StartPos, const VECTOR_ref& EndPos, const int sel = 0) const noexcept { return this->m_col.CollCheck_Line(StartPos, EndPos, -1, sel); }
			void			GetColNearestInAllMesh(const VECTOR_ref& StartPos, VECTOR_ref* EndPos) const noexcept {
				MV1_COLL_RESULT_POLY colres;
				for (int i = 0; i < this->m_col.mesh_num(); ++i) {
					colres = GetColLine(StartPos, *EndPos, i);
					if (colres.HitFlag == TRUE) {
						*EndPos = colres.HitPosition;
					}
				}
			}
		public:
			void			LoadModel(PHYSICS_SETUP TYPE, const char* filepath, const char* objfilename = "model", const char* colfilename = "col") noexcept;
			void			CopyModel(const std::shared_ptr<ObjectBaseClass>& pBase) noexcept;
			void			SaveModel(bool UseToonWhenCreateFile) noexcept;
		public:
			virtual void	Init(void) noexcept;
			virtual void	FirstExecute(void) noexcept { }
			void			ExecuteCommon(void) noexcept;
			virtual void	LateExecute(void) noexcept { }
			virtual void	Depth_Draw(void) noexcept { }
			virtual void	DrawShadow(void) noexcept;
			virtual void	CheckDraw(void) noexcept;
			virtual void	Draw(bool isDrawSemiTrans) noexcept;
			//
			virtual void	Dispose(void) noexcept;
		};
	};
};
