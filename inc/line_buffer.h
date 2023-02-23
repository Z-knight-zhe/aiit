#pragma once
#include "enc_def.h"
#include "md_utils.h"

struct MapScu PUBLIC_C DeleteOperator
{
	U32 map_scu;
	U32 map_cu_mode;
	S14 mv[REFP_NUM][MV_D];
	S2 refi[REFP_NUM];
	U32 ip_map_scu;
	pel rec[MIN_CU_SIZE];
	S8 ipm;

	void init()
	{
#pragma HLS inline
#pragma HLS ARRAY_PARTITION variable=rec dim=1 cyclic factor=4
		map_scu = 0;
		map_cu_mode = 0;
		ip_map_scu = 0;
#if !SIMPLE_MD
		for (int i = 0; i < REFP_NUM; ++i)
		{
#pragma HLS pipeline
			for (int j = 0; j < MV_D; ++j)
			{
				mv[i][j] = 0;
			}
			refi[i] = -1;
		}
#endif // !SIMPLE_MD
		for (int i = 0; i < MIN_CU_SIZE; ++i)
		{
#pragma HLS UNROLL
			rec[i] = 0;
		}
		ipm = 0;
	}

	friend void copy(MapScu& dst, const MapScu& src)
	{
#pragma HLS inline
		dst.map_scu = src.map_scu;
		dst.map_cu_mode = src.map_cu_mode;
		dst.ip_map_scu = src.ip_map_scu;
#if !SIMPLE_MD
		for (int i = 0; i < REFP_NUM; ++i)
		{
#pragma HLS pipeline
			for (int j = 0; j < MV_D; ++j)
			{
				dst.mv[i][j] = src.mv[i][j];
			}
			dst.refi[i] = src.refi[i];
		}
#endif // !SIMPLE_MD
		for (int i = 0; i < MIN_CU_SIZE; ++i)
		{
#pragma HLS UNROLL
			dst.rec[i] = src.rec[i];
		}
		dst.ipm = src.ipm;
	}
	friend void copy(MapScu& dst1, MapScu& dst2, const MapScu& src)
	{
		dst1.map_scu = src.map_scu;
		dst2.map_scu = src.map_scu;
		dst1.map_cu_mode = src.map_cu_mode;
		dst2.map_cu_mode = src.map_cu_mode;
		dst1.ip_map_scu = src.ip_map_scu;
		dst2.ip_map_scu = src.ip_map_scu;
#if !SIMPLE_MD
		for (int i = 0; i < REFP_NUM; ++i)
		{
			for (int j = 0; j < MV_D; ++j)
			{
#pragma HLS pipeline
				dst1.mv[i][j] = src.mv[i][j];
				dst2.mv[i][j] = src.mv[i][j];
			}
			dst1.refi[i] = src.refi[i];
			dst2.refi[i] = src.refi[i];
		}
#endif // !SIMPLE_MD
		for (int i = 0; i < MIN_CU_SIZE; ++i)
		{
#pragma HLS UNROLL
			dst1.rec[i] = src.rec[i];
			dst2.rec[i] = src.rec[i];
		}
		dst1.ipm = src.ipm;
		dst2.ipm = src.ipm;
	}
};

struct LineBuffer PUBLIC_C DeleteOperator
{
	MapScu up[32];
	MapScu left[32];
	MapScu topleft[32];
	U8 y[32];
	U8 y0;

	void init()
	{
		for (int i = 0; i < 32; ++i)
		{
#pragma HLS pipeline
			up[i].init();
			left[i].init();
			topleft[i].init();
			y[i] = 0;
		}
		y0 = 0;
	}

	friend void copy(LineBuffer& dst, const LineBuffer& src)
	{
#pragma HLS inline
		for (int i = 0; i < 32; ++i)
		{
#pragma HLS pipeline
			copy(dst.up[i], src.up[i]);
			copy(dst.left[i], src.left[i]);
			copy(dst.topleft[i], src.topleft[i]);
			dst.y[i] = src.y[i];
		}
		dst.y0 = src.y0;
	}

	friend void copy_excepty0(LineBuffer& dst, const LineBuffer& src)
	{
#pragma HLS ARRAY_PARTITION variable=dst.topleft[0].rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst.up[0].rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst.left[0].rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst.topleft[0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst.topleft[0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=dst.up[0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst.up[0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=dst.left[0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=dst.left[0].mv complete dim=2

#pragma HLS ARRAY_PARTITION variable=src.topleft[0].rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=src.up[0].rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=src.left[0].rec complete dim=1
#pragma HLS ARRAY_PARTITION variable=src.topleft[0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=src.topleft[0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=src.up[0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=src.up[0].mv complete dim=2
#pragma HLS ARRAY_PARTITION variable=src.left[0].mv complete dim=1
#pragma HLS ARRAY_PARTITION variable=src.left[0].mv complete dim=2
		for (int i = 0; i < 4; ++i)
		{
#pragma HLS pipeline
			for (int j = 0; j < 8; j++)
			{
				copy(dst.up[i*8+j], src.up[i*8+j]);
				copy(dst.left[i*8+j], src.left[i * 8 + j]);
				copy(dst.topleft[i * 8 + j], src.topleft[i * 8 + j]);
				dst.y[i * 8 + j] = src.y[i * 8 + j];
			}
		}
	}
};

struct LcuLineBuffer
{
	MapScu up[960];
	MapScu left[16];
	MapScu topleft;

	void init_up()
	{
#pragma HLS INLINE OFF
#pragma HLS ARRAY_PARTITION variable=up cyclic factor=16 dim=1
		for (int i = 0; i < 960; ++i)
		{
#pragma HLS pipeline
#pragma HLS UNROLL factor=16
			up[i].init();
		}
	}
	void init_left()
	{
#pragma HLS INLINE OFF
		for (int i = 0; i < 16; ++i)
		{
#pragma HLS pipeline
			left[i].init();
		}
	}
	void init()
	{
		init_up();
		init_left();
		topleft.init();
	}
};

