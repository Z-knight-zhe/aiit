#pragma once
#include "enc_def.h"
#include "md_utils.h"

#define CU_DATA_POOL_SIZE		80
#define CU_DATA_UNIT_SIZE		16
#define UNIT_IN_LCU_SIZE		(64/16)

using PoolUnit = ENC_CU_DATA_ARRAY_SIZE<CU_DATA_UNIT_SIZE>;
using CuDataPool = PoolUnit[CU_DATA_POOL_SIZE];

struct CuDataPtr PUBLIC_C DeleteOperator
{
	s64 cost[UNIT_IN_LCU_SIZE][UNIT_IN_LCU_SIZE];
	int idx[UNIT_IN_LCU_SIZE][UNIT_IN_LCU_SIZE];

	void init()
	{
		for (int i = 0; i < UNIT_IN_LCU_SIZE; ++i)
		{
			for (int j = 0; j < UNIT_IN_LCU_SIZE; ++j)
			{
#pragma HLS pipeline
				cost[i][j] = MAX_S56;
				idx[i][j] = -1;
			}
		}
	}

	void set(int x, int y, int w, int h, int offset, s64 cost_sum)
	{
		int x_unit = x / CU_DATA_UNIT_SIZE;
		int y_unit = y / CU_DATA_UNIT_SIZE;
		int w_unit = w / CU_DATA_UNIT_SIZE;
		int h_unit = h / CU_DATA_UNIT_SIZE;
		assert(x_unit * CU_DATA_UNIT_SIZE == x);
		assert(y_unit * CU_DATA_UNIT_SIZE == y);
		assert(w_unit * CU_DATA_UNIT_SIZE == w);
		assert(h_unit * CU_DATA_UNIT_SIZE == h);

		for (int i = 0; i < UNIT_IN_LCU_SIZE; ++i)
		{
			for (int j = 0; j < UNIT_IN_LCU_SIZE; ++j)
			{
#pragma HLS pipeline
				if ((i < h_unit) && (j < w_unit)) {
				idx[y_unit + i][x_unit + j] = offset + i * h_unit + j;
				cost[y_unit + i][x_unit + j] = 0;
			}
		}
		}
		cost[y_unit][x_unit] = cost_sum;
	}

	void set_cost(int x, int y, int w, int h, s64 cost_sum)
	{
		int x_unit = x / CU_DATA_UNIT_SIZE;
		int y_unit = y / CU_DATA_UNIT_SIZE;
		int w_unit = w / CU_DATA_UNIT_SIZE;
		int h_unit = h / CU_DATA_UNIT_SIZE;
		assert(x_unit * CU_DATA_UNIT_SIZE == x);
		assert(y_unit * CU_DATA_UNIT_SIZE == y);
		assert(w_unit * CU_DATA_UNIT_SIZE == w);
		assert(h_unit * CU_DATA_UNIT_SIZE == h);

		for (int i = 0; i < h_unit; ++i)
		{
#pragma HLS LOOP_TRIPCOUNT max=4
			for (int j = 0; j < w_unit; ++j)
			{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=4
				assert(idx[idx[y_unit + i][x_unit + j]] >= 0);
				cost[y_unit + i][x_unit + j] = 0;
			}
		}
		cost[y_unit][x_unit] = cost_sum;
	}

	s64 get_cost(int x, int y, int w, int h) const
	{
		int x_unit = x / CU_DATA_UNIT_SIZE;
		int y_unit = y / CU_DATA_UNIT_SIZE;
		int w_unit = w / CU_DATA_UNIT_SIZE;
		int h_unit = h / CU_DATA_UNIT_SIZE;
		assert(x_unit * CU_DATA_UNIT_SIZE == x);
		assert(y_unit * CU_DATA_UNIT_SIZE == y);
		assert(w_unit * CU_DATA_UNIT_SIZE == w);
		assert(h_unit * CU_DATA_UNIT_SIZE == h);

		s64 sum = 0;
		for (int i = 0; i < h_unit; ++i)
		{
#pragma HLS LOOP_TRIPCOUNT max=4
			for (int j = 0; j < w_unit; ++j)
			{
#pragma HLS LOOP_TRIPCOUNT max=4
#pragma HLS pipeline
				if (idx[i + y_unit][j + x_unit] >= 0)
				{
					sum += cost[i + y_unit][j + x_unit];
				}
			}
		}
		return sum == 0 ? MAX_S56 : sum;
	}

