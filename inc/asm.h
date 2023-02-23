#pragma once
#include <vector>
#include <map>
#include "md_cmod.h"

class Disassembler
{
public:
	Disassembler(Instruction& inst) : inst(inst) {}

	std::string print() const
	{
		std::string version = "(version " + to_string(inst.version()) + ")";
		switch (inst.opcode())
		{
		case LOAD_1_N: return version + "LOAD_1_N\t\t" + load_1_n();
		case LOAD_1_BYPASS: return version + "LOAD_1_BYPASS\t" + load_1_bypass();
		case LOAD_1_OUTPUT: return version + "LOAD_1_OUTPUT\t\t" + load_1_output();
		case STORE_N_1: return version + "STORE_N_1\t\t" + store_n_1();
		case KERNEL_16X16: return version + "KERNEL_16X16" + tag_pu() + "\t" + kernel();
		case KERNEL_8X16: return version + "KERNEL_8X16" + tag_pu() + "\t" + kernel();
		case KERNEL_16X8: return version + "KERNEL_16X8" + tag_pu() + "\t" + kernel();
		case KERNEL_8X8: return version + "KERNEL_8X8" + tag_pu() + "\t" + kernel();
		case BYPASS: return version + "BYPASS" + to_string(inst.opcode()) + tag_pu() + "\t\t" + bypass();
		case PU_OUTPUT: return version + "PU_OUTPUT" + tag_pu();
		case INIT: return version + "INIT\t\t\t" + init();
		case UPDATE: return version + "UPDATE\t\t" + update();
		case DONE: return version + "DONE";
		default: assert(false);
		}
	}

private:
	std::string load_1_n() const
	{
		return "CHN" + chn() + ", " + pu();
	}
	std::string load_1_bypass() const
	{
		return "CHN" + chn() + ", " + bp();
	}
	std::string load_1_output() const
	{
		return "CHN" + chn();
	}
	std::string store_n_1() const
	{
		return "ADDR" + addr() + ", CHN" + chn() + ", CU" + cu() + "," + pu();
	}
	std::string kernel() const
	{
		return "CU" + cu() + ", DEPTH" + depth() + ", " + cu_mode();
	}
	std::string bypass() const
	{
		return "CU" + cu() + ", DEPTH" + depth() + ", " + split_mode();
	}
	std::string init() const
	{
		return "CHN" + chn() + ", CU" + cu();
	}
	std::string update() const
	{
		return "CHN" + chn() + ", CU" + cu() + ", " + pu();
	}

	std::string addr() const
	{
		return to_string(inst.mem_ptr());
	}
	std::string chn() const
	{
		return std::to_string(enc_32_5(inst.flag_chn())) + "(" + to_string(inst.tag_chn()) + ")";
	}
	std::string cu() const
	{
		return to_string(inst.x0()) + " " + to_string(inst.y0()) + " " + to_string(inst.cu_width_log2()) + " " + to_string(inst.cu_height_log2());
	}
	std::string pu() const
	{
		std::string res = bp();
		for (int i = BP_NUM + 1 + BYPASS; i < PU_NUM; ++i)
		{
			if (inst.flag_pu(i))
			{
				res += " PU" + std::to_string(i - BP_NUM - BYPASS);
			}
		}
		return res + tag_pu();
	}
	std::string bp() const
	{
		std::string res;
		for (int i = BYPASS; i < BP_NUM; ++i)
		{
			if (inst.flag_pu(i))
			{
				res += " BP" + std::to_string(i);
			}
		}
		return res + tag_pu();
	}
	std::string tag_pu() const
	{
		return "(" + to_string(inst.tag_pu()) + ")";
	}
	std::string cu_mode() const
	{
		return split_mode() + "\t" + pred_mode() + "\t" + tree_status();
	}
	std::string depth() const
	{
		return to_string(inst.cup()) + " " + to_string(inst.cud()) + " " + to_string(inst.qtd()) + " " + to_string(inst.btd());
	}
	std::string split_mode() const
	{
		switch (inst.split_mode())
		{
		case SPLIT_QUAD: return "SPLIT_QUAD";
		case SPLIT_BI_HOR: return "SPLIT_BI_HOR";
		case SPLIT_BI_VER: return "SPLIT_BI_VER";
		case NO_SPLIT: return "NO_SPLIT";
		default: assert(false);
		}
	}
	std::string pred_mode() const
	{
		switch (inst.pred_mode())
		{
		case NO_MODE_CONS: return "NO_MODE_CONS";
		default: assert(false);
		}
	}
	std::string tree_status() const
	{
		switch (inst.tree_status())
		{
		case TREE_LC: return "TREE_LC";
		default: assert(false);
		}
	}

	static std::string to_string(Instruction::InsnRef ap)
	{
		return std::to_string(ap.to_int());
	}

private:
	Instruction inst;
};

struct NodeInfo
{
	int pu;
	SPLIT_MODE split;
	U4 cud;
	U3 qt_depth;
	U3 bet_depth;
	U3 cu_width_log2;
	U3 cu_height_log2;
};