inline void load_line_buffer(int x, int y, LineBuffer& lcu, const LcuLineBuffer& frame)
{
	assert((x & 63) == 0);
	assert((y & 63) == 0);
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < 16; ++i)
	{
#pragma HLS pipeline
		// left[0] - left[15]
	if (x == 0)
	{
			lcu.left[i].init();
		}
		else
		{
			copy(lcu.left[i], frame.left[i]);
		}
		if (x + i < 960)
		{
			copy(lcu.up[i], lcu.topleft[i + 1], frame.up[x + i]);
	}
	else
	{
			lcu.topleft[i + 1].init();
			copy(lcu.up[i], frame.up[x + i]);
		}
		lcu.y[i] = 0;
	}
	for (int i = 16; i < 32; ++i)
	{
#pragma HLS pipeline
		lcu.left[i].init();
		if (i < 31)
		{
		if (x + i < 960)
		{
				copy(lcu.up[i], lcu.topleft[i + 1], frame.up[x + i]);
		}
		else
		{
				lcu.topleft[i + 1].init();
			lcu.up[i].init();
			}
		}
		else  // up[31], topleft[0]
		{
			if (x + 31 < 960)
			{
				copy(lcu.up[31], frame.up[x + 31]);
			}
			else
			{
				lcu.up[31].init();
	}
	if (x == 0)
	{
		lcu.topleft[0].init();
	}
	else
	{
		copy(lcu.topleft[0], frame.topleft);
	}
		}

		lcu.y[i] = 0;
		}

	lcu.y0 = 16;
}

inline void update_line_buffer(int x, int y, int w, int h, LcuLineBuffer& frame, const LineBuffer& lcu)
{
	assert((x & 63) == 0);
	assert((y & 63) == 0);
	assert(w == 64);
	assert(h == 64);
	x >>= 2;
	y >>= 2;

	//assert(lcu.y0 == 16);

	if (x + 15 >= 960 || y == 0)
	{
		frame.topleft.init();
	}
	else
	{
		copy(frame.topleft, frame.up[x + 15]);
	}
	for (int i = 0; i < 16; ++i)
	{
#pragma HLS pipeline
		copy(frame.up[x + i], lcu.up[i]);
		copy(frame.left[i], lcu.left[i]);
		//assert(lcu.y[i] == 16);
	}
	for (int i = 16; i < 32; ++i)
	{
#pragma HLS pipeline
		if (x + i < 960)
		{
			copy(frame.up[x + i], lcu.up[i]);
		}
	}
}

template <size_t _Size>
void load_line_buffer(int x, int y, ENC_ME_LINE_MAP_SIZE<_Size>& me_line_map, LineBuffer& line_buffer)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Size >> 2); ++i)
	{
#pragma HLS pipeline
		me_line_map.map_scu_up_line[i + 1] = line_buffer.up[x + i].map_scu;
		me_line_map.map_cu_mode_up_line[i + 1] = line_buffer.up[x + i].map_cu_mode;
	}
	if (line_buffer.y[x + (_Size >> 2)] == y)
	{
		me_line_map.map_scu_up_line[(_Size >> 2) + 1] = line_buffer.up[x + (_Size >> 2)].map_scu;
		me_line_map.map_cu_mode_up_line[(_Size >> 2) + 1] = line_buffer.up[x + (_Size >> 2)].map_cu_mode;
	}
	else
	{
		me_line_map.map_scu_up_line[(_Size >> 2) + 1] = 0;
		me_line_map.map_cu_mode_up_line[(_Size >> 2) + 1] = 0;
	}
	for (int i = 0; i < (_Size >> 2); ++i)
	{
#pragma HLS pipeline
		me_line_map.map_scu_left_line[i] = line_buffer.left[y + i].map_scu;
		me_line_map.map_cu_mode_left_line[i] = line_buffer.left[y + i].map_cu_mode;
	}
	me_line_map.map_scu_up_line[0] = line_buffer.topleft[x].map_scu;
	me_line_map.map_cu_mode_up_line[0] = line_buffer.topleft[x].map_cu_mode;
}

template <size_t _Width, size_t _Height>
void load_line_buffer(int x, int y, IP_ARRAY_SIZE<_Width, _Height>& ip_array, LineBuffer& line_buffer)
{
	x &= 63;
	y &= 63;

	for (int i = 0; i < _Width; ++i)
	{
#pragma HLS pipeline
		ip_array.up_line[i] = line_buffer.up[(x + i) >> 2].rec[(x + i) & 3];
	}
	for (int i = _Width; i < (_Width << 1); ++i)
	{
#pragma HLS pipeline
		if (line_buffer.y[(x + i) >> 2] == (y >> 2))
		{
			ip_array.up_line[i] = line_buffer.up[(x + i) >> 2].rec[(x + i) & 3];
		}
		else
		{
			ip_array.up_line[i] = 0;
		}
	}
	for (int i = 0; i < _Height; ++i)
	{
#pragma HLS pipeline
		ip_array.left_line[i] = line_buffer.left[(y + i) >> 2].rec[(y + i) & 3];
	}
	if (x == 0)
	{
		for (int i = _Height; i < (_Height << 1); ++i)
		{
			if (line_buffer.y0 > ((y + i) >> 2))
			{
				ip_array.left_line[i] = line_buffer.left[(y + i) >> 2].rec[(y + i) & 3];
			}
			else
			{
				ip_array.left_line[i] = 0;
			}
		}
	}
	else
	{
		for (int i = _Height; i < (_Height << 1); ++i)
		{
#pragma HLS pipeline
			if (line_buffer.y[(x >> 2) - 1] > ((y + i) >> 2))
			{
				ip_array.left_line[i] = line_buffer.left[(y + i) >> 2].rec[(y + i) & 3];
			}
			else
			{
				ip_array.left_line[i] = 0;
			}
		}
	}
	ip_array.top_left = line_buffer.topleft[x >> 2].rec[3];
}

