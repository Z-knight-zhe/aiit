#pragma once
#include "com_def.h"
#include "enc_def.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "md_utils.h"
#include "line_buffer.h"
#include "cu_data_man.h"
#include "iime.h"

#ifndef CMOD
#define CMOD					1
#endif

#if !CMOD
#ifndef load_core_interface
#define load_core_interface		1
#endif
#ifndef load_interface
#define load_interface			1
#endif
#ifndef pu_interface
#define pu_interface			1
#endif
#ifndef store_core_interface
#define store_core_interface	1
#endif
#ifndef store_interface
#define store_interface			1
#endif
#ifndef vmod_interface
#define vmod_interface			1
#endif
#endif // !CMOD

#ifndef STORE_AXIS_ADAPTER
#define STORE_AXIS_ADAPTER			1
#endif
#ifndef BYPASS_AXIS_ADAPTER
#define BYPASS_AXIS_ADAPTER			0
#endif
#ifndef OUTPUT_AXIS_ADAPTER
#define OUTPUT_AXIS_ADAPTER			0
#endif

#ifndef CHROMA_AXIS_ADAPTER
#define CHROMA_AXIS_ADAPTER			1
#endif

#ifndef PU_16X8_AXIS_ADAPTER
#define PU_16X8_AXIS_ADAPTER        1
#endif
#ifndef PU_8X16_AXIS_ADAPTER
#define PU_8X16_AXIS_ADAPTER        1
#endif

#ifndef LOAD_AXIS_ADAPTER
#define LOAD_AXIS_ADAPTER			0
#endif

#if !CMOD
#include <hls_stream.h>
#define stream_type hls::stream
#define reverse_stream_type hls::stream
#else
#define stream_type Stream
#define reverse_stream_type ReverseStream
#endif
#if __SYNTHESIS__
typedef ap_uint<16> ref_pel;
#else
typedef pel ref_pel;
#endif

#define PR_SIZE							256
#define IR_SIZE							8

#define BYPASS_0						0
#define BYPASS_1						1
#define BYPASS_2						2
#define BYPASS_3						3
#define BYPASS_4						4
#define KERNEL_32X32					5
#define KERNEL_16X32_1					5
#define KERNEL_16X16_1					5
#define KERNEL_8X16_1					5
#define KERNEL_16X8_1					5
#define KERNEL_32X16_1					5
#define KERNEL_16X16_2					5
#define KERNEL_8X16_2					5
#define KERNEL_16X8_2					5
#define KERNEL_16X16_3					5
#define KERNEL_8X16_3					5
#define KERNEL_16X8_3					5
#define KERNEL_8X8_1					5
#define KERNEL_64X64					5
#define KERNEL_32X64					5
#define KERNEL_16X64					5
#define KERNEL_16X32_2					5
#define KERNEL_16X16_4					5
#define KERNEL_8X16_4					5
#define KERNEL_16X8_4					5
#define KERNEL_8X64						5
#define KERNEL_64X32					5
#define KERNEL_64X16					5
#define KERNEL_32X16_2					5
#define KERNEL_16X16_5					5
#define KERNEL_8X16_5					5
#define KERNEL_16X8_5					5
#define KERNEL_64X8						5

#define CHANNEL_0						0
#define CHANNEL_1						1
#define CHANNEL_2						2
#define CHANNEL_3						3
#define CHANNEL_4						4

#define BYPASS							0
#if !ONLY_QT_QT
#define KERNEL_16X64					16
#define KERNEL_64X16					17
#define KERNEL_8X64						18
#define KERNEL_64X8						19
#define KERNEL_16X32					20
#define KERNEL_32X16					21
#endif
#define KERNEL_16X16					6
#define KERNEL_8X16						2
#define KERNEL_16X8						3
#define KERNEL_8X8						4
#if MD_CHANNEL_DEBUG
#define BYPASS_CHANNEL					26
#define OUTPUT_CHANNEL					27
#endif
#define PU_OUTPUT						7

#if MD_CHANNEL_DEBUG
#define LOAD_CHANNEL					32
#define LOAD_1_OUT_CHANNEL				33
#endif
#define LOAD_1_N						34
#if !ONLY_QT_QT
#define LOAD_1_N_16x16					35
#define LOAD_1_N_16x16_MD1				36
#define LOAD_1_N_16x16_MD2				37
#define LOAD_1_N_32x32_32x16_16x32_MD1	38
#define LOAD_1_N_32x32_32x16_16x32_MD2	39
#define LOAD_1_N_16x64_64x16			40
#define LOAD_1_N_64x64_64x32_32x64		41
#endif
#define LOAD_1_BYPASS					42
#define LOAD_1_OUTPUT					43

#define STORE_N_1						48
#if MD_CHANNEL_DEBUG
#define STORE_CHN_1						49
#endif
#if !ONLY_QT_QT
#define STORE_N_1_16x16					50
#define STORE_N_1_16x16_MD1				51
#define STORE_N_1_16x16_MD2				52
#define STORE_N_1_32x32					53
#define STORE_N_1_32x32_32x16_16x32_MD1	54
#define STORE_N_1_32x32_32x16_16x32_MD2	55
#define STORE_N_1_16x64_64x16			56
#define STORE_N_1_64x64_64x32_32x64		57
#endif
#define INIT							58
#define UPDATE							59

#define DONE							64

#define STREAM_DEPTH					2

#define COPY_RMD_TEMP_VAR_MACRO(W, H)		\
	temp_Right_modes = _Right.modes_##W##x##H [i][j];								
#define COPY_RMD_MACRO(W, H)														\
	pu_in_buf[ii].buffer.g_rmd_output.modes_##W##x##H [i][j] = temp_Right_modes;	
#define COPY_ME_TEMP_VAR_MACRO(W, H)														\
	temp_Right_blockMvDir = _Right.block##W##x##H##MvDir [i][j];							
#define COPY_ME_MACRO(W, H)																	\
	pu_in_buf[ii].buffer.me_mv_dir.block##W##x##H##MvDir [i][j] = temp_Right_blockMvDir;	
#define COPY_FME_TEMP_VAR_MACRO(W, H)													\
	temp_Right_sadTreeFmv = _Right.sadTree##W##x##H##Fmv [i][j][k];		
#define COPY_FME_MACRO(W, H)																\
	pu_in_buf[ii].buffer.fme_mv_info.sadTree##W##x##H##Fmv [i][j][k] = temp_Right_sadTreeFmv;	
template <typename _Type>
struct Stream
{
	_Type buf[STREAM_DEPTH];
	int size;

	Stream() : size(0) {}

	void write(_Type data)
	{
#if __SYNTHESIS__
		assert(!full());
#else
		while (full());
#endif
		buf[size++] = data;
	}

	_Type read()
	{
		_Type data;
#if __SYNTHESIS__
		assert(!empty());
		read_nb(data);
#else
		while (read_nb(data));
#endif
		return data;
	}

	bool read_nb(_Type& data)
	{
		bool ret = empty();
		if (!ret)
		{
			data = buf[0];
			for (int i = 0; i < STREAM_DEPTH - 1; ++i)
			{
				buf[i] = buf[i + 1];
			}
			--size;
		}
		return !ret;
	}

	bool empty()
	{
		return size == 0;
	}

	bool full()
	{
		return false;
	}

	~Stream()
	{
		assert(size == 0);
	}
};

template <typename _Type>
inline _Type get(Stream<_Type>& _Right)
{
	return _Right.read();
}

template <typename _Type>
inline void set(Stream<_Type>& _Left, const _Type& _Right)
{
	_Left.write(_Right);
}

template <typename _Type>
struct ReverseStream
{
	stream_type<_Type> sbuf;

	ReverseStream() : sbuf() {}

	bool full()
	{
		return sbuf.empty();
	}

	void write(_Type data)
	{
		sbuf.read();
	}
};

