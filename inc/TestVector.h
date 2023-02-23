#pragma once
#include <stdlib.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>
#include <cassert>

#if defined LINUX
#include<unistd.h>
#else
#include<io.h>
#endif //LINUX

#ifdef LINUX
#define _INLINE_VAR
#endif

static bool is_dir_exist(std::string path)
{
#if defined LINUX 
	if (access(path.c_str(), 0) == 0)return true;
	else return false;
#else
	if (_access(path.c_str(), 0) == 0)return true;
	else return false;
#endif
}

class TestVector
{
private:
	struct input_t
	{
		static constexpr char* reg = "input_reg";
		static constexpr char* mem = "input_mem_";
	};
	struct output_t
	{
		static constexpr char* reg = "output_reg";
		static constexpr char* mem = "output_mem_";
	};

public:
	static constexpr _INLINE_VAR input_t input{};
	static constexpr _INLINE_VAR output_t output{};

public:
	TestVector(const std::string& path, int ptr, int lcu, bool bin_out = false) : _path(path), _ptr(ptr), _lcu(lcu), _bin_out(bin_out)
	{
        std::cout << path << std::endl;
//        std::cout << is_dir_exist(path) << std::endl;
		if (!is_dir_exist(path))
		{
#if LINUX
            std::string cmd = "mkdir -p " + path;
#else
			std::string cmd = "mkdir " + path;
#endif
//            std::cout << cmd << std::endl;
			int ret = system(cmd.c_str());
		}
		std::ofstream ofs_input_reg = stream("input_reg", std::ios::trunc);
		ofs_input_reg.close();
		std::ofstream ofs_output_reg = stream("output_reg", std::ios::trunc);
		ofs_output_reg.close();
	}

#if 1
	template <typename dir, typename T, typename std::enable_if<std::is_arithmetic<T>::value, int>::type = 0>
	void write_reg(dir, const std::string& name, T value)
	{
//        std::cout << name << std::endl;
		std::ofstream ofs = stream(dir::reg, std::ios::app);
		ofs << name << "\t" << "0x" << std::hex << static_cast<u64>(value) << "\t" << sizeof(T) << std::endl;
		ofs.close();
	}
#else
	template <typename dir, typename T, typename T1 = int>
	void write_reg(dir, const std::string& name, T value)
	{
		//        std::cout << name << std::endl;
		std::ofstream ofs = stream(dir::reg, std::ios::app);
		ofs << name << "\t" << "0x" << std::hex << static_cast<u64>(value) << "\t" << sizeof(T) << std::endl;
		ofs.close();
	}
#endif
	template <typename dir, typename Array, typename... Dims>
	void write_mem(dir, const std::string& name, const Array value, Dims... dims)
	{
//        std::cout << name << std::endl;
		static_assert(std::is_pointer<Array>::value, "");
		std::string file_name = static_cast<std::ostringstream&&>(std::ostringstream() << dir::mem << name).str();
		std::ofstream ofs = stream(file_name, std::ios::trunc);
		size_t depth = calc_depth(dims...);
		ofs << name << "\t" << 8 * deref_arr_size_of<Array, Dims...> << "\t" << depth << std::endl;
        if(_bin_out)
        {
            _ofs_bin = stream(file_name, "bin", std::ios::trunc | std::ios::binary);
            _bytes = deref_arr_size_of<Array, Dims...>;
        }
		if (value)
		{
			print_mem(ofs, value, dims...);
		}
		else
		{
			for (size_t i = 0; i < depth; ++i)
			{
				ofs <<  "0x" << 0 << std::endl;
			}
            if(_bin_out)
            {
                assert(_bytes <= 32);
                assert(_ofs_bin);
                const char byte = 0;
                for(int i = 0; i < depth * deref_arr_size_of<Array, Dims...>; ++i)
                {
                    _ofs_bin.write(&byte, 1);
                }
            }
		}
		ofs.close();
        if(_bin_out)
        {
            _ofs_bin.close();
        }
	}
	
private:
	std::ofstream stream(const std::string& file_name, const std::string& suffix, std::ios_base::openmode mode)
	{
		std::string name = static_cast<std::ostringstream&&>(std::ostringstream() << _path << "/" << file_name << "_" << _ptr << "_" << _lcu << "." << suffix).str();
		std::ofstream ofs(name, mode);
		if (ofs.fail())
		{
			std::cout << "Open file " << name << " failed!" << std::endl;
            throw;
		}
		return ofs;
	}
	std::ofstream stream(const std::string& file_name, std::ios_base::openmode mode)
	{
		return stream(file_name, "txt", mode);
	}

	template <typename Array, typename Dim0, typename... Dims>
	void print_mem(std::ostream& os, const Array value, Dim0 dim0, Dims... dims)
	{
		static_assert(std::is_integral<Dim0>::value, "");
		assert(dim0 >= 0);
		for (int i = 0; i < dim0; ++i)
		{
//            std::cout << i <<std::endl;
			print_mem(os, value[i], dims...);
		}
	}
	template <typename T>
	void print_mem(std::ostream& os, T value)
	{
		os << "0x" << std::hex << static_cast<u64>(value) << std::endl;
        if(_bin_out)
        {
                assert(_bytes <= 32);
                assert(_ofs_bin);
            for(int i = 0; i < _bytes; ++i)
            {
//            std::cout << "width=" << _bytes << std::endl;
                const char byte = value & 0xff;
                _ofs_bin.write(&byte, 1);
                value >>= 8;
            }
        }
	}

	template <typename Dim0, typename... Dims>
	static size_t calc_depth(Dim0 dim0, Dims... dims)
	{
		static_assert(std::is_integral<Dim0>::value, "");
		return dim0 * calc_depth(dims...);
	}
	static size_t calc_depth()
	{
		return 1;
	}

	template <typename Array, typename... Dims> struct _deref_arr_size_of;
	template <typename Array, typename Dim0, typename... Dims>
	struct _deref_arr_size_of<Array, Dim0, Dims...>
	{
		static_assert(std::is_pointer<Array>::value || std::is_array<Array>::value, "");
		using type = typename std::remove_reference<decltype(**((Array*)0))>::type;
		static constexpr size_t value = _deref_arr_size_of<type, Dims...>::value;
	};
	template <typename T>
	struct _deref_arr_size_of<T>
	{
		static constexpr size_t value = sizeof(T);
	};
	template <typename Array, typename... Dims>
	static constexpr size_t deref_arr_size_of = _deref_arr_size_of<Array, Dims...>::value;

private:
	int _ptr, _lcu;
	std::string _path;
    bool _bin_out;
    std::ofstream _ofs_bin;
    size_t _bytes;
};

/*
template<typename T>
static inline void print(char* name = "", T *arr, int W, int H = 1) {
	printf("\n%s\n", name);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			printf("%d ", arr[i * W + j]);
		}
		printf("\n");
	}
}
*/