template <size_t _Width, size_t _Height>
void load_line_buffer(int x, int y, MPM_ARRAY_SIZE<_Width, _Height>& mpm_buffer, LineBuffer& line_buffer)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Width >> 2); ++i)
	{
#pragma HLS pipeline
		mpm_buffer.up_line[i] = line_buffer.up[x + i].ipm;
	}
	for (int i = 0; i < (_Height >> 2); ++i)
	{
#pragma HLS pipeline
		mpm_buffer.left_line[i] = line_buffer.left[y + i].ipm;
	}
	mpm_buffer.up = mpm_buffer.up_line[0];
	mpm_buffer.left = mpm_buffer.left_line[0];
}

template <size_t _Width, size_t _Height>
void load_line_buffer(int x, int y, IP_MAP_SCU_SIZE<_Width, _Height>& ip_map_scu, LineBuffer& line_buffer)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Width >> 2); ++i)
	{
#pragma HLS pipeline
		ip_map_scu.map_scu_up_line[i] = line_buffer.up[x + i].ip_map_scu;
	}
	for (int i = (_Width >> 2); i < (_Width >> 1); ++i)
	{
#pragma HLS pipeline
		if (line_buffer.y[x + i] == y)
		{
			ip_map_scu.map_scu_up_line[i] = line_buffer.up[x + i].ip_map_scu;
		}
		else
		{
			ip_map_scu.map_scu_up_line[i] = 0;
		}
	}
	for (int i = 0; i < (_Height >> 2); ++i)
	{
#pragma HLS pipeline
		ip_map_scu.map_scu_left_line_new[i] = line_buffer.left[y + i].ip_map_scu;
	}
	if (x == 0)
	{
		for (int i = (_Height >> 2); i < (_Height >> 1); ++i)
		{
#pragma HLS pipeline
			if (line_buffer.y0 > y + i)
			{
				ip_map_scu.map_scu_left_line_new[i] = line_buffer.left[y + i].ip_map_scu;
			}
			else
			{
				ip_map_scu.map_scu_left_line_new[i] = 0;
			}
		}
	}
	else
	{
		for (int i = (_Height >> 2); i < (_Height >> 1); ++i)
		{
#pragma HLS pipeline
			if (line_buffer.y[x - 1] > y + i)
			{
				ip_map_scu.map_scu_left_line_new[i] = line_buffer.left[y + i].ip_map_scu;
			}
			else
			{
				ip_map_scu.map_scu_left_line_new[i] = 0;
			}
		}
	}
	ip_map_scu.map_scu_top_left_new = line_buffer.topleft[x].ip_map_scu;
}

template <size_t _Width, size_t _Height>
void load_line_buffer(int x, int y, ENC_ME_NEB_INFO up_nebs[(_Width >> 2) + 2], ENC_ME_NEB_INFO left_nebs[_Height >> 2], LineBuffer& line_buffer)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Width >> 2); ++i)
	{
#pragma HLS pipeline
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				up_nebs[i + 1].mv[j][k] = line_buffer.up[x + i].mv[j][k];
			}
			up_nebs[i + 1].refi[j] = line_buffer.up[x + i].refi[j];
		}
	}
	if (line_buffer.y[x + (_Width >> 2)] == y)
	{
		for (int j = 0; j < REFP_NUM; ++j)
		{
#pragma HLS pipeline
			for (int k = 0; k < MV_D; ++k)
			{
				up_nebs[(_Width >> 2) + 1].mv[j][k] = line_buffer.up[x + (_Width >> 2)].mv[j][k];
			}
			up_nebs[(_Width >> 2) + 1].refi[j] = line_buffer.up[x + (_Width >> 2)].refi[j];
		}
	}
	else
	{
		for (int j = 0; j < REFP_NUM; ++j)
		{
#pragma HLS pipeline
			for (int k = 0; k < MV_D; ++k)
			{
				up_nebs[(_Width >> 2) + 1].mv[j][k] = 0;
			}
			up_nebs[(_Width >> 2) + 1].refi[j] = -1;
		}
	}
	for (int i = 0; i < (_Height >> 2); ++i)
	{
#pragma HLS pipeline
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				left_nebs[i].mv[j][k] = line_buffer.left[y + i].mv[j][k];
			}
			left_nebs[i].refi[j] = line_buffer.left[y + i].refi[j];
		}
	}
	for (int j = 0; j < REFP_NUM; ++j)
	{
#pragma HLS pipeline
		for (int k = 0; k < MV_D; ++k)
		{
			up_nebs[0].mv[j][k] = line_buffer.topleft[x].mv[j][k];
		}
		up_nebs[0].refi[j] = line_buffer.topleft[x].refi[j];
	}
}

template <size_t _Width, size_t _Height>
void update_line_buffer(int x, int y, int w, int h, LineBuffer& line_buffer_input, LineBuffer& line_buffer_output, pel rec[_Height][_Width])
{
	for (int i = 0; i < MIN_CU_SIZE; ++i)
	{
#pragma HLS pipeline
		line_buffer_output.topleft[x].rec[i] = line_buffer_input.left[y + h - 1].rec[i];
	}
	for (int i = 1; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < MIN_CU_SIZE; ++j)
		{
			line_buffer_output.topleft[x + i].rec[j] = rec[(h << 2) - 1][((i - 1) << 2) + j];
		}
		line_buffer_output.y[x + i] = y + h;
	}
	for (int i = 0; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < MIN_CU_SIZE; ++j)
		{
			line_buffer_output.up[x + i].rec[j] = rec[(h << 2) - 1][(i << 2) + j];
		}
	}
	for (int i = 0; i < h; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < MIN_CU_SIZE; ++j)
		{
			line_buffer_output.left[y + i].rec[j] = rec[(i << 2) + j][(w << 2) - 1];
		}
	}
}