inline ap_uint<5> enc_32_5(ap_uint<32> src)
{
	ap_uint<5> dst;
	dst(0, 0) = (src & 0xaaaaaaaa) ? 1 : 0;
	dst(1, 1) = (src & 0xcccccccc) ? 1 : 0;
	dst(2, 2) = (src & 0xf0f0f0f0) ? 1 : 0;
	dst(3, 3) = (src & 0xff00ff00) ? 1 : 0;
	dst(4, 4) = (src & 0xffff0000) ? 1 : 0;
	return dst;
}

struct Instruction
{
	ap_uint<256> buf;

	typedef ap_range_ref<256, false> InsnRef;

	Instruction() { buf=0; }

	InsnRef x0						() const { return buf( 12,   0); }
	InsnRef y0						() const { return buf( 26,  14); }
	InsnRef cu_width_log2			() const { return buf( 30,  28); }
	InsnRef cu_height_log2			() const { return buf( 33,  31); }
	InsnRef cup						() const { return buf( 45,  34); }
	InsnRef cud						() const { return buf( 61,  58); }
	InsnRef qtd						() const { return buf( 64,  62); }
	InsnRef btd						() const { return buf( 68,  66); }
	InsnRef tree_status				() const { return buf( 71,  70); }
	InsnRef pred_mode				() const { return buf( 73,  72); }
	InsnRef split_mode				() const { return buf( 76,  74); }
	InsnRef part_num				() const { return buf( 79,  77); }
	InsnRef prev_up_log2_sub_cuw	() const { return buf( 82,  80); }
	InsnRef x_pos					() const { return buf( 96,  83); }
	InsnRef y_pos					() const { return buf(110,  97); }
	InsnRef flag_pu					() const { return buf(167, 128); }
	InsnRef flag_chn				() const { return buf(199, 168); }
	InsnRef tag_pu					() const { return buf(207, 200); }
	InsnRef tag_chn					() const { return buf(215, 208); }
	InsnRef mem_ptr					() const { return buf(223, 216); }
	InsnRef version					() const { return buf(247, 244); }
	InsnRef opcode					() const { return buf(255, 248); }
	InsnRef flag_pu(U8  pu_idx) const { assert(pu_idx < PU_NUM); return buf(128 + pu_idx, 128 + pu_idx); }
	InsnRef flag_chn(U8 chn_idx) const { assert(chn_idx < CHN_NUM); return buf(168 + chn_idx, 168 + chn_idx); }

	Instruction& operator=(const Instruction& _Right) { buf = _Right.buf; return *this; }
	Instruction& operator=(const ap_uint<256>& _Right) { buf = _Right; return *this; }
};

struct Register
{
public:
	ap_uint<128> _sys;
	ap_uint<1024> _cond0, _cond1;
	ap_uint<1024> _cfg0, _cfg1;

	typedef ap_range_ref<128, false> SysRef;
	typedef ap_range_ref<1024, false> RegRef;

public:
	Register() : _sys(0), _cond0(0), _cond1(0), _cfg0(0), _cfg1(0) {}

	SysRef inst_addr	()			const { return _sys(15, 0); }
	SysRef load_addr	()			const { return _sys(15, 0); }
	SysRef pu_addr		()			const { return _sys(31, 16); }
	SysRef store_addr	()			const { return _sys(47, 32); }
	SysRef boot			()			const { return _sys(63, 63); }
	SysRef x			()			const { return _sys(79, 64); }
	SysRef y			()			const { return _sys(95, 80); }

	RegRef pu			()			const { return _cond0(1023, 0); }
	RegRef load			()			const { return _cond1(511, 0); }
	RegRef store		()			const { return _cond1(1023, 512); }
	RegRef pu			(U10 idx)	const { return _cond0(idx, idx); }
	RegRef load			(U10 idx)	const { return _cond1(idx, idx); }
	RegRef store		(U10 idx)	const { return _cond1(512 + idx, 512 + idx); }

	RegRef cfg0			()			const { return _cfg0(1023, 0); }
	RegRef cfg1			()			const { return _cfg1(1023, 0); }
	RegRef cfg_8		(U8 idx)	const { return cfg(8, idx); }
	RegRef cfg_16		(U8 idx)	const { return cfg(16, idx); }
	RegRef cfg_32		(U8 idx)	const { return cfg(32, idx); }
	RegRef cfg_64		(U8 idx)	const { return cfg(64, idx); }

private:
	RegRef cfg(U8 bit_log2, U8 idx) const
	{
		U11 min = idx << bit_log2;
		U11 max = idx + (1 << bit_log2) - 1;

		if (max < 1024)
		{
			return _cfg0(max, min);
		}
		else if (min >= 1024)
		{
			return _cfg1(max - 1024, min - 1024);
		}
		else
		{
			assert(false);
		}
	}
};

struct InstructionPU PUBLIC_C DeleteOperator
{
	U8 pu_idx;
	U13 x0;
	U13 y0;
	U12 cup;
	U3 cu_width_log2;
	U3 cu_height_log2;
	U4 cud;
	U3 qt_depth;
	U3 bet_depth;
	U2 cons_pred_mode;
	U8 cu_width;
	U8 cu_height;
	U2 tree_status;
	U3 split_mode;
	U8 tag;

	friend void copy(InstructionPU& _Left, const InstructionPU& _Right)
	{
		_Left.pu_idx = _Right.pu_idx;
		_Left.x0 = _Right.x0;
		_Left.y0 = _Right.y0;
		_Left.cup = _Right.cup;
		_Left.cu_width_log2 = _Right.cu_width_log2;
		_Left.cu_height_log2 = _Right.cu_height_log2;
		_Left.cud = _Right.cud;
		_Left.qt_depth = _Right.qt_depth;
		_Left.bet_depth = _Right.bet_depth;
		_Left.cons_pred_mode = _Right.cons_pred_mode;
		_Left.cu_width = _Right.cu_width;
		_Left.cu_height = _Right.cu_height;
		_Left.tree_status = _Right.tree_status;
		_Left.split_mode = _Right.split_mode;
		_Left.tag = _Right.tag;
	}
};

struct InstructionLoad
{
	U8 type;
	U3 part_num;
	U8 pu_num;
	U8 pool_idx;
	U8 w;
	U8 h;
	U13 x0;
	U13 y0;
	U4 version;
};

struct InstructionStore
{
	U8 type;
	U3 cu_width_log2;
	U3 cu_height_log2;
	U4 cud;
	U2 tree_status;
	U13 x0;
	U13 y0;
	U8 cu_width;
	U8 cu_height;
	U3 part_num;
	U12 cup;
	U2 cons_pred_mode;
	U2 cons_pred_mode_child[4];
	U2 tree_status_child[4];
	U2 split_mode[4];
	s64 cost_temp[4];
	U3 best_split_mode;
	U2 best_cons_pred_mode;
	U2 best_tree_status_child;
	U14      x_pos;
	U14      y_pos;
	U3 prev_up_log2_sub_cuw;
	U8 pool_idx;
	U1 flag_pu[PU_NUM];
	U8 status;
	U3 num_split_to_try;
	U8 mem_ptr;
};

inline ap_uint<5> cnt_32_5(ap_uint<32> src)
{
	return src(0, 0) + src(1, 1) + src(2, 2) + src(3, 3) + src(4, 4) + src(5, 5) + src(6, 6) + src(7, 7) 
		+ src(8, 8) + src(9, 9) + src(10, 10) + src(11, 11) + src(12, 12) + src(13, 13) + src(14, 14) + src(15, 15) 
		+ src(16, 16) + src(17, 17) + src(18, 18) + src(19, 19) + src(20, 20) + src(21, 21) + src(22, 22) + src(23, 23) 
		+ src(24, 24) + src(25, 25) + src(26, 26) + src(27, 27) + src(28, 28) + src(29, 29) + src(30, 30) + src(31, 31);
}