	s64 get_cost() const
	{
		s64 sum = 0;
		for (int i = 0; i < UNIT_IN_LCU_SIZE; ++i)
		{
			for (int j = 0; j < UNIT_IN_LCU_SIZE; ++j)
			{
#pragma HLS pipeline
				if (idx[i][j] >= 0)
				{
					sum += cost[i][j];
				}
			}
		}
		return sum == 0 ? MAX_S56 : sum;
	}

	friend void copy(CuDataPtr& _Left, const CuDataPtr& _Right)
	{
#pragma HLS inline
		for (int i = 0; i < UNIT_IN_LCU_SIZE; ++i)
		{
			for (int j = 0; j < UNIT_IN_LCU_SIZE; ++j)
			{
#pragma HLS pipeline
				_Left.cost[i][j] = _Right.cost[i][j];
				_Left.idx[i][j] = _Right.idx[i][j];
			}
		}
	}
};

struct CuDataMan PUBLIC_C DeleteOperator
{
	bool csr[CU_DATA_POOL_SIZE];

	void init()
	{
		for (int i = 0; i < CU_DATA_POOL_SIZE; ++i)
		{
#pragma HLS pipeline
			csr[i] = false;
		}
	}

	int alloc(int length, int offset)
	{
		for (int i = 0; i < length; ++i)
		{
#pragma HLS pipeline
			assert(csr[i + offset] == false);
			csr[i + offset] = true;
		}
	}

	void release(int length, int offset)
	{
		for (int i = 0; i < length; ++i)
		{
#pragma HLS pipeline
			assert(csr[i + offset] == true);
			csr[i + offset] = false;
		}
	}
};

template <size_t _Size>
void write_memory(int w, int h, PoolUnit* mem, ENC_CU_DATA_ARRAY_SIZE<_Size>& data)
{
	int w_unit = w / CU_DATA_UNIT_SIZE;
	int h_unit = h / CU_DATA_UNIT_SIZE;
	assert(w_unit * CU_DATA_UNIT_SIZE == w);
	assert(h_unit * CU_DATA_UNIT_SIZE == h);

#pragma HLS INLINE
	for (int i_unit = 0; i_unit < h_unit; ++i_unit)
	{
#pragma HLS LOOP_TRIPCOUNT max=4
		for (int j_unit = 0; j_unit < w_unit; ++j_unit)
		{
#pragma HLS LOOP_TRIPCOUNT max=4
			PoolUnit* unit = mem + i_unit * w_unit + j_unit;

			for (int i = 0; i < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++i)
			{
				for (int j = 0; j < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++j)
				{
#pragma HLS pipeline
					int i_src = i_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + i;
					int j_src = j_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int dstp = i * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int srcp = i_src * (w >> MIN_CU_LOG2) + j_src;

					for (int k = 0; k < REFP_NUM; ++k)
					{
						for (int l = 0; l < MV_D; ++l)
						{
							unit->mv[dstp][k][l] = data.mv[srcp][k][l];
							unit->mvd[dstp][k][l] = data.mvd[srcp][k][l];
						}
						unit->refi[dstp][k] = data.refi[srcp][k];
					}
					unit->tb_part[dstp] = data.tb_part[srcp];
					unit->pred_mode[dstp] = data.pred_mode[srcp];
					unit->mvr_idx[dstp] = data.mvr_idx[srcp];
					unit->umve_flag[dstp] = data.umve_flag[srcp];
					unit->umve_idx[dstp] = data.umve_idx[srcp];
					unit->skip_idx[dstp] = data.skip_idx[srcp];
					unit->map_scu[dstp] = data.map_scu[srcp];
					unit->affine_flag[dstp] = data.affine_flag[srcp];
					unit->smvd_flag[dstp] = data.smvd_flag[srcp];
					unit->map_cu_mode[dstp] = data.map_cu_mode[srcp];
					unit->map_pb_tb_part[dstp] = data.map_pb_tb_part[srcp];
					unit->depth[dstp] = data.depth[srcp];
					unit->ipf_flag[dstp] = data.ipf_flag[srcp];
					unit->mvp_from_hmvp_flag[dstp] = data.mvp_from_hmvp_flag[srcp];
#if INTERPF
					unit->inter_filter_flag[dstp] = data.inter_filter_flag[srcp];
#endif
					for (int k = 0; k < 2; ++k)
					{
						unit->mpm[k][dstp] = data.mpm[k][srcp];
						unit->ipm[k][dstp] = data.ipm[k][srcp];
					}
					for (int k = 0; k < 8; ++k)
					{
						unit->mpm_ext[k][dstp] = data.mpm_ext[k][srcp];
					}
					for (int k = 0; k < N_C; ++k)
					{
						unit->num_nz_coef[k][dstp] = data.num_nz_coef[k][srcp];
					}
#if CUDQP_QP_MAP
					unit->cu_qp_group_pred_qp = data.cu_qp_group_pred_qp;
					unit->cu_luma_qp[dstp] = data.cu_luma_qp[srcp];
#endif
					int m_src = i_unit * CU_DATA_UNIT_SIZE + i*4;
					int n_src = j_unit * CU_DATA_UNIT_SIZE + j*4;
					for (int m = 0; m < 4; ++m)
					{
						for (int n = 0; n < 4; ++n)
						{
							unit->coef_y[4*i+m][4*j+n] = data.coef_y[m_src+m][n_src+n];
							unit->reco_y[4*i+m][4*j+n] = data.reco_y[m_src+m][n_src+n];
						}
					}
					int k = 4*i + j;
					for (int t = 0; t < 16; ++t)
					{
						for (int m = 0; m < MAX_CU_DEPTH; ++m)
						{
							for (int n = 0; n < NUM_BLOCK_SHAPE; ++n)
							{
								unit->split_mode[m][n][16*k+t] = NO_SPLIT;  //5*7*256
							}
						}
					}
				}
			}
		}
	}
}