template <size_t _Width, size_t _Height>
void update_line_buffer(int x, int y, int w, int h, LineBuffer& line_buffer_input, LineBuffer& line_buffer_output, S8 ipm[(_Width >> 2) * (_Height >> 2)])
{
	line_buffer_output.topleft[x].ipm = line_buffer_input.left[y + h - 1].ipm;
	for (int i = 1; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.topleft[x + i].ipm = ipm[w * (h - 1) + i - 1];
		line_buffer_output.y[x + i] = y + h;
	}
	for (int i = 0; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.up[x + i].ipm = ipm[w * (h - 1) + i];
	}
	for (int i = 0; i < h; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.left[y + i].ipm = ipm[w * i + w - 1];
	}
}

template <size_t _Width, size_t _Height>
void update_line_buffer(int x, int y, int w, int h, LineBuffer& line_buffer_input, LineBuffer& line_buffer_output, U32 map_scu[(_Width >> 2) * (_Height >> 2)], U32 map_cu_mode[(_Width >> 2) * (_Height >> 2)])
{
	line_buffer_output.topleft[x].map_scu = line_buffer_input.left[y + h - 1].map_scu;
	line_buffer_output.topleft[x].map_cu_mode = line_buffer_input.left[y + h - 1].map_cu_mode;
	for (int i = 1; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.topleft[x + i].map_scu = map_scu[w * (h - 1) + i - 1];
		line_buffer_output.topleft[x + i].map_cu_mode = map_cu_mode[w * (h - 1) + i - 1];
		line_buffer_output.y[x + i] = y + h;
	}
	for (int i = 0; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.up[x + i].map_scu = map_scu[w * (h - 1) + i];
		line_buffer_output.up[x + i].map_cu_mode = map_cu_mode[w * (h - 1) + i];
	}
	for (int i = 0; i < h; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.left[y + i].map_scu = map_scu[w * i + w - 1];
		line_buffer_output.left[y + i].map_cu_mode = map_cu_mode[w * i + w - 1];
	}
}

template <size_t _Width, size_t _Height>
void update_line_buffer(int x, int y, int w, int h, LineBuffer& line_buffer_input, LineBuffer& line_buffer_output, U32 ip_map_scu[(_Width >> 2) * (_Height >> 2)])
{
	line_buffer_output.topleft[x].ip_map_scu = line_buffer_input.left[y + h - 1].ip_map_scu;
	for (int i = 1; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.topleft[x + i].ip_map_scu = ip_map_scu[w * (h - 1) + i - 1];
		line_buffer_output.y[x + i] = y + h;
	}
	for (int i = 0; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.up[x + i].ip_map_scu = ip_map_scu[w * (h - 1) + i];
	}
	for (int i = 0; i < h; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		line_buffer_output.left[y + i].ip_map_scu = ip_map_scu[w * i + w - 1];
	}
}

template <size_t _Width, size_t _Height>
void update_line_buffer(int x, int y, int w, int h, LineBuffer& line_buffer_input, LineBuffer& line_buffer_output, S3 refi[(_Width >> 2) * (_Height >> 2)][2], S14 mv[(_Width >> 2) * (_Height >> 2)][2][2])
{
	for (int j = 0; j < REFP_NUM; ++j)
	{
#pragma HLS pipeline
		for (int k = 0; k < MV_D; ++k)
		{
			line_buffer_output.topleft[x].mv[j][k] = line_buffer_input.left[y + h - 1].mv[j][k];
		}
		line_buffer_output.topleft[x].refi[j] = line_buffer_input.left[y + h - 1].refi[j];
	}
	for (int i = 1; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer_output.topleft[x + i].mv[j][k] = mv[w * (h - 1) + i - 1][j][k];
			}
			line_buffer_output.topleft[x + i].refi[j] = refi[w * (h - 1) + i - 1][j];
		}
		line_buffer_output.y[x + i] = y + h;
	}
	for (int i = 0; i < w; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer_output.up[x + i].mv[j][k] = mv[w * (h - 1) + i][j][k];
			}
			line_buffer_output.up[x + i].refi[j] = refi[w * (h - 1) + i][j];
		}
	}
	for (int i = 0; i < h; ++i)
	{
#pragma HLS pipeline
#pragma HLS LOOP_TRIPCOUNT max=64
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer_output.left[y + i].mv[j][k] = mv[w * i + w - 1][j][k];
			}
			line_buffer_output.left[y + i].refi[j] = refi[w * i + w - 1][j];
		}
	}
}

template <size_t _Size>
void copy(int x, int y, LineBuffer& line_buffer, ENC_ME_LINE_MAP_SIZE<_Size>& me_line_map)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Size >> 2) + 1; ++i)
	{
#pragma HLS pipeline
		line_buffer.up[x + i].map_scu = me_line_map.map_scu_up_line[i + 1];
		line_buffer.up[x + i].map_cu_mode = me_line_map.map_cu_mode_up_line[i + 1];
	}
	for (int i = 0; i < (_Size >> 2); ++i)
	{
#pragma HLS pipeline
		line_buffer.left[y + i].map_scu = me_line_map.map_scu_left_line[i];
		line_buffer.left[y + i].map_cu_mode = me_line_map.map_cu_mode_left_line[i];
	}
	for (int i = 0; i < (_Size >> 2) + 2; ++i)
	{
#pragma HLS pipeline
		line_buffer.topleft[x + i].map_scu = me_line_map.map_scu_up_line[i];
		line_buffer.topleft[x + i].map_cu_mode = me_line_map.map_cu_mode_up_line[i];
		line_buffer.y[x + i] = y;
	}
}

template <size_t _Width, size_t _Height>
void copy(int x, int y, LineBuffer& line_buffer, IP_ARRAY_SIZE<_Width, _Height>& ip_array)
{
	x &= 63;
	y &= 63;

	for (int i = 0; i < (_Width << 1); ++i)
	{
#pragma HLS pipeline
		line_buffer.up[(x + i) >> 2].rec[(x + i) & 3] = ip_array.up_line[i];
	}
	for (int i = 0; i < (_Height << 1); ++i)
	{
#pragma HLS pipeline
		line_buffer.left[(y + i) >> 2].rec[(y + i) & 3] = ip_array.left_line[i];
	}
	line_buffer.topleft[x >> 2].rec[3] = ip_array.top_left;
	line_buffer.y[x >> 2] = y >> 2;
	for (int i = 0; i < (_Width << 1); ++i)
	{
#pragma HLS pipeline
		line_buffer.topleft[((x + i) >> 2) + 1].rec[(x + i) & 3] = ip_array.up_line[i];
		line_buffer.y[((x + i) >> 2)] = y >> 2;
	}
}