inline void insn_decode(Instruction& src, InstructionLoad& dst, const Register& reg)
{
	dst.type = src.opcode();
	dst.part_num = src.part_num();
	dst.pool_idx = enc_32_5(src.flag_chn());
	dst.pu_num = cnt_32_5(src.flag_pu());
	dst.w = 1 << src.cu_width_log2();
	dst.h = 1 << src.cu_height_log2();
	dst.x0 = src.x0() + reg.x();
	dst.y0 = src.y0() + reg.y();
	dst.version = src.version();
}

inline void insn_decode(Instruction& src, InstructionStore& dst, const Register& reg)
{
	dst.type = src.opcode();
	dst.cons_pred_mode = src.pred_mode();
	dst.cud = src.cud();
	dst.cup = src.cup();
	dst.cu_height = 1 << src.cu_height_log2();
	dst.cu_height_log2 = src.cu_height_log2();
	dst.cu_width = 1 << src.cu_width_log2();
	dst.cu_width_log2 = src.cu_width_log2();
	for (int i = 0; i < PU_NUM; ++i)
	{
		dst.flag_pu[i] = src.flag_pu(i);
	}
	dst.part_num = src.part_num();
	dst.pool_idx = enc_32_5(src.flag_chn());
	dst.prev_up_log2_sub_cuw = src.prev_up_log2_sub_cuw();
	dst.tree_status = src.tree_status();
	dst.x0 = src.x0() + reg.x();
	dst.x_pos = src.x_pos();
	dst.y0 = src.y0() + reg.y();
	dst.y_pos = src.y_pos();
	dst.status = src.tag_chn();
	dst.mem_ptr = src.mem_ptr();
}

inline void insn_decode(const Instruction& src, InstructionPU& dst, const Register& reg)
{
	dst.bet_depth = src.btd();
	dst.cons_pred_mode = src.pred_mode();
	dst.cud = src.cud();
	dst.cup = src.cup();
	dst.cu_height = 1 << src.cu_height_log2();
	dst.cu_height_log2 = src.cu_height_log2();
	dst.cu_width = 1 << src.cu_width_log2();
	dst.cu_width_log2 = src.cu_width_log2();
	dst.pu_idx = src.opcode();
	dst.qt_depth = src.qtd();
	dst.split_mode = src.split_mode();
	dst.tree_status = src.tree_status();
	dst.x0 = src.x0() + reg.x();
	dst.y0 = src.y0() + reg.y();
	dst.tag = src.tag_pu();
}

struct PRMan
{
	U8 idx;

	void init()
	{
		idx = 0;
	}

	Instruction get(Instruction mem[PR_SIZE])
	{
		return mem[idx++];
	}
};

struct Arbitor
{
#define FLAT_8 \
    FLAT(0) \
    FLAT(1) \
    FLAT(2) \
    FLAT(3) \
    FLAT(4) \
    FLAT(5) \
    FLAT(6) \
    FLAT(7)

#define FLAT_8_2 \
    FLAT(0, 0) \
    FLAT(0, 1) \
    FLAT(1, 0) \
    FLAT(1, 1) \
    FLAT(2, 0) \
    FLAT(2, 1) \
    FLAT(3, 0) \
    FLAT(3, 1) \
    FLAT(4, 0) \
    FLAT(4, 1) \
    FLAT(5, 0) \
    FLAT(5, 1) \
    FLAT(6, 0) \
    FLAT(6, 1) \
    FLAT(7, 0) \
    FLAT(7, 1)

#define FLAT(i, j) ref_pel ref_##i##_##j [2560][16]; 
	FLAT_8_2
#undef FLAT

	Arbitor& operator=(pel refwin_mc[2560][8][2][16])
	{
		for (int i = 0; i < 2560; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{
#define FLAT(m, n) ref_##m##_##n [i][j] = refwin_mc[i][m][n][j];
				FLAT_8_2
#undef FLAT
			}
		}
		return *this;
	}
};

struct Channel
{
	LineBuffer line_buffer;
	CuDataPtr cu_data_ptr;

	ENC_ME_LINE_MAP_SIZE<32> me_line_map_ptr_32;
	ENC_ME_LINE_MAP_SIZE<64> me_line_map_ptr_64;
	ENC_ME_NEB_INFO	up_nebs[10];
	ENC_ME_NEB_INFO	left_nebs[8];
	ENC_ME_NEB_INFO up_nebs_32x32[18];
	ENC_ME_NEB_INFO left_nebs_32x32[16];
	ENC_ME_NEB_INFO up_nebs_32[10];
	ENC_ME_NEB_INFO up_nebs_64[1 + (64 >> 2) + 1];
	ENC_ME_NEB_INFO left_nebs_32[8];
	ENC_ME_NEB_INFO left_nebs_64[(64 >> 2)];
	ENC_CU_DATA_ARRAY_SIZE<32> prev_cu_data_ptr;
	ENC_CU_DATA prev_cu_data_ptr_32x32;
	ENC_CU_DATA_ARRAY prev_up_cu_data_temp_array_ptr;
	IP_ARRAY_SIZE<32, 32> IP_buffer_ptr_32x32;
	IP_ARRAY_SIZE<32, 64> IP_buffer_ptr_32x64;
	IP_ARRAY_SIZE<64, 32> IP_buffer_ptr_64x32;
	IP_ARRAY_SIZE<64, 64> IP_buffer_ptr_64x64;
	MPM_ARRAY_SIZE<32, 32> MPM_buffer_ptr_32x32;
	MPM_ARRAY_SIZE<32, 64> MPM_buffer_ptr_32x64;
	MPM_ARRAY_SIZE<64, 32> MPM_buffer_ptr_64x32;
	MPM_ARRAY_SIZE<64, 64> MPM_buffer_ptr_64x64;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_ptr_32x32;
	IP_MAP_SCU_SIZE<32, 64> IP_map_scu_ptr_32x64;
	IP_MAP_SCU_SIZE<64, 32> IP_map_scu_ptr_64x32;
	IP_MAP_SCU_SIZE<64, 64> IP_map_scu_ptr_64x64;
	IP_ARRAY_SIZE<16, 32> IP_buffer_ptr_16x32;
	MPM_ARRAY_SIZE<16, 32> MPM_buffer_ptr_16x32;
	IP_MAP_SCU_SIZE<16, 32> IP_map_scu_ptr_16x32;
	IP_ARRAY_SIZE<32, 16> IP_buffer_ptr_32x16;
	MPM_ARRAY_SIZE<32, 16> MPM_buffer_ptr_32x16;
	IP_MAP_SCU_SIZE<32, 16> IP_map_scu_ptr_32x16;
	ENC_CU_DATA_ARRAY curr_cu_data_best;
	IP_ARRAY_SIZE<32, 64>	IP_buffer_32x64;
	IP_ARRAY_SIZE<64, 32>	IP_buffer_64x32;
	IP_MAP_SCU_SIZE<32, 64>	IP_map_scu_32x64;
	IP_MAP_SCU_SIZE<64, 32>	IP_map_scu_64x32;
	MPM_ARRAY_SIZE<32, 64>	MPM_buffer_32x64;
	MPM_ARRAY_SIZE<64, 32>	MPM_buffer_64x32;

