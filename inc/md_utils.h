#pragma once

#define BP_NUM			1
#define KERNEL_NUM		8	//28  //PU_OUTPUT - 1
#define PU_NUM			9   //(BP_NUM+KERNEL_NUM+3)
#define CHN_NUM			5

struct DeleteOperator
{
	DeleteOperator& operator=(const DeleteOperator& _Right) = delete;
};

struct LCUConfig PUBLIC_C DeleteOperator
{
	MD_FW md_fw_ptr;
	MD_INPUT md_input_ptr;
	U11		min_scu_x;
	U11		min_scu_y;

	friend void copy(LCUConfig& _Left, const LCUConfig& _Right)
	{
		copy(_Left.md_fw_ptr, _Right.md_fw_ptr);
		copy_luma(_Left.md_input_ptr, _Right.md_input_ptr);
		_Left.min_scu_x = _Right.min_scu_x;
		_Left.min_scu_y = _Right.min_scu_y;
	}
};

struct LCUInput PUBLIC_C DeleteOperator
{
	RMD_OUTPUT g_rmd_output;
	pel RefWindowLCU_UP[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP[(SWH * SWW) >> 2];
	S14 CtrPos[RANGE_NUM][DIR_DIM][MAP_DIMS];
#if ENABLE_BFRAME
	pel RefWindowLCU_UP_ref1[(SWH * SWW) >> 2];
	pel RefWindowLCU_VP_ref1[(SWH * SWW) >> 2];
	S14 CtrPos_ref1[RANGE_NUM][DIR_DIM][MAP_DIMS];
	ME_MV_DIR me_mv_dir;
#endif
	pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH];
	SKIP_MERGE_INPUT skip_in;
	ENC_FME_INFO_ALL fme_mv_info;
	strFetch_ref_window Fetch_Ref_window_ptr;

	friend void copy(int w, int h, LCUInput& _Left, LCUInput& _Right)
	{
		//copy(w, h, _Left.g_rmd_output, _Right.g_rmd_output);
		//copy(w, h, _Left.me_mv_dir, _Right.me_mv_dir);
		//copy(w, h, _Left.fme_mv_info, _Right.fme_mv_info);
		copy(_Left.g_rmd_output, _Right.g_rmd_output);
		copy(_Left.me_mv_dir, _Right.me_mv_dir);
		copy(_Left.fme_mv_info, _Right.fme_mv_info);
		//_Left.g_rmd_output = _Right.g_rmd_output;
		//_Left.me_mv_dir = _Right.me_mv_dir;
		//_Left.fme_mv_info = _Right.fme_mv_info;
		for (int i = 0; i < RANGE_NUM; ++i)
		{
			for (int j = 0; j < DIR_DIM; ++j)
			{
				for (int k = 0; k < MAP_DIMS; ++k)
				{
					_Left.CtrPos[i][j][k] = _Right.CtrPos[i][j][k];
#if ENABLE_BFRAME
					_Left.CtrPos_ref1[i][j][k] = _Right.CtrPos_ref1[i][j][k];
#endif // ENABLE_BFRAME
				}
			}
		}
		for (int i = 0; i < MAX_CU_SIZE_FETCH; ++i)
		{
			for (int j = 0; j < MAX_CU_SIZE_FETCH; ++j)
			{
				_Left.p_fenc_LCU_Y[i][j] = _Right.p_fenc_LCU_Y[i][j];
			}
		}
		copy(_Left.skip_in, _Right.skip_in);
		copy_idx(_Left.Fetch_Ref_window_ptr, _Right.Fetch_Ref_window_ptr);
	}
};