template <size_t _Width, size_t _Height>
void copy(int x, int y, LineBuffer& line_buffer, IP_MAP_SCU_SIZE<_Width, _Height>& ip_map_scu)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Width >> 1); ++i)
	{
#pragma HLS pipeline
		line_buffer.up[x + i].ip_map_scu = ip_map_scu.map_scu_up_line[i];
	}
	for (int i = 0; i < (_Height >> 1); ++i)
	{
#pragma HLS pipeline
		line_buffer.left[y + i].ip_map_scu = ip_map_scu.map_scu_left_line_new[i];
	}
	line_buffer.topleft[x].ip_map_scu = ip_map_scu.map_scu_top_left_new;
	line_buffer.y[x] = y;
	for (int i = 0; i < (_Width >> 1); ++i)
	{
#pragma HLS pipeline
		line_buffer.topleft[x + i + 1].ip_map_scu = ip_map_scu.map_scu_up_line[i];
		line_buffer.y[x + i + 1] = y;
	}
}

template <size_t _Width, size_t _Height>
void copy(int x, int y, LineBuffer& line_buffer, MPM_ARRAY_SIZE<_Width, _Height>& mpm_buffer)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Width >> 2); ++i)
	{
#pragma HLS pipeline
		line_buffer.up[x + i].ipm = mpm_buffer.up_line[i];
	}
	for (int i = 0; i < (_Height >> 2); ++i)
	{
#pragma HLS pipeline
		line_buffer.left[y + i].ipm = mpm_buffer.left_line[i];
	}
	//line_buffer.topleft[x].ipm = mpm_buffer.top_left;
	//line_buffer.y[x] = y;
	for (int i = 0; i < (_Width >> 2); ++i)
	{
#pragma HLS pipeline
		line_buffer.topleft[x + i + 1].ipm = mpm_buffer.up_line[i];
		line_buffer.y[x + i + 1] = y;
	}
}

template <size_t _Width, size_t _Height>
void copy(int x, int y, LineBuffer& line_buffer, ENC_ME_NEB_INFO up_nebs[(_Width >> 2) + 2], ENC_ME_NEB_INFO left_nebs[_Height >> 2])
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;

	for (int i = 0; i < (_Width >> 2) + 1; ++i)
	{
#pragma HLS pipeline
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer.up[x + i].mv[j][k] = up_nebs[i + 1].mv[j][k];
			}
			line_buffer.up[x + i].refi[j] = up_nebs[i + 1].refi[j];
		}
	}
	for (int i = 0; i < (_Height >> 2); ++i)
	{
#pragma HLS pipeline
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer.left[y + i].mv[j][k] = left_nebs[i].mv[j][k];
			}
			line_buffer.left[y + i].refi[j] = left_nebs[i].refi[j];
		}
	}
	for (int i = 0; i < (_Width >> 2) + 2; ++i)
	{
#pragma HLS pipeline
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer.topleft[x + i].mv[j][k] = up_nebs[i].mv[j][k];
			}
			line_buffer.topleft[x + i].refi[j] = up_nebs[i].refi[j];
		}
		line_buffer.y[x + i] = y;
	}
}

template <size_t _Size, size_t _Width, size_t _Height>
void load_line_buffer(int x, int y, ENC_ME_LINE_MAP_SIZE<_Size>& me_line_map, IP_ARRAY_SIZE<_Width, _Height>& ip_array, MPM_ARRAY_SIZE<_Width, _Height>& mpm_buffer, ENC_ME_NEB_INFO up_nebs[(_Width >> 2) + 2], ENC_ME_NEB_INFO left_nebs[_Height >> 2], IP_MAP_SCU_SIZE<_Width, _Height>& IP_map_scu, LineBuffer& line_buffer, LCUConfig& config)
{
	load_line_buffer(x, y, ip_array, line_buffer);
	load_line_buffer(x, y, mpm_buffer, line_buffer);
	load_line_buffer(x, y, me_line_map, line_buffer);
	me_line_map.min_scu_x = config.min_scu_x;
	me_line_map.min_scu_y = config.min_scu_y;
	load_line_buffer<_Width, _Height>(x, y, up_nebs, left_nebs, line_buffer);
	load_line_buffer(x, y, IP_map_scu, line_buffer);
	me_line_map.print_line();
	ip_array.print_line();
	mpm_buffer.print_line();
#if PRINT_NEB_LINE
	print<_Width, _Height>(up_nebs, left_nebs);
#endif
	IP_map_scu.print_line();
}