	s64 cost_best[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	MPM_ARRAY_SIZE<16, 16> MPM_buffer_16x16;
	MPM_ARRAY_SIZE<16, 64> MPM_buffer_16x64;
	MPM_ARRAY_SIZE<64, 16> MPM_buffer_64x16;
	IP_ARRAY_SIZE<16, 16> IP_buffer_16x16;
	IP_ARRAY_SIZE<16, 64> IP_buffer_16x64;
	IP_ARRAY_SIZE<64, 16> IP_buffer_64x16;
	IP_MAP_SCU_SIZE<16, 16> IP_map_scu_16x16;
	ENC_ME_LINE_MAP_SIZE<16> me_line_map_16;

	MPM_ARRAY_SIZE<32, 32> MPM_buffer_32x32;
	IP_ARRAY_SIZE<32, 32> IP_buffer_32x32;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_32x32;
	ENC_ME_LINE_MAP_SIZE<32> me_line_map_32;
	ENC_ME_LINE_MAP_SIZE<64> me_line_map_64;

	ENC_ME_NEB_INFO				nebs_line_ver_32[8];
	ENC_ME_NEB_INFO				nebs_line_hor_32[8];	
	ENC_ME_NEB_INFO				nebs_line_ver_32x32[8];
	ENC_ME_NEB_INFO				nebs_line_hor_32x32[8+8];
	ENC_ME_NEB_INFO				nebs_line_ver[4];
	ENC_ME_NEB_INFO nebs_line_ver_64[16];
	ENC_ME_NEB_INFO				nebs_line_hor[4 + 4];
	ENC_ME_NEB_INFO nebs_line_hor_64[16];

	int split_mode_father;
	U1 boundary;
	U3 best_split_mode;
	U2 best_cons_pred_mode;

};

struct ShareMemory
{
	Channel channel[CHN_NUM];
};

struct PUAxisInPorts_16x16{
	u8 in_buf_buffer_skip_in_slice_type;
	int in_buf_buffer_skip_in_ptr;
	u8 in_buf_buffer_skip_in_umve_enable_flag;
	int in_buf_buffer_Fetch_Ref_window_ptr_code_stru;
	U8 in_buf_line_buffer_y0;
	int in_buf_config_md_fw_ptr_num_refp_0;
	int in_buf_config_md_fw_ptr_num_refp_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_0_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_1_1;
#if ENABLE_BFRAME
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_1;
#endif // ENABLE_BFRAME		
};

#if PU_16X8_AXIS_ADAPTER
struct PUAxisInPorts_16x8{
	U13 in_buf_config_md_fw_ptr_pic_width               ; 
	U13 in_buf_config_md_fw_ptr_pic_height              ; 
	U2  in_buf_config_md_fw_ptr_slice_type              ; 
	U11 in_buf_config_md_fw_ptr_pic_width_in_scu        ; 
	U11 in_buf_config_md_fw_ptr_pic_height_in_scu       ; 
	U3  in_buf_config_md_fw_ptr_log2_culine             ; 
	U4  in_buf_config_md_fw_ptr_bit_depth_internal      ; 
	U4  in_buf_config_md_fw_ptr_max_part_ratio          ; 
	U3  in_buf_config_md_fw_ptr_min_cu_size             ; 
	U3  in_buf_config_md_fw_ptr_max_split_times         ; 
	U4  in_buf_config_md_fw_ptr_min_qt_size             ; 
	U7  in_buf_config_md_fw_ptr_max_bt_size             ; 
	U7  in_buf_config_md_fw_ptr_max_eqt_size            ; 
	int in_buf_config_md_fw_ptr_mipf_enable_flag        ; 
	U9  in_buf_config_md_fw_ptr_cur_ptr                 ; 
	U13 in_buf_config_md_input_ptr_pix_x                ; 
	U13 in_buf_config_md_input_ptr_pix_y                ; 
	s64 in_buf_config_md_input_ptr_lambda_y             ; 
	U6  in_buf_config_md_input_ptr_qp_y                 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_1      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_1      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_0_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_1_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 ; 
	int in_buf_config_md_input_ptr_cu_dqp_enable        ; 
	int in_buf_config_md_input_ptr_cu_qp_group_pred_qp  ;
	U11 in_buf_config_min_scu_x                         ; 
	U11 in_buf_config_min_scu_y                         ; 
	U8  in_buf_buffer_skip_in_slice_type                ; 
	int in_buf_buffer_skip_in_ptr                       ; 
	U8  in_buf_buffer_skip_in_umve_enable_flag          ; 
	int in_buf_buffer_Fetch_Ref_window_ptr_code_stru    ; 
	U13 in_buf_insn_x0                                  ; 
	U13 in_buf_insn_y0                                  ; 
	U12 in_buf_insn_cup                                 ; 
	U3  in_buf_insn_cu_width_log2                       ; 
	U4  in_buf_insn_cud                                 ; 
	U3  in_buf_insn_qt_depth                            ; 
	U3  in_buf_insn_bet_depth                           ; 
	U2  in_buf_insn_cons_pred_mode                      ; 
	U8  in_buf_insn_cu_width                            ; 
	U8  in_buf_insn_cu_height                           ; 
	U2  in_buf_insn_tree_status                         ; 
	U3  in_buf_insn_split_mode                          ; 
	U8  in_buf_insn_tag                                 ; 
	U8  in_buf_line_buffer_y0                           ; 
	int in_buf_config_md_fw_ptr_refp_ptr[17][2]         ;
    U5  in_buf_config_md_fw_ptr_num_refp[2]             ;
};

struct PURamOutPorts_16x8{
	U8  out_buf_line_buffer_y0[1]                       ; 
	s64 out_buf_cost_temp[1]                            ; 
	U2  out_buf_cons_pred_mode[1]                       ; 
	U2  out_buf_tree_status_child[1]                    ; 
	U3  out_buf_split_mode[1]                           ; 
	U8  out_buf_tag[1]                                  ; 
};
#endif // PU_16X8_AXIS_ADAPTER

#if PU_8X16_AXIS_ADAPTER
struct PUAxisInPorts_8x16{
	U13 in_buf_config_md_fw_ptr_pic_width               ; 
	U13 in_buf_config_md_fw_ptr_pic_height              ; 
	U2  in_buf_config_md_fw_ptr_slice_type              ; 
	U11 in_buf_config_md_fw_ptr_pic_width_in_scu        ; 
	U11 in_buf_config_md_fw_ptr_pic_height_in_scu       ; 
	U3  in_buf_config_md_fw_ptr_log2_culine             ; 
	U4  in_buf_config_md_fw_ptr_bit_depth_internal      ; 
	U4  in_buf_config_md_fw_ptr_max_part_ratio          ; 
	U3  in_buf_config_md_fw_ptr_min_cu_size             ; 
	U3  in_buf_config_md_fw_ptr_max_split_times         ; 
	U4  in_buf_config_md_fw_ptr_min_qt_size             ; 
	U7  in_buf_config_md_fw_ptr_max_bt_size             ; 
	U7  in_buf_config_md_fw_ptr_max_eqt_size            ; 
	int in_buf_config_md_fw_ptr_mipf_enable_flag        ; 
	U9  in_buf_config_md_fw_ptr_cur_ptr                 ; 
	U13 in_buf_config_md_input_ptr_pix_x                ; 
	U13 in_buf_config_md_input_ptr_pix_y                ; 
	s64 in_buf_config_md_input_ptr_lambda_y             ; 
	U6  in_buf_config_md_input_ptr_qp_y                 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_1      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_1      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_0_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_1_0      ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0 ; 
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0 ; 
	int in_buf_config_md_input_ptr_cu_dqp_enable        ; 
	int in_buf_config_md_input_ptr_cu_qp_group_pred_qp  ;
	U11 in_buf_config_min_scu_x                         ; 
	U11 in_buf_config_min_scu_y                         ; 
	U8  in_buf_buffer_skip_in_slice_type                ; 
	int in_buf_buffer_skip_in_ptr                       ; 
	U8  in_buf_buffer_skip_in_umve_enable_flag          ; 
	int in_buf_buffer_Fetch_Ref_window_ptr_code_stru    ; 
	U13 in_buf_insn_x0                                  ; 
	U13 in_buf_insn_y0                                  ; 
	U12 in_buf_insn_cup                                 ; 
	U3  in_buf_insn_cu_width_log2                       ; 
	U4  in_buf_insn_cud                                 ; 
	U3  in_buf_insn_qt_depth                            ; 
	U3  in_buf_insn_bet_depth                           ; 
	U2  in_buf_insn_cons_pred_mode                      ; 
	U8  in_buf_insn_cu_width                            ; 
	U8  in_buf_insn_cu_height                           ; 
	U2  in_buf_insn_tree_status                         ; 
	U3  in_buf_insn_split_mode                          ; 
	U8  in_buf_insn_tag                                 ; 
	U8  in_buf_line_buffer_y0                           ; 
	int in_buf_config_md_fw_ptr_refp_ptr[17][2]         ;
    U5  in_buf_config_md_fw_ptr_num_refp[2]             ;
};

struct PURamOutPorts_8x16{
	U8  out_buf_line_buffer_y0[1]                       ; 
	s64 out_buf_cost_temp[1]                            ; 
	U2  out_buf_cons_pred_mode[1]                       ; 
	U2  out_buf_tree_status_child[1]                    ; 
	U3  out_buf_split_mode[1]                           ; 
	U8  out_buf_tag[1]                                  ; 
};
#endif // PU_8X16_AXIS_ADAPTER

struct PUAxisInPorts_8x8{
	U13 in_buf_config_md_fw_ptr_pic_width;
	U13 in_buf_config_md_fw_ptr_pic_height;
	U2  in_buf_config_md_fw_ptr_slice_type;
	U11  in_buf_config_md_fw_ptr_pic_width_in_scu;
	U11  in_buf_config_md_fw_ptr_pic_height_in_scu;
	U4   in_buf_config_md_fw_ptr_bit_depth_internal;
	U4   in_buf_config_md_fw_ptr_max_part_ratio;
	U3   in_buf_config_md_fw_ptr_min_cu_size;
	U3   in_buf_config_md_fw_ptr_max_split_times;
	U4   in_buf_config_md_fw_ptr_min_qt_size;
	U7   in_buf_config_md_fw_ptr_max_bt_size;
	U7   in_buf_config_md_fw_ptr_max_eqt_size;
	int  in_buf_config_md_fw_ptr_mipf_enable_flag;
	int in_buf_config_md_fw_ptr_refp_ptr_0_0;
	int in_buf_config_md_fw_ptr_refp_ptr_0_1;
	int in_buf_config_md_fw_ptr_refp_ptr_1_0;
	int in_buf_config_md_fw_ptr_refp_ptr_1_1;
	int in_buf_config_md_fw_ptr_refp_ptr_2_0;
	int in_buf_config_md_fw_ptr_refp_ptr_2_1;
	int in_buf_config_md_fw_ptr_refp_ptr_3_0;
	int in_buf_config_md_fw_ptr_refp_ptr_3_1;
	int in_buf_config_md_fw_ptr_refp_ptr_4_0;
	int in_buf_config_md_fw_ptr_refp_ptr_4_1;
	int in_buf_config_md_fw_ptr_refp_ptr_5_0;
	int in_buf_config_md_fw_ptr_refp_ptr_5_1;
	int in_buf_config_md_fw_ptr_refp_ptr_6_0;
	int in_buf_config_md_fw_ptr_refp_ptr_6_1;
	int in_buf_config_md_fw_ptr_refp_ptr_7_0;
	int in_buf_config_md_fw_ptr_refp_ptr_7_1;
	int in_buf_config_md_fw_ptr_refp_ptr_8_0;
	int in_buf_config_md_fw_ptr_refp_ptr_8_1;
	int in_buf_config_md_fw_ptr_refp_ptr_9_0;
	int in_buf_config_md_fw_ptr_refp_ptr_9_1;
	int in_buf_config_md_fw_ptr_refp_ptr_10_0;
	int in_buf_config_md_fw_ptr_refp_ptr_10_1;
	int in_buf_config_md_fw_ptr_refp_ptr_11_0;
	int in_buf_config_md_fw_ptr_refp_ptr_11_1;
	int in_buf_config_md_fw_ptr_refp_ptr_12_0;
	int in_buf_config_md_fw_ptr_refp_ptr_12_1;
	int in_buf_config_md_fw_ptr_refp_ptr_13_0;
	int in_buf_config_md_fw_ptr_refp_ptr_13_1;
	int in_buf_config_md_fw_ptr_refp_ptr_14_0;
	int in_buf_config_md_fw_ptr_refp_ptr_14_1;
	int in_buf_config_md_fw_ptr_refp_ptr_15_0;
	int in_buf_config_md_fw_ptr_refp_ptr_15_1;
	int in_buf_config_md_fw_ptr_refp_ptr_16_0;
	int in_buf_config_md_fw_ptr_refp_ptr_16_1;
	U5 in_buf_config_md_fw_ptr_num_refp_0;
	U5 in_buf_config_md_fw_ptr_num_refp_1;
    U9 in_buf_config_md_fw_ptr_cur_ptr;
	U13 in_buf_config_md_input_ptr_pix_x;
	U13 in_buf_config_md_input_ptr_pix_y;
	s64 in_buf_config_md_input_ptr_lambda_y;
	U6  in_buf_config_md_input_ptr_qp_y;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_0_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_0_1_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_1_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_0_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_0_1_1;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_0_0;
	S14 in_buf_config_md_input_ptr_CtrPos2MD_ref1_1_1_0;
	int in_buf_config_md_input_ptr_cu_dqp_enable;
	int in_buf_config_md_input_ptr_cu_qp_group_pred_qp;
	U11	in_buf_config_min_scu_x;
	U11	in_buf_config_min_scu_y;
	u8  in_buf_buffer_skip_in_slice_type;
	int in_buf_buffer_skip_in_ptr;
	u8  in_buf_buffer_skip_in_umve_enable_flag;
	int in_buf_buffer_Fetch_Ref_window_ptr_code_stru;
	U13 in_buf_insn_x0;
	U13 in_buf_insn_y0;
	U3 in_buf_insn_cu_width_log2;
	U4 in_buf_insn_cud;
	U3 in_buf_insn_qt_depth;
	U3 in_buf_insn_bet_depth;
	U8 in_buf_insn_cu_width;
	U8 in_buf_insn_cu_height;
	U2 in_buf_insn_tree_status;
	U3 in_buf_insn_split_mode;
	U8 in_buf_insn_tag;
	U8 in_buf_line_buffer_y0;
	U2 out_buf_cons_pred_mode;
} ;//PUAxisInPorts_8x8

struct PURamOut_8x8{
	U8 out_buf_line_buffer_y0[1];
	s64 out_buf_cost_temp[1];
	U2 out_buf_tree_status_child[1];
	U3 out_buf_split_mode[1];
	U8 out_buf_tag[1];
}; //PURamOut_8x8


struct Load2PU
{
	LCUConfig config;
	LCUInput buffer;
	InstructionPU insn;
	s64 cost_best;