class Assembler
{
public:
	Assembler& load_kernel(int idx_chn, U8 tag_chn, U8 tag_pu, U2 cons_pred_mode, U12 cup, U2 tree_status, U14 x0, U14 y0, const std::vector<NodeInfo>& nodes)
	{
		std::map<int, bool> pus;
		for (const auto& node : nodes)
		{
			pus[node.pu] = true;
		}

		load(idx_chn, tag_chn, pus);

		for (const auto& node : nodes)
		{
			kernel(1, node.pu, tag_pu, node.split, node.bet_depth, cons_pred_mode, node.cud, cup, node.cu_width_log2, node.cu_height_log2, node.qt_depth, tree_status, x0, y0);
		}

		return *this;
	}

	Assembler& load(int idx_chn, U8 tag_chn, const std::map<int, bool>& pus)
	{
		Instruction inst;
		int pu_n = 0;

		inst.opcode() = LOAD_1_N;
		assert(idx_chn < CHN_NUM);
		inst.flag_chn(idx_chn) = 1;
		inst.tag_chn() = tag_chn;
		for (const auto& pu : pus)
		{
			assert(pu.first >= BYPASS + BP_NUM && pu.first < BYPASS + BP_NUM + KERNEL_NUM);
			pu_n += inst.flag_pu(pu.first) = pu.second ? 1 : 0;
		}

		if (pu_n)
		{
			mem_load.push_back(inst);
		}
		return *this;
	}

	Assembler& kernel(bool pu_en, U8 idx_kernel, U8 tag_pu, U3 split_mode, U3 bet_depth, U2 cons_pred_mode, U4 cud, U12 cup, U3 cu_width_log2, U3 cu_height_log2, U3 qt_depth, U2 tree_status, U14 x0, U14 y0)
	{
		if (pu_en)
		{
			Instruction inst;

			assert(idx_kernel >= BYPASS + BP_NUM && idx_kernel < BYPASS + BP_NUM + KERNEL_NUM);
			inst.opcode() = idx_kernel;
			inst.x0() = x0;
			inst.y0() = y0;
			inst.cu_height_log2() = cu_height_log2;
			inst.cu_width_log2() = cu_width_log2;
			inst.split_mode() = split_mode;
			inst.pred_mode() = cons_pred_mode;
			inst.tree_status() = tree_status;
			inst.cup() = cup;
			inst.cud() = cud;
			inst.qtd() = qt_depth;
			inst.btd() = bet_depth;
			inst.tag_pu() = tag_pu;

			mem_pu.push_back(inst);
		}
		return *this;
	}

	Assembler& compare(int idx_chn, U8 mem_ptr, U8 tag_chn, U8 tag_pu, U14 x0, U14 y0, const NodeInfo& node_out, const std::vector<NodeInfo>& node_in)
	{
		std::map<int, bool> pus;
		for (const auto& node : node_in)
		{
			pus[node.pu] = true;
		}
		return compare(idx_chn, mem_ptr, tag_chn, tag_pu, x0, y0, node_out.cud, node_out.cu_width_log2, node_out.cu_height_log2, pus);
	}

	Assembler& compare(int idx_chn, U8 mem_ptr, U8 tag_chn, U8 tag_pu, U14 x0, U14 y0, U4 cud, U3 cu_width_log2, U3 cu_height_log2, const std::map<int, bool>& pus)
	{
		Instruction inst;

		inst.opcode() = STORE_N_1;
		inst.mem_ptr() = mem_ptr;
		inst.x0() = x0;
		inst.y0() = y0;
		inst.cu_height_log2() = cu_height_log2;
		inst.cu_width_log2() = cu_width_log2;
		for (const auto& pu : pus)
		{
			assert(pu.first >= BYPASS && pu.first < BYPASS + BP_NUM + KERNEL_NUM);
			inst.flag_pu(pu.first) = pu.second ? 1 : 0;
		}
		assert(idx_chn < CHN_NUM);
		inst.flag_chn(idx_chn) = 1;
		inst.tag_pu() = tag_pu;
		inst.tag_chn() = tag_chn;
		inst.cud() = cud;

		mem_store.push_back(inst);
		return *this;
	}

	Assembler& bypass(int idx_chn, int idx_bp, U8 tag_chn, U8 tag_pu, U14 x0, U14 y0, NodeInfo& node)
	{
		return bypass(idx_chn, idx_bp, tag_chn, tag_pu, x0, y0, node.cu_width_log2, node.cu_height_log2, node.cud, node.bet_depth, node.qt_depth, node.split);
	}

	Assembler& bypass(int idx_chn, int idx_bp, U8 tag_chn, U8 tag_pu, U14 x0, U14 y0, U3 cu_width_log2, U3 cu_height_log2, U4 cud, U3 bet_depth, U3 qt_depth, U3 split_mode)
	{
		Instruction load, bypass;

		load.version() = 2;
		load.opcode() = LOAD_1_BYPASS;
		assert(idx_chn < CHN_NUM);
		load.flag_chn(idx_chn) = 1;
		load.tag_chn() = tag_chn;
		assert(idx_bp < BP_NUM);
		load.flag_pu(BYPASS + idx_bp) = 1;

		bypass.opcode() = BYPASS + idx_bp;
		bypass.cu_width_log2() = cu_width_log2;
		bypass.cu_height_log2() = cu_height_log2;
		bypass.x0() = x0;
		bypass.y0() = y0;
		bypass.cud() = cud;
		bypass.qtd() = qt_depth;
		bypass.btd() = bet_depth;
		bypass.split_mode() = split_mode;
		bypass.tag_pu() = tag_pu;

		mem_load.push_back(load);
		mem_pu.push_back(bypass);
		return *this;
	}