template <size_t _Size, size_t _Width, size_t _Height>
void load16(int x, int y, ENC_ME_LINE_MAP_SIZE<_Size>& me_line_map, IP_ARRAY_SIZE<_Width, _Height>& ip_array, MPM_ARRAY_SIZE<_Width, _Height>& mpm_buffer, ENC_ME_NEB_INFO up_nebs[6], ENC_ME_NEB_INFO left_nebs[4], IP_MAP_SCU_SIZE<_Width, _Height>& IP_map_scu, LineBuffer& line_buffer, LCUConfig& config)
{
	x &= 63;
	y &= 63;
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.up[0].mv complete dim=1
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.up[0].mv complete dim=2
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.up[0].rec complete dim=0
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.left[0].mv complete dim=1
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.left[0].mv complete dim=2
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.left[0].rec complete dim=0
 //#pragma HLS ARRAY_PARTITION variable=line_buffer.y complete dim=0
 
 //#pragma HLS ARRAY_PARTITION variable=up_nebs[0].mv complete dim=1
 //#pragma HLS ARRAY_PARTITION variable=up_nebs[0].mv complete dim=2

 //#pragma HLS ARRAY_PARTITION variable=left_nebs[0].mv complete dim=1
 //#pragma HLS ARRAY_PARTITION variable=left_nebs[0].mv complete dim=2
for (int i = 0; i < 4; i++)
	{
#pragma HLS pipeline
		for (int j = 0; j < 8; j++)
		{
			if (i * 8 +j < 16)
			{
				ip_array.up_line[i * 8 +j] = line_buffer.up[(x + i * 8 +j) >> 2].rec[(x + i * 8 +j) & 3];
				ip_array.left_line[i * 8 +j] = line_buffer.left[(y + i * 8 +j) >> 2].rec[(y + i * 8 + j) & 3];
			}
			else
			{
				if (line_buffer.y[(x + i * 8 + j) >> 2] == (y >> 2))
				{
					ip_array.up_line[i * 8 + j] = line_buffer.up[(x + i * 8 + j) >> 2].rec[(x + i * 8 + j) & 3];
				}
				else
				{
					ip_array.up_line[i * 8 + j] = 0;
				}
				if (x == 0)
				{
					if (line_buffer.y0 > ((y + i * 8 + j) >> 2))
					{
						ip_array.left_line[i * 8 + j] = line_buffer.left[(y + i * 8 + j) >> 2].rec[(y + i * 8 + j) & 3];
					}
					else
					{
						ip_array.left_line[i * 8 + j] = 0;
					}
				}
				else
				{
					if (line_buffer.y[(x >> 2) - 1] > ((y + i * 8 + j) >> 2))
					{
						ip_array.left_line[i * 8 + j] = line_buffer.left[(y + i * 8 + j) >> 2].rec[(y + i * 8 + j) & 3];
					}
					else
					{
						ip_array.left_line[i * 8 + j] = 0;
					}
				}
			}
		}
		IP_map_scu.map_scu_up_line[i] = line_buffer.up[(x >> 2) + i].ip_map_scu;
    	IP_map_scu.map_scu_left_line_new[i] = line_buffer.left[(y >> 2) + i].ip_map_scu;
		mpm_buffer.up_line[i] = line_buffer.up[(x >> 2) + i].ipm;
   		mpm_buffer.left_line[i] = line_buffer.left[(y >> 2) + i].ipm;
    	me_line_map.map_scu_up_line[i + 1] = line_buffer.up[(x >> 2) + i].map_scu;
		me_line_map.map_cu_mode_up_line[i + 1] = line_buffer.up[(x >> 2) + i].map_cu_mode;
    	me_line_map.map_scu_left_line[i] = line_buffer.left[(y >> 2) + i].map_scu;
		me_line_map.map_cu_mode_left_line[i] = line_buffer.left[(y >> 2) + i].map_cu_mode;
   		for (int j = 0; j < REFP_NUM; j++)
		{
			for (int k = 0; k < MV_D; k++)
			{
			up_nebs[i + 1].mv[j][k] = line_buffer.up[(x >> 2) + i].mv[j][k];
       		left_nebs[i].mv[j][k] = line_buffer.left[(y >> 2) + i].mv[j][k];        
			}
			up_nebs[i + 1].refi[j] = line_buffer.up[(x >> 2) + i].refi[j];
      		left_nebs[i].refi[j] = line_buffer.left[(y >> 2) + i].refi[j];      
		}   
	}
	
	ip_array.top_left = line_buffer.topleft[x >> 2].rec[3];

	x >>= 2;
	y >>= 2;
// 	for (int i = 0; i < 4; ++i)
// 	{
// #pragma HLS pipeline
//     IP_map_scu.map_scu_up_line[i] = line_buffer.up[x + i].ip_map_scu;
//     IP_map_scu.map_scu_left_line_new[i] = line_buffer.left[y + i].ip_map_scu;
// 		mpm_buffer.up_line[i] = line_buffer.up[x + i].ipm;
//    	mpm_buffer.left_line[i] = line_buffer.left[y + i].ipm;
//     me_line_map.map_scu_up_line[i + 1] = line_buffer.up[x + i].map_scu;
// 		me_line_map.map_cu_mode_up_line[i + 1] = line_buffer.up[x + i].map_cu_mode;
//     me_line_map.map_scu_left_line[i] = line_buffer.left[y + i].map_scu;
// 		me_line_map.map_cu_mode_left_line[i] = line_buffer.left[y + i].map_cu_mode;
//    for (int j = 0; j < REFP_NUM; ++j)
// 		{
// 			for (int k = 0; k < MV_D; ++k)
// 			{
// 				up_nebs[i + 1].mv[j][k] = line_buffer.up[x + i].mv[j][k];
//         left_nebs[i].mv[j][k] = line_buffer.left[y + i].mv[j][k];        
// 			}
// 			up_nebs[i + 1].refi[j] = line_buffer.up[x + i].refi[j];
//       left_nebs[i].refi[j] = line_buffer.left[y + i].refi[j];      
// 		}                
// 	}
//     ip_array.top_left = line_buffer.topleft[x].rec[3];
	
	mpm_buffer.up = mpm_buffer.up_line[0];
	mpm_buffer.left = mpm_buffer.left_line[0];
 
	
	if (line_buffer.y[x + (_Size >> 2)] == y)
	{
		me_line_map.map_scu_up_line[(_Size >> 2) + 1] = line_buffer.up[x + (_Size >> 2)].map_scu;
		me_line_map.map_cu_mode_up_line[(_Size >> 2) + 1] = line_buffer.up[x + (_Size >> 2)].map_cu_mode;
	}
	else
	{
		me_line_map.map_scu_up_line[(_Size >> 2) + 1] = 0;
		me_line_map.map_cu_mode_up_line[(_Size >> 2) + 1] = 0;
	}
	
	me_line_map.map_scu_up_line[0] = line_buffer.topleft[x].map_scu;
	me_line_map.map_cu_mode_up_line[0] = line_buffer.topleft[x].map_cu_mode;



	me_line_map.min_scu_x = config.min_scu_x;
	me_line_map.min_scu_y = config.min_scu_y;

//	if (line_buffer.y[x + (_Width >> 2)] == y)
//	{
//		for (int j = 0; j < REFP_NUM; ++j)
//		{
//#pragma HLS pipeline
//			for (int k = 0; k < MV_D; ++k)
//			{
//				up_nebs[(_Width >> 2) + 1].mv[j][k] = line_buffer.up[x + (_Width >> 2)].mv[j][k];
//			}
//			up_nebs[(_Width >> 2) + 1].refi[j] = line_buffer.up[x + (_Width >> 2)].refi[j];
//		}
//	}
//	else
//	{
//		for (int j = 0; j < REFP_NUM; ++j)
//		{
//#pragma HLS pipeline
//			for (int k = 0; k < MV_D; ++k)
//			{
//				up_nebs[(_Width >> 2) + 1].mv[j][k] = 0;
//			}
//			up_nebs[(_Width >> 2) + 1].refi[j] = -1;
//		}
//	}
//
//	for (int j = 0; j < REFP_NUM; ++j)
//	{
//#pragma HLS pipeline
//		for (int k = 0; k < MV_D; ++k)
//		{
//			up_nebs[0].mv[j][k] = line_buffer.topleft[x].mv[j][k];
//		}
//		up_nebs[0].refi[j] = line_buffer.topleft[x].refi[j];
//	}
	for (int j = 0; j < REFP_NUM; ++j)
	{
#pragma HLS pipeline
		if (line_buffer.y[x + (_Width >> 2)] == y)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				up_nebs[(_Width >> 2) + 1].mv[j][k] = line_buffer.up[x + (_Width >> 2)].mv[j][k];
			}
			up_nebs[(_Width >> 2) + 1].refi[j] = line_buffer.up[x + (_Width >> 2)].refi[j];
		}
		else
		{
			for (int k = 0; k < MV_D; ++k)
			{
				up_nebs[(_Width >> 2) + 1].mv[j][k] = 0;
			}
			up_nebs[(_Width >> 2) + 1].refi[j] = -1;
		}

		for (int k = 0; k < MV_D; ++k)
		{
			up_nebs[0].mv[j][k] = line_buffer.topleft[x].mv[j][k];
		}
		up_nebs[0].refi[j] = line_buffer.topleft[x].refi[j];
	}

	for (int i = 4; i < 8; ++i)
	{
#pragma HLS pipeline
		if (line_buffer.y[x + i] == y)
		{
			IP_map_scu.map_scu_up_line[i] = line_buffer.up[x + i].ip_map_scu;
		}
		else
		{
			IP_map_scu.map_scu_up_line[i] = 0;
		}
		if (x == 0)
		{
			if (line_buffer.y0 > y + i)
			{
				IP_map_scu.map_scu_left_line_new[i] = line_buffer.left[y + i].ip_map_scu;
			}
			else
			{
				IP_map_scu.map_scu_left_line_new[i] = 0;
			}
		}
		else
		{
			if (line_buffer.y[x - 1] > y + i)
			{
				IP_map_scu.map_scu_left_line_new[i] = line_buffer.left[y + i].ip_map_scu;
			}
			else
			{
				IP_map_scu.map_scu_left_line_new[i] = 0;
			}

		}
	}
	IP_map_scu.map_scu_top_left_new = line_buffer.topleft[x].ip_map_scu;
	me_line_map.print_line();
	ip_array.print_line();
	mpm_buffer.print_line();