	LineBuffer line_buffer;

	ENC_ME_LINE_MAP_SIZE<32> me_line_map_32;
	ENC_ME_LINE_MAP_SIZE<16> me_line_map_16;
	ENC_ME_LINE_MAP_SIZE<64> me_line_map_64;
	ENC_ME_NEB_INFO up_nebs_32[10];
	ENC_ME_NEB_INFO left_nebs_32[8];	
	ENC_ME_NEB_INFO up_nebs[6];
	ENC_ME_NEB_INFO up_nebs_64[1 + (64 >> 2) + 1];
	ENC_ME_NEB_INFO left_nebs[4];
	ENC_ME_NEB_INFO left_nebs_64[(64 >> 2)];
	IP_ARRAY_SIZE<16, 32> IP_buffer_16x32;
	MPM_ARRAY_SIZE<16, 32> MPM_buffer_16x32;
	IP_MAP_SCU_SIZE<16, 32> IP_map_scu_16x32;
	IP_ARRAY_SIZE<32, 16> IP_buffer_32x16;
	MPM_ARRAY_SIZE<32, 16> MPM_buffer_32x16;
	IP_MAP_SCU_SIZE<32, 16> IP_map_scu_32x16;	
	IP_ARRAY_SIZE<16, 16> IP_buffer_16x16;
	IP_ARRAY_SIZE<16, 64> IP_buffer_16x64;
	IP_ARRAY_SIZE<64, 16> IP_buffer_64x16;
	MPM_ARRAY_SIZE<16, 16> MPM_buffer_16x16;
	MPM_ARRAY_SIZE<16, 64> MPM_buffer_16x64;
	MPM_ARRAY_SIZE<64, 16> MPM_buffer_64x16;
	IP_MAP_SCU_SIZE<16, 16> IP_map_scu_16x16;
	IP_MAP_SCU_SIZE<16, 64> IP_map_scu_16x64;
	IP_MAP_SCU_SIZE<64, 16> IP_map_scu_64x16;