	Assembler& init(int idx_chn, U8 tag_chn, U14 x0, U14 y0)
	{
		Instruction inst;

		inst.opcode() = INIT;
		assert(idx_chn < CHN_NUM);
		inst.flag_chn(idx_chn) = 1;
		inst.tag_chn() = tag_chn;
		inst.x0() = x0;
		inst.y0() = y0;

		mem_store.push_back(inst);
		return *this;
	}
	
	Assembler& output(int idx_chn, U8 tag_chn, U14 x0, U14 y0)
	{
		Instruction load, out, update;

		load.opcode() = LOAD_1_OUTPUT;
		assert(idx_chn < CHN_NUM);
		load.flag_chn(idx_chn) = 1;
		load.tag_chn() = tag_chn;
		load.flag_pu(PU_OUTPUT) = 1;

		out.opcode() = PU_OUTPUT;
		out.tag_pu() = 0;

		update.opcode() = UPDATE;
		update.x0() = x0;
		update.y0() = y0;
		update.flag_pu(PU_OUTPUT) = 1;
		update.flag_chn(0) = 1;
		update.tag_pu() = 0;

		mem_load.push_back(load);
		mem_pu.push_back(out);
		mem_store.push_back(update);
		return *this;
	}

	Assembler& done()
	{
		Instruction inst;

		inst.opcode() = DONE;

		mem_load.push_back(inst);
		mem_store.push_back(inst);
		return *this;
	}

	void compile(Instruction*& load, Instruction*& pu, Instruction*& store)
	{
		if (!(mem_load[0].opcode() == DONE && mem_store[0].opcode() == DONE))
		{
			assert(mem_load.size() + 1 == mem_store.size());

			assert(mem_store[0].opcode() == INIT);
			if (mem_store[1].opcode() != DONE)
			{
				for (size_t i = 0, i_pu = 0; i < mem_load.size() - 1; ++i)
				{
					assert(mem_load[i].tag_chn() == mem_store[i].tag_chn());
					assert(mem_load[i].flag_chn() == mem_store[i].flag_chn());

					std::vector<int> pus;
					for (int j = 0; j < PU_NUM; ++j)
					{
						if (mem_load[i].flag_pu(j))
						{
							pus.push_back(j);
						}
					}
					while (pus.size())
					{
						bool flag = false;
						assert(i_pu < mem_pu.size());
						for (auto it = pus.begin(), end = pus.end(); it < end; ++it)
						{
							if (*it == mem_pu[i_pu].opcode())
							{
								if (mem_store[i + 1].tag_pu() != mem_pu[i_pu].tag_pu())
								{
									std::cout << "store[" << i + 1 << "].tag_pu != pu[" << i_pu << "].tag_pu (" << mem_store[i + 1].tag_pu() << "!=" << mem_pu[i_pu].tag_pu() << ")" << std::endl;
									std::cout << "store[" << i + 1 << "]\t" << Disassembler(mem_store[i + 1]).print() << std::endl;
									std::cout << "pu[" << i_pu << "]\t\t" << Disassembler(mem_pu[i_pu]).print() << std::endl;
									assert(false);
								}
								pus.erase(it);
								++i_pu;
								flag = true;
								break;
							}
						}
						assert(flag);
					}
				}
				assert(mem_load.back().opcode() == DONE);
				assert((mem_store.end() - 2)->opcode() == UPDATE);
				assert(mem_store.back().opcode() == DONE);
			}
		}

		load = new Instruction[PR_SIZE];
		pu = new Instruction[PR_SIZE];
		store = new Instruction[PR_SIZE];

		//load[0] = mem_load.size();
		//pu[0] = mem_pu.size();
		//store[0] = mem_store.size();

		for (int i = 0; i < mem_load.size(); ++i)
		{
			load[i] = mem_load[i];
#if 0
			std::cout << "load[" << i << "]:\t" << Disassembler(load[i]).print() << std::endl;
#endif
		}
		for (int i = 0; i < mem_pu.size(); ++i)
		{
			pu[i] = mem_pu[i];
#if 0
			std::cout << "pu[" << i << "]:\t\t" << Disassembler(pu[i]).print() << std::endl;
#endif
		}
		for (int i = 0; i < mem_store.size(); ++i)
		{
			store[i] = mem_store[i];
#if 0
			std::cout << "store[" << i << "]:\t" << Disassembler(store[i]).print() << std::endl;
#endif
		}
	}

private:
	std::vector<Instruction> mem_load;
	std::vector<Instruction> mem_pu;
	std::vector<Instruction> mem_store;
};