#if PRINT_NEB_LINE
	print<_Width, _Height>(up_nebs, left_nebs);
#endif
	IP_map_scu.print_line();
}
//input output separation
template <size_t _Size>
void update_line_buffer(int x, int y, int w, int h, U8 y0, LineBuffer& line_buffer_input, LineBuffer& line_buffer_output, ENC_CU_DATA_ARRAY_SIZE<_Size>& cu_data)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;
	w >>= 2;
	h >>= 2;

	line_buffer_output.y[x] = y + h;
	if (x == 0)
	{
		if (y0 < y + h)
		{
#if !PU_16X16_FIX_AXIS_ERR
			line_buffer_output.y0 = y + h;
#endif // !PU_16X16_FIX_AXIS_ERR
		}
	}

	update_line_buffer<_Size, _Size>(x, y, w, h, line_buffer_input, line_buffer_output, cu_data.reco_y);
	update_line_buffer<_Size, _Size>(x, y, w, h, line_buffer_input, line_buffer_output, cu_data.ipm[0]);
	update_line_buffer<_Size, _Size>(x, y, w, h, line_buffer_input, line_buffer_output, cu_data.map_scu, cu_data.map_cu_mode);
	update_line_buffer<_Size, _Size>(x, y, w, h, line_buffer_input, line_buffer_output, cu_data.map_scu);
	update_line_buffer<_Size, _Size>(x, y, w, h, line_buffer_input, line_buffer_output, cu_data.refi, cu_data.mv);
}


template <size_t _Size>
void update16(int x, int y, int w, int h, U8 y0, LineBuffer& line_buffer_input,
	LineBuffer& line_buffer_output, ENC_CU_DATA_ARRAY_SIZE<_Size>& cu_data)
{
	x &= 63;
	y &= 63;
	x >>= 2;
	y >>= 2;
	w >>= 2;
	h >>= 2;

	line_buffer_output.y[x] = y + h;
	if (x == 0)
	{
		if (y0 < y + h)
		{
#if !PU_16X16_FIX_AXIS_ERR
			line_buffer_output.y0 = y + h;
#endif // !PU_16X16_FIX_AXIS_ERR
		}
	}

//#pragma HLS ARRAY_PARTITION variable=cu_data.mv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=cu_data.mv complete dim=3
//#pragma HLS ARRAY_PARTITION variable=cu_data.refi complete dim=2
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.topleft[0].rec complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.up[0].rec complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.left[0].rec complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.topleft[0].mv complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.topleft[0].mv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.up[0].mv complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.up[0].mv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.left[0].mv complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.left[0].mv complete dim=2
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.topleft[0].refi complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.up[0].refi complete dim=1
//#pragma HLS ARRAY_PARTITION variable=line_buffer_output.left[0].refi complete dim=1
	update16 <_Size, _Size>(x, y, w, h, line_buffer_input,
		line_buffer_output, cu_data.reco_y, cu_data.ipm[0],
		cu_data.map_scu, cu_data.map_scu,cu_data.map_cu_mode, cu_data.refi, cu_data.mv);


}