	IP_ARRAY_SIZE<32, 64>	IP_buffer_32x64;
	IP_ARRAY_SIZE<64, 32>	IP_buffer_64x32;
	IP_MAP_SCU_SIZE<32, 64>	IP_map_scu_32x64;
	IP_MAP_SCU_SIZE<64, 32>	IP_map_scu_64x32;
	MPM_ARRAY_SIZE<32, 64>	MPM_buffer_32x64;
	MPM_ARRAY_SIZE<64, 32>	MPM_buffer_64x32; 

	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_32x32;
	IP_ARRAY_SIZE<32, 32> IP_buffer_32x32;
	MPM_ARRAY_SIZE<32, 32> MPM_buffer_32x32;

#if USE_SPEED_LEVEL
	u8 ip_list[IPD_RDO_CNT + EXTRA_RDO_MODE];

#else
	u8 ip_list[IPD_RDO_CNT];
#endif

#if ENABLE_BFRAME
   	ME_MV_DIR_16 me_mv_dir_16;
#endif
   	pel org_y_16[16][16];
   	ENC_FME_INFO_ALL_16 fme_mv_info_16;
	strFetch_ref_window_16 Fetch_Ref_window_16;
   	SKIP_MERGE_INPUT_16 skip_in_16;
	U1 boundary;
	U3 part_num;
	int split_mode_father;
};

#if BYPASS_AXIS_ADAPTER
struct Load2Bp_AXIS
{
	U13 in_buf_insn_x0;
	U13 in_buf_insn_y0;
	U3 in_buf_insn_cu_width_log2;
	U3 in_buf_insn_cu_height_log2;
	U3 in_buf_insn_qt_depth;
	U3 in_buf_insn_bet_depth;
	U8 in_buf_insn_cu_width;
	U8 in_buf_insn_cu_height;
	U3 in_buf_insn_split_mode;
	U8 in_buf_insn_tag;
	U13 in_buf_config_md_fw_ptr_pic_width;
	U13 in_buf_config_md_fw_ptr_pic_height;
	U4 in_buf_config_md_fw_ptr_max_part_ratio;
	U3 in_buf_config_md_fw_ptr_min_cu_size;
	U3 in_buf_config_md_fw_ptr_max_split_times;
	U4 in_buf_config_md_fw_ptr_min_qt_size;
	U7 in_buf_config_md_fw_ptr_max_bt_size;
	U7 in_buf_config_md_fw_ptr_max_eqt_size;
	s64 in_buf_config_md_input_ptr_lambda_y;
	U8 in_buf_line_buffer_y0;
};

struct PU2Store_RAM
{
	U8 out_buf_line_buffer_y0[1];
	s64 out_buf_cost_temp[1];
	U3 out_buf_split_mode[1];
	U8 out_buf_tag[1];
};

#endif // BYPASS_AXIS_ADAPTER

struct Load2Bp
{
	s64 cost_best[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	InstructionPU insn;
	LCUConfig config;
	CuDataPtr cu_data_ptr;
	U4 version;
	LineBuffer line_buffer;

	ENC_CU_DATA_ARRAY_SIZE<32> cu_data_temp_ptr;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_ptr_32x32;
};

struct Load2Output
{
	ENC_CU_DATA cu_data;
	LineBuffer line_buffer;
};

#if MD_CHANNEL_DEBUG
struct Load2BpChn
{
	U8 w, h;
	LineBuffer line_buffer;
	ENC_ME_NEB_INFO up_nebs_32[10];
	ENC_ME_NEB_INFO up_nebs_64[1 + (64 >> 2) + 1];
	ENC_ME_NEB_INFO left_nebs_32[8];
	ENC_ME_NEB_INFO left_nebs_64[(64 >> 2)];
	ENC_ME_LINE_MAP_SIZE<32> me_line_map_ptr_32;
	ENC_ME_LINE_MAP_SIZE<64> me_line_map_ptr_64;
	IP_ARRAY_SIZE<32, 32> IP_buffer_ptr_32x32;
	IP_ARRAY_SIZE<32, 64> IP_buffer_ptr_32x64;
	IP_ARRAY_SIZE<64, 32> IP_buffer_ptr_64x32;
	IP_ARRAY_SIZE<64, 64> IP_buffer_ptr_64x64;
	MPM_ARRAY_SIZE<32, 32> MPM_buffer_ptr_32x32;
	MPM_ARRAY_SIZE<32, 64> MPM_buffer_ptr_32x64;
	MPM_ARRAY_SIZE<64, 32> MPM_buffer_ptr_64x32;
	MPM_ARRAY_SIZE<64, 64> MPM_buffer_ptr_64x64;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_ptr_32x32;
	IP_MAP_SCU_SIZE<32, 64> IP_map_scu_ptr_32x64;
	IP_MAP_SCU_SIZE<64, 32> IP_map_scu_ptr_64x32;
	IP_MAP_SCU_SIZE<64, 64> IP_map_scu_ptr_64x64;
	ENC_CU_DATA prev_cu_data_ptr_32x32;
	ENC_ME_NEB_INFO				nebs_line_ver_32x32[8];
	ENC_ME_NEB_INFO				nebs_line_hor_32x32[8 + 8];
	ENC_ME_NEB_INFO up_nebs_32x32[18];
	ENC_ME_NEB_INFO left_nebs_32x32[16];
};

struct Load2OutChn
{
	LineBuffer line_buffer;
	U8 w, h;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_ptr_32x32;
	IP_MAP_SCU_SIZE<32, 64> IP_map_scu_ptr_32x64;
	IP_MAP_SCU_SIZE<64, 32> IP_map_scu_ptr_64x32;
	IP_MAP_SCU_SIZE<64, 64> IP_map_scu_ptr_64x64;
	ENC_CU_DATA_ARRAY_SIZE<32> prev_cu_data_ptr;
	ENC_CU_DATA_ARRAY prev_up_cu_data_temp_array_ptr;
	s64 cost_best[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN];
	ENC_ME_LINE_MAP_SIZE<32> me_line_map_ptr_32;
	IP_ARRAY_SIZE<32, 32> IP_buffer_ptr_32x32;
	MPM_ARRAY_SIZE<32, 32> MPM_buffer_ptr_32x32;
	ENC_ME_NEB_INFO				nebs_line_ver_32x32[8];
	ENC_ME_NEB_INFO				nebs_line_hor_32x32[8 + 8];
};
#endif

#if STORE_AXIS_ADAPTER
struct Store_RAM
{
	U8 line_buffer_y0[1];
	s64 cost_temp[1];
#if CUDQP_QP_MAP
	int cu_data_temp_cu_qp_group_pred_qp[1];
	int curr_cu_data_temp_cu_qp_group_pred_qp[1];
	int curr_cu_data_temp_64_cu_qp_group_pred_qp[1];
#endif
	U3 split_mode[1];
	U8 tag[1];
};
#endif // STORE_AXIS_ADAPTER

struct PU2Store
{
	LineBuffer line_buffer;
	CuDataPtr cu_data_ptr;
	s64 cost_temp;