template <size_t _Size>
void read_memory(int x, int y, int w, int h, ENC_CU_DATA_ARRAY_SIZE<_Size>& data, CuDataPool& mem, CuDataPtr& ptr)
{
	int x_unit = x / CU_DATA_UNIT_SIZE;
	int y_unit = y / CU_DATA_UNIT_SIZE;
	int w_unit = w / CU_DATA_UNIT_SIZE;
	int h_unit = h / CU_DATA_UNIT_SIZE;
	assert(x_unit * CU_DATA_UNIT_SIZE == x);
	assert(y_unit * CU_DATA_UNIT_SIZE == y);
	assert(w_unit * CU_DATA_UNIT_SIZE == w);
	assert(h_unit * CU_DATA_UNIT_SIZE == h);

	for (int m = 0; m < MAX_CU_DEPTH; ++m)
	{
		for (int n = 0; n < NUM_BLOCK_SHAPE; ++n)
		{
			for (int i = 0; i < (h >> 2); ++i)
			{
				for (int j = 0; j < (w >> 2); ++j)
				{
					data.split_mode[m][n][i * (w >> 2) + j] = 0;
				}
			}
		}
	}

	for (int i_unit = 0; i_unit < h_unit; ++i_unit)
	{
		for (int j_unit = 0; j_unit < w_unit; ++j_unit)
		{
			assert(ptr.idx[i_unit + y_unit][j_unit + x_unit] >= 0);
			PoolUnit* unit = mem + ptr.idx[i_unit + y_unit][j_unit + x_unit];

			for (int m = 0; m < MAX_CU_DEPTH; ++m)
			{
				for (int n = 0; n < NUM_BLOCK_SHAPE; ++n)
				{
					for (int i = 0; i < (h >> 2); ++i)
					{
						for (int j = 0; j < (w >> 2); ++j)
						{
							data.split_mode[m][n][i * (w >> 2) + j] += unit->split_mode[m][n][(i + (y >> 2)) * 16 + j + (x >> 2)];
						}
					}
				}
			}

			for (int i = 0; i < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++i)
			{
				for (int j = 0; j < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++j)
				{
					int i_dst = i_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + i;
					int j_dst = j_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int srcp = i * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int dstp = i_dst * (w >> MIN_CU_LOG2) + j_dst;

					for (int k = 0; k < REFP_NUM; ++k)
					{
						for (int l = 0; l < MV_D; ++l)
						{
							data.mv[dstp][k][l] = unit->mv[srcp][k][l];
							data.mvd[dstp][k][l] = unit->mvd[srcp][k][l];
						}
						data.refi[dstp][k] = unit->refi[srcp][k];
					}
					data.tb_part[dstp] = unit->tb_part[srcp];
					data.pred_mode[dstp] = unit->pred_mode[srcp];
					data.mvr_idx[dstp] = unit->mvr_idx[srcp];
					data.umve_flag[dstp] = unit->umve_flag[srcp];
					data.umve_idx[dstp] = unit->umve_idx[srcp];
					data.skip_idx[dstp] = unit->skip_idx[srcp];
					data.map_scu[dstp] = unit->map_scu[srcp];
					data.affine_flag[dstp] = unit->affine_flag[srcp];
					data.smvd_flag[dstp] = unit->smvd_flag[srcp];
					data.map_cu_mode[dstp] = unit->map_cu_mode[srcp];
					data.map_pb_tb_part[dstp] = unit->map_pb_tb_part[srcp];
					data.depth[dstp] = unit->depth[srcp];
					data.ipf_flag[dstp] = unit->ipf_flag[srcp];
					data.mvp_from_hmvp_flag[dstp] = unit->mvp_from_hmvp_flag[srcp];
#if INTERPF
					data.inter_filter_flag[dstp] = unit->inter_filter_flag[srcp];
#endif
					for (int k = 0; k < 2; ++k)
					{
						data.mpm[k][dstp] = unit->mpm[k][srcp];
						data.ipm[k][dstp] = unit->ipm[k][srcp];
					}
					for (int k = 0; k < 8; ++k)
					{
						data.mpm_ext[k][dstp] = unit->mpm_ext[k][srcp];
					}
					for (int k = 0; k < N_C; ++k)
					{
						data.num_nz_coef[k][dstp] = unit->num_nz_coef[k][srcp];
					}
#if CUDQP_QP_MAP
					data.cu_qp_group_pred_qp = unit->cu_qp_group_pred_qp;
					data.cu_luma_qp[dstp] = unit->cu_luma_qp[srcp];
#endif
				}
			}

			for (int i = 0; i < CU_DATA_UNIT_SIZE; ++i)
			{
				for (int j = 0; j < CU_DATA_UNIT_SIZE; ++j)
				{
					int i_dst = i_unit * CU_DATA_UNIT_SIZE + i;
					int j_dst = j_unit * CU_DATA_UNIT_SIZE + j;

					data.coef_y[i_dst][j_dst] = unit->coef_y[i][j];
					data.reco_y[i_dst][j_dst] = unit->reco_y[i][j];
				}
			}
		}
	}
}