template <size_t _Width, size_t _Height>
void update16(int x, int y, int w, int h, LineBuffer& line_buffer_input,
	LineBuffer& line_buffer_output, pel rec[_Height][_Width], S8 ipm[(_Width >> 2) * (_Height >> 2)],
	U32 map_scu[(_Width >> 2) * (_Height >> 2)], U32 ip_map_scu[(_Width >> 2) * (_Height >> 2)],
	U32 map_cu_mode[(_Width >> 2) * (_Height >> 2)],
	S3 refi[(_Width >> 2) * (_Height >> 2)][2], S14 mv[(_Width >> 2) * (_Height >> 2)][2][2])
{
	line_buffer_output.topleft[x].map_scu = line_buffer_input.left[y + h - 1].map_scu;
	line_buffer_output.topleft[x].map_cu_mode = line_buffer_input.left[y + h - 1].map_cu_mode;
	line_buffer_output.topleft[x].ip_map_scu = line_buffer_input.left[y + h - 1].ip_map_scu;
	line_buffer_output.topleft[x].ipm = line_buffer_input.left[y + h - 1].ipm;
 
    line_buffer_output.up[x].map_scu = map_scu[w * (h - 1)];
		line_buffer_output.up[x].map_cu_mode = map_cu_mode[w * (h - 1)];
		line_buffer_output.up[x].ip_map_scu = ip_map_scu[w * (h - 1)];
		line_buffer_output.up[x].ipm = ipm[w * (h - 1)];

//#pragma HLS array_partition variable=rec cyclic factor=4 dim=1
//#pragma HLS array_partition variable=rec cyclic factor=4 dim=2
//#pragma HLS array_partition variable=mv complete dim=2
//#pragma HLS array_partition variable=mv complete dim=3
//#pragma HLS array_partition variable=map_scu complete dim=3

	for (int j = 0; j < REFP_NUM; ++j)
	{
#pragma HLS pipeline
		for (int k = 0; k < MV_D; ++k)
		{
      line_buffer_output.up[x].mv[j][k] = mv[w * (h - 1)][j][k];
			for (int i = 0; i < MIN_CU_SIZE; ++i)
				line_buffer_output.topleft[x].mv[j][k] = line_buffer_input.left[y + h - 1].mv[j][k];
		}
		line_buffer_output.topleft[x].refi[j] = line_buffer_input.left[y + h - 1].refi[j];
    line_buffer_output.up[x].refi[j] = refi[w * (h - 1)][j];
	}
	for (int i = 0; i < MIN_CU_SIZE; ++i)
	{
#pragma HLS pipeline
		line_buffer_output.topleft[x].rec[i] = line_buffer_input.left[y + h - 1].rec[i];
    line_buffer_output.up[x].rec[i] = rec[(h << 2) - 1][i];
	}

	for (int i = 1; i < 4; ++i)
	{
#pragma HLS pipeline
		line_buffer_output.topleft[x + i].map_scu = map_scu[w * (h - 1) + i - 1];
		line_buffer_output.topleft[x + i].map_cu_mode = map_cu_mode[w * (h - 1) + i - 1];
		line_buffer_output.topleft[x + i].ip_map_scu = ip_map_scu[w * (h - 1) + i - 1];
		line_buffer_output.topleft[x + i].ipm = ipm[w * (h - 1) + i - 1];
    line_buffer_output.up[x + i].map_scu = map_scu[w * (h - 1) + i];
		line_buffer_output.up[x + i].map_cu_mode = map_cu_mode[w * (h - 1) + i];
		line_buffer_output.up[x + i].ip_map_scu = ip_map_scu[w * (h - 1) + i];
		line_buffer_output.up[x + i].ipm = ipm[w * (h - 1) + i];
		line_buffer_output.y[x + i] = y + h;
		for (int j = 0; j < MIN_CU_SIZE; ++j)
		{
			line_buffer_output.topleft[x + i].rec[j] = rec[(h << 2) - 1][((i - 1) << 2) + j];
      line_buffer_output.up[x + i].rec[j] = rec[(h << 2) - 1][(i << 2) + j];
		}
		for (int j = 0; j < REFP_NUM; ++j)
		{
			for (int k = 0; k < MV_D; ++k)
			{
				line_buffer_output.topleft[x + i].mv[j][k] = mv[w * (h - 1) + i - 1][j][k];
        line_buffer_output.up[x + i].mv[j][k] = mv[w * (h - 1) + i][j][k];
			}
			line_buffer_output.topleft[x + i].refi[j] = refi[w * (h - 1) + i - 1][j];
      line_buffer_output.up[x + i].refi[j] = refi[w * (h - 1) + i][j];
		}	
	}
		for (int i = 0; i < 4; ++i)
		{
#pragma HLS pipeline
			line_buffer_output.left[y + i].map_scu = map_scu[w * i + w - 1];
			line_buffer_output.left[y + i].map_cu_mode = map_cu_mode[w * i + w - 1];
			line_buffer_output.left[y + i].ip_map_scu = ip_map_scu[w * i + w - 1];
			line_buffer_output.left[y + i].ipm = ipm[w * i + w - 1];

			for (int j = 0; j < MIN_CU_SIZE; ++j)
			{
				line_buffer_output.left[y + i].rec[j] = rec[(i << 2) + j][(w << 2) - 1];
			}


			for (int j = 0; j < REFP_NUM; ++j)
			{
				for (int k = 0; k < MV_D; ++k)
				{
					line_buffer_output.left[y + i].mv[j][k] = mv[w * i + w - 1][j][k];
				}
				line_buffer_output.left[y + i].refi[j] = refi[w * i + w - 1][j];
			}

		}
}