	ENC_CU_DATA_ARRAY_SIZE<16> curr_cu_data_temp;
	ENC_CU_DATA_ARRAY_SIZE<32> cu_data_temp;
	ENC_CU_DATA_ARRAY_SIZE<64> curr_cu_data_temp_64;
	ENC_CU_DATA_ARRAY curr_cu_data_temp_64x64;
	U2 cons_pred_mode_child;
	U2 cons_pred_mode;
	U2 tree_status_child;
	ENC_ME_NEB_INFO up_nebs_32[10];
	ENC_ME_NEB_INFO up_nebs_64[1 + (64 >> 2) + 1];
	ENC_ME_NEB_INFO left_nebs_32[8];
	ENC_ME_NEB_INFO left_nebs_64[(64 >> 2)];
	ENC_ME_LINE_MAP_SIZE<32> me_line_map_ptr_32;
	ENC_ME_LINE_MAP_SIZE<64> me_line_map_ptr_64;
	IP_ARRAY_SIZE<32, 32> IP_buffer_ptr_32x32;
	IP_ARRAY_SIZE<32, 64> IP_buffer_ptr_32x64;
	IP_ARRAY_SIZE<64, 32> IP_buffer_ptr_64x32;
	IP_ARRAY_SIZE<64, 64> IP_buffer_ptr_64x64;
	MPM_ARRAY_SIZE<32, 32> MPM_buffer_ptr_32x32;
	MPM_ARRAY_SIZE<32, 64> MPM_buffer_ptr_32x64;
	MPM_ARRAY_SIZE<64, 32> MPM_buffer_ptr_64x32;
	MPM_ARRAY_SIZE<64, 64> MPM_buffer_ptr_64x64;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_ptr_32x32;
	IP_MAP_SCU_SIZE<32, 64> IP_map_scu_ptr_32x64;
	IP_MAP_SCU_SIZE<64, 32> IP_map_scu_ptr_64x32;
	IP_MAP_SCU_SIZE<64, 64> IP_map_scu_ptr_64x64;
	U3 split_mode;
	U8 tag;
	int split_mode_father;

	int cu_qp_group_pred_qp;//axis out
};

struct PURamOut_16x16
{
	s64 out_buf_cost_temp[1];
	U2 	out_buf_cons_pred_mode[1];
	U2 	out_buf_tree_status_child[1];
	U3 	out_buf_split_mode[1];
	U8 	out_buf_tag[1];
};

struct Output2Store
{
	LineBuffer line_buffer;
};

#if LOAD_AXIS_ADAPTER
struct Load_AXIS
{
	Load2PU pu_in_buf[PU_NUM];
	LCUInput buffer;
	Load2Bp bp_in_buf[BP_NUM];
};
#endif // LOAD_AXIS_ADAPTER

#if MD_CHANNEL_DEBUG
struct BpChn2Store
{
	LineBuffer line_buffer;
	ENC_ME_NEB_INFO up_nebs_32[10];
	ENC_ME_NEB_INFO left_nebs_32[8];
	ENC_ME_LINE_MAP_SIZE<32> me_line_map_ptr_32;
	ENC_ME_LINE_MAP_SIZE<64> me_line_map_ptr_64;
	IP_ARRAY_SIZE<32, 32> IP_buffer_ptr_32x32;
	IP_ARRAY_SIZE<32, 64> IP_buffer_ptr_32x64;
	IP_ARRAY_SIZE<64, 32> IP_buffer_ptr_64x32;
	IP_ARRAY_SIZE<64, 64> IP_buffer_ptr_64x64;
	MPM_ARRAY_SIZE<32, 32> MPM_buffer_ptr_32x32;
	MPM_ARRAY_SIZE<32, 64> MPM_buffer_ptr_32x64;
	MPM_ARRAY_SIZE<64, 32> MPM_buffer_ptr_64x32;
	MPM_ARRAY_SIZE<64, 64> MPM_buffer_ptr_64x64;
	IP_MAP_SCU_SIZE<32, 32> IP_map_scu_ptr_32x32;
	IP_MAP_SCU_SIZE<32, 64> IP_map_scu_ptr_32x64;
	IP_MAP_SCU_SIZE<64, 32> IP_map_scu_ptr_64x32;
	IP_MAP_SCU_SIZE<64, 64> IP_map_scu_ptr_64x64;
	ENC_CU_DATA prev_cu_data_ptr_32x32;
	ENC_ME_NEB_INFO up_nebs_64[1 + (64 >> 2) + 1];
	ENC_ME_NEB_INFO left_nebs_64[(64 >> 2)];
	ENC_ME_NEB_INFO				nebs_line_ver_32x32[8];
	ENC_ME_NEB_INFO				nebs_line_hor_32x32[8 + 8];
	ENC_ME_NEB_INFO up_nebs_32x32[18];
	ENC_ME_NEB_INFO left_nebs_32x32[16];
};
#endif

template <typename _Insn>
struct InstructionRegister
{
	_Insn buf[IR_SIZE];
	U8 size;
	U8 offset;

	void init()
	{
		size = 0;
		offset = 0;
	}

	bool full() const
	{
		return size == IR_SIZE;
	}

	void update(_Insn* mem)
	{
		for (; size < IR_SIZE && offset < PR_SIZE; ++size, ++offset)
		{
#pragma HLS LOOP_TRIPCOUNT max=8
#pragma HLS PIPELINE
			buf[size] = mem[offset];
			if (buf[size].opcode() == DONE)
			{
				++size;
				break;
			}
		}
	}

	void push(const _Insn& insn)
	{
		assert(size != IR_SIZE);
		buf[size++] = insn;
	}

	void remove(int idx)
	{
#pragma HLS INLINE
		for (int i = 0; i < IR_SIZE - 1; ++i)
		{
#pragma HLS UNROLL
			if (i >= idx)
			{
				buf[i] = buf[i + 1];
			}
		}
		--size;
	}
};

#if BYPASS_AXIS_ADAPTER
void bypass(Load2Bp& in_buf, PU2Store& out_buf, Load2Bp_AXIS& axis_in, PU2Store_RAM& ram_out);
#else
void bypass(Load2Bp& in_buf, PU2Store& out_buf);
#endif // BYPASS_AXIS_ADAPTER

template <size_t _Kernel>
void pu(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);

#if OUTPUT_AXIS_ADAPTER
void pu_output(Load2Output& in_buf, Output2Store& out_buf, ENC_CU_DATA& cu_data, U8& axis_in, U8 ram_out_out_buf_line_buffer_y0[1]);
#else
void pu_output(Load2Output& in_buf, Output2Store& out_buf, ENC_CU_DATA& cu_data);
#endif // OUTPUT_AXIS_ADAPTER

#if MD_CHANNEL_DEBUG
void bypass_channel(BpChn2Store& out_buf, Load2BpChn& in_buf);
void output_channel(Load2OutChn& in_buf, Channel& output);
#endif

bool load_core(InstructionRegister<Instruction>& ir1, InstructionRegister<Instruction>& ir2, Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], PRMan& pu_man, hls::stream<U32>& status, Register& reg
#if MD_CHANNEL_DEBUG
	, Load2BpChn& bp_chn_in_buf, Load2OutChn& out_chn_in_buf
#endif
	, Load2PU pu_in_buf[PU_NUM]
#if MD_CHANNEL_DEBUG
	, Channel& input
#endif
	, ShareMemory& pool, LCUConfig& config, LCUInput& buffer, stream_type<ap_uint<1> > push_data[PU_NUM], Load2Bp bp_in_buf[BP_NUM], CuDataPool& cu_data, Load2Output& out_in_buf);

bool store_core(InstructionRegister<Instruction>& ir, Instruction mem_store[PR_SIZE], hls::stream<U32>& status, Register& reg, reverse_stream_type<ap_uint<1> > push_data[PU_NUM],
	LCUConfig& config
#if MD_CHANNEL_DEBUG
	, BpChn2Store& bp_chn_out_buf
#endif
	, PU2Store pu_out_buf[PU_NUM], 
#if STORE_AXIS_ADAPTER
	Store_RAM ram_in[PU_NUM],
#endif
	ShareMemory& pool, CuDataPool& cu_data, Output2Store& out_out_buf);

#if !ONLY_QT_QT
void load_line_buffer_16x16_md2(InstructionPU& insn_pu, Load2PU& in_buf, Channel& channel, InstructionLoad& insn, LCUConfig& config, LCUInput& buffer);
void store_n_1_16x16_md2(LCUConfig& config, PU2Store pu_out_buf[PU_NUM], InstructionStore& insn, Channel& channel);
void pu_16x16_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);
void pu_16x16_8x16_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);
void pu_16x16_16x8_md2(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);
void load_64x64_to_32x32(InstructionPU& insn_pu, InstructionLoad& insn,
	Load2PU& in_buf, Channel& channel,
	ENC_ME_NEB_INFO* up_nebs_father, ENC_ME_NEB_INFO* left_nebs_father,
	ENC_ME_NEB_INFO  up_nebs[1 + 16 + 1],
	ENC_ME_NEB_INFO  left_nebs[16],
	LCUConfig & config, LCUInput & buffer);