inline void read_memory(int x, int y, int w, int h, ENC_CU_DATA& data, CuDataPool& mem, CuDataPtr& ptr)
{
	int x_unit = 0;
	int y_unit = 0;
	int w_unit = 4;
	int h_unit = 4;
	assert(x == 0);
	assert(y == 0);
	assert(w == 64);
	assert(h == 64);

#pragma HLS ARRAY_PARTITION variable=data.split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=data.split_mode cyclic factor=7 dim=2


			for (int i = 0; i < (h >> 2); ++i)
			{
				for (int j = 0; j < (w >> 2); ++j)
		{
#pragma HLS PIPELINE
			for (int m = 0; m < MAX_CU_DEPTH; ++m)
			{
				for (int n = 0; n < NUM_BLOCK_SHAPE; ++n)
				{
					data.split_mode[m][n][i * (w >> 2) + j] = 0;
				}
			}
		}
	}

	for (int i_unit = 0; i_unit < h_unit; ++i_unit)
	{
		for (int j_unit = 0; j_unit < w_unit; ++j_unit)
		{
			assert(ptr.idx[i_unit + y_unit][j_unit + x_unit] >= 0);
			PoolUnit* unit = mem + ptr.idx[i_unit + y_unit][j_unit + x_unit];
#pragma HLS ARRAY_PARTITION variable=unit->split_mode cyclic factor=5 dim=1
#pragma HLS ARRAY_PARTITION variable=unit->split_mode cyclic factor=7 dim=2

			for (int i = 0; i < (h >> 2); ++i)
			{
				for (int j = 0; j < (w >> 2); ++j)
				{
#pragma HLS PIPELINE
					for (int m = 0; m < MAX_CU_DEPTH; ++m)
					{
						for (int n = 0; n < NUM_BLOCK_SHAPE; ++n)
						{
							data.split_mode[m][n][i * (w >> 2) + j] += unit->split_mode[m][n][(i + (y >> 2)) * 16 + j + (x >> 2)];
						}
					}
				}
			}

			for (int i = 0; i < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++i)
			{
#pragma HLS PIPELINE
				for (int j = 0; j < (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2); ++j)
				{
					int i_dst = i_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + i;
					int j_dst = j_unit * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int srcp = i * (CU_DATA_UNIT_SIZE >> MIN_CU_LOG2) + j;
					int dstp = i_dst * (w >> MIN_CU_LOG2) + j_dst;

					for (int k = 0; k < REFP_NUM; ++k)
					{
						for (int l = 0; l < MV_D; ++l)
						{
							data.mv[dstp][k][l] = unit->mv[srcp][k][l];
							data.mvd[dstp][k][l] = unit->mvd[srcp][k][l];
						}
						data.refi[dstp][k] = unit->refi[srcp][k];
					}
					data.tb_part[dstp] = unit->tb_part[srcp];
					data.pred_mode[dstp] = unit->pred_mode[srcp];
					data.mvr_idx[dstp] = unit->mvr_idx[srcp];
					data.umve_flag[dstp] = unit->umve_flag[srcp];
					data.umve_idx[dstp] = unit->umve_idx[srcp];
					data.skip_idx[dstp] = unit->skip_idx[srcp];
					data.map_scu[dstp] = unit->map_scu[srcp];
					data.affine_flag[dstp] = unit->affine_flag[srcp];
					data.smvd_flag[dstp] = unit->smvd_flag[srcp];
					data.map_cu_mode[dstp] = unit->map_cu_mode[srcp];
					data.map_pb_tb_part[dstp] = unit->map_pb_tb_part[srcp];
					data.depth[dstp] = unit->depth[srcp];
					data.ipf_flag[dstp] = unit->ipf_flag[srcp];
					data.mvp_from_hmvp_flag[dstp] = unit->mvp_from_hmvp_flag[srcp];
#if INTERPF
					data.inter_filter_flag[dstp] = unit->inter_filter_flag[srcp];
#endif
					for (int k = 0; k < 2; ++k)
					{
						data.mpm[k][dstp] = unit->mpm[k][srcp];
						data.ipm[k][dstp] = unit->ipm[k][srcp];
					}
					for (int k = 0; k < 8; ++k)
					{
						data.mpm_ext[k][dstp] = unit->mpm_ext[k][srcp];
					}
					for (int k = 0; k < N_C; ++k)
					{
						data.num_nz_coef[k][dstp] = unit->num_nz_coef[k][srcp];
					}
#if CUDQP_QP_MAP
					data.cu_luma_qp[dstp] = unit->cu_luma_qp[srcp];
#endif
				}
			}

			for (int i = 0; i < CU_DATA_UNIT_SIZE; ++i)
			{
#pragma HLS PIPELINE
				for (int j = 0; j < CU_DATA_UNIT_SIZE; ++j)
				{
					int i_dst = i_unit * CU_DATA_UNIT_SIZE + i;
					int j_dst = j_unit * CU_DATA_UNIT_SIZE + j;

					data.coef_y[i_dst][j_dst] = unit->coef_y[i][j];
					data.reco_y[i_dst][j_dst] = unit->reco_y[i][j];
				}
			}
		}
	}
}