void store_32x32_to_64x64(InstructionStore& insn_store, Channel& channel, LCUConfig& config,
	s64 cost[1],
	U2 tree_status_child[1],
	U2 cons_pred_mode_child[1],
	U3 split_mode[1],
	ENC_ME_NEB_INFO                         nebs_line_ver[8],
	ENC_ME_NEB_INFO                         nebs_line_hor[8],
	//ENC_CU_DATA_ARRAY&			curr_cu_data_best,
	COM_SPLIT_STRUCT split_struct,
	ENC_CU_DATA_ARRAY_SIZE<32>	cu_data_temp[1]);
#endif


void vmod(LCUConfig& config, LCUInput& buffer, Arbitor& ref
#if MD_CHANNEL_DEBUG
			, Channel& input, Channel& output
#endif
				, Instruction mem_load[PR_SIZE], Instruction mem_pu[PR_SIZE], Instruction mem_store[PR_SIZE], ENC_CU_DATA& md_output, Register& reg);

void md_run_qtqt(LCUInput& buffer, Arbitor& ref, Instruction* mem_load, Instruction* mem_pu, Instruction* mem_store, Register& reg, FME_OUT_PARA_SET* fme_out_para_set, ENC_ME_NEB_INFO cur_ctu_mi[SCU_IN_LCU_NUMS_LEN][SCU_IN_LCU_NUMS_LEN], pel dbk_y[71][71], ENC_CU_DATA* md2_output, ENC_ME_LINE_MAP* me_line_map_ptr_input, ENC_ME_LINE_MAP* me_line_map_ptr_output);
void md_input_connection(LCUInput& buffer, Arbitor& ref, RMD_OUTPUT& g_rmd_output, ME_MV_DIR& me_mv_dir, ENC_FME_INFO_ALL& fme_mv_info, SKIP_MERGE_INPUT& skip_in, strFetch_ref_window& Fetch_Ref_window_ptr, pel p_fenc_LCU_Y[MAX_CU_SIZE_FETCH][MAX_CU_SIZE_FETCH], pel fme2mdrefbuf[2560][8][2][16]);

void asm_qtqt(int x0, int y0, int log2w0, int log2h0, int pic_w, int pic_h, int log2_culine, int min_qt_size, Instruction*& load, Instruction*& pu, Instruction*& store);
void asm_pix_xy(int* asm_pix_x, int* asm_pix_y, Slice_Level_Parameters* ctu_ctrl_input, CTX_FETCH* fetch_ctx);
void pu_KERNEL_16X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_16x16& axis_in, PURamOut_16x16& ram_out);
#if PU_8X16_AXIS_ADAPTER
void pu_KERNEL_8X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_8x16& axis_in, PURamOutPorts_8x16& axis_out);
#else
void pu_KERNEL_8X16(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);
#endif

#if PU_16X8_AXIS_ADAPTER
void pu_KERNEL_16X8(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, PUAxisInPorts_16x8& axis_in, PURamOutPorts_16x8& axis_out);
#else
void pu_KERNEL_16X8(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref);
#endif
void pu_KERNEL_8X8(Load2PU& in_buf, PU2Store& out_buf, Arbitor& ref, const PUAxisInPorts_8x8& axis_in, PURamOut_8x8& ram_out);


void md_run_64_reg(int x, int y, Register& reg);

void copy_in_buf_up(/*MapScu& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM]);
void copy_in_buf_left(/*MapScu& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM]);
void copy_in_buf_topleft(/*MapScu& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM]);
void copy_line_buffer_all(/*LineBuffer& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM]);
void copy_line_buffer_pu(/*LineBuffer& dst*/Load2PU in_buf[PU_NUM], const LineBuffer& src, bool flag_pu[PU_NUM]);
void copy_inst_PU(/*InstructionPU& _Left*/Load2PU pu_in_buf[PU_NUM], const InstructionPU _Right[PU_NUM], bool flag_pu[PU_NUM]);
void copy_lcu_config(/*LCUConfig& _Left*/Load2PU pu_in_buf[PU_NUM], const LCUConfig& _Right, bool flag_pu[PU_NUM]);
void copy_md_fw_ptr(/*_MD_FW& _Left*/Load2PU pu_in_buf[PU_NUM], const _MD_FW& _Right, bool flag_pu[PU_NUM]);
void copy_luma(/*_MD_INPUT& _Left*/Load2PU pu_in_buf[PU_NUM], const _MD_INPUT& _Right, bool flag_pu[PU_NUM]);
void copy_lcu_input(/*LCUInput& _Left*/Load2PU pu_in_buf[PU_NUM], LCUInput& _Right, bool flag_pu[PU_NUM]);
void copy_g_rmd_output(/*RMD_OUTPUT& _Left*/Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void copy_me_mv_dir(/*_ME_MV_DIR_BW& _Left*/Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void copy_fme_mv_info(/*_ENC_FME_INFO_ALL& _Left*/Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void copy_p_fenc_LCU_Y(/*_ENC_FME_INFO_ALL& _Left*/Load2PU pu_in_buf[PU_NUM], const LCUInput& _Right, bool flag_pu[PU_NUM]);
void copy_skip_merge_input(/*_SKIP_MERGE_INPUT& _Left*/Load2PU pu_in_buf[PU_NUM], _SKIP_MERGE_INPUT& _Right, bool flag_pu[PU_NUM]);
void cp_skip_in(/*_SKIP_MERGE_INPUT * dst*/Load2PU pu_in_buf[PU_NUM], _SKIP_MERGE_INPUT * src, bool flag_pu[PU_NUM]);
void copy_idx_Fetch_Ref_window_ptr(/*__strFetch_ref_window& _Left*/Load2PU pu_in_buf[PU_NUM], const  __strFetch_ref_window& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_8x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_16x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_32x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_64x8(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_8x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_16x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_32x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_64x16(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_8x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_16x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_32x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_64x32(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_8x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_16x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_32x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_RMD_64x64(Load2PU pu_in_buf[PU_NUM], const RMD_OUTPUT& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_64x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_32x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_64x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_32x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_16x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_32x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_16x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_64x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_16x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_8x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_16x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_8x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_32x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_8x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_64x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_8x64(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_32x4(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_4x32(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_16x4(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_4x16(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_8x4(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_ME_4x8(Load2PU pu_in_buf[PU_NUM], const _ME_MV_DIR_BW& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_64x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_32x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_64x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_32x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_16x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_32x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_16x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_64x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_16x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_8x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_16x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_8x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_32x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_8x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_64x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_8x64(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_4x8(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_8x4(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_16x4(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_4x16(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_32x4(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);
void COPY_FME_4x32(Load2PU pu_in_buf[PU_NUM], const _ENC_FME_INFO_ALL& _Right, bool flag_pu[PU_NUM]);