inline void write_split(int x, int y, int wlog2, int hlog2, int cud, int split_mode, CuDataPtr& ptr, CuDataPool& cu_data)
{
	int x_cur_cu_scu = x >> 2;
	int y_cur_cu_scu = y >> 2;
	int cu_width = 1 << wlog2;
	int cu_height = 1 << hlog2;
	int prev_up_log2_sub_cuw = 6;
	int idx_dst = ((y_cur_cu_scu + (U9)(cu_height >> 1 >> 2)) << (prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
	U3 shape = SQUARE + (wlog2 - hlog2);
	for (int i = 0; i < UNIT_IN_LCU_SIZE; ++i)
	{
		for (int j = 0; j < UNIT_IN_LCU_SIZE; ++j)
		{
#pragma HLS pipeline
			if (ptr.idx[i][j] >= 0)
			{
				(cu_data[ptr.idx[i][j]].split_mode[cud][shape] + idx_dst)[(cu_width >> 1 >> 2)] = split_mode;
				return;
			}
		}
	}
}

inline void write_split(PoolUnit* mem, int x, int y, int wlog2, int hlog2, int cud, int split_mode)
{
	int w = 1 << wlog2;
	int h = 1 << hlog2;


	U3 shape = SQUARE + (wlog2 - hlog2);
	int x_cur_cu_scu = x >> 2;
	int y_cur_cu_scu = y >> 2;
	int prev_up_log2_sub_cuw = 6;
	int idx_dst = ((y_cur_cu_scu + (U9)(h >> 1 >> 2)) << (prev_up_log2_sub_cuw - 2)) + x_cur_cu_scu;
	(mem->split_mode[cud][shape] + idx_dst)[(w >> 1 >> 2)] = split_mode;
}
