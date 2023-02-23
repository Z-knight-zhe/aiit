#ifndef _MD_COMMON_H_
#define _MD_COMMON_H_
#include "com_def.h"
#include "enc_def.h"


#define    _MD_FUNC_NAME(name, w, h)    name##_##w##x##h
#define MD_FUNC_NAME(name, w, h)    _MD_FUNC_NAME(name, w, h)


#define _CON_NAME(a, b) a##b
#define CON_NAME(a, b) _CON_NAME(a, b)
#define _CON_NAME_R(a, b) b##a
#define CON_NAME_R(a, b) _CON_NAME_R(a, b)

#define __REPEATE_STEP(REPEAT_STEP, n, data) REPEAT_STEP(n, data)
#define _REPEATE_STEP(REPEAT_STEP, n, data) __REPEATE_STEP(REPEAT_STEP, n, data)
#define REPEAT1(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 0,  data)
#define REPEAT2(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 1,  data) REPEAT1(REPEAT_STEP, data)
#define REPEAT3(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 2,  data) REPEAT2(REPEAT_STEP, data)
#define REPEAT4(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 3,  data) REPEAT3(REPEAT_STEP, data)
#define REPEAT5(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 4,  data) REPEAT4(REPEAT_STEP, data)
#define REPEAT6(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 5,  data) REPEAT5(REPEAT_STEP, data)
#define REPEAT7(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 6,  data) REPEAT6(REPEAT_STEP, data)
#define REPEAT8(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 7,  data) REPEAT7(REPEAT_STEP, data)
#define REPEAT9(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 8,  data) REPEAT8(REPEAT_STEP, data)
#define REPEAT10(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 9,  data) REPEAT9(REPEAT_STEP, data)
#define __REPEAT(n, REPEAT_STEP, data) REPEAT##n(REPEAT_STEP, data)
#define _REPEAT(n, REPEAT_STEP, data) __REPEAT(n, REPEAT_STEP, data)
#define REPEAT(n, REPEAT_STEP, data) _REPEAT(n, REPEAT_STEP, data)

#define REPEAT_INC0(REPEAT_STEP, data)
#define REPEAT_INC1(REPEAT_STEP, data)  _REPEATE_STEP(REPEAT_STEP, 0,  data)
#define REPEAT_INC2(REPEAT_STEP, data)  REPEAT_INC1(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 1,  data)
#define REPEAT_INC3(REPEAT_STEP, data)  REPEAT_INC2(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 2,  data)
#define REPEAT_INC4(REPEAT_STEP, data)  REPEAT_INC3(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 3,  data)
#define REPEAT_INC5(REPEAT_STEP, data)  REPEAT_INC4(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 4,  data) 
#define REPEAT_INC6(REPEAT_STEP, data)  REPEAT_INC5(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 5,  data) 
#define REPEAT_INC7(REPEAT_STEP, data)  REPEAT_INC6(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 6,  data) 
#define REPEAT_INC8(REPEAT_STEP, data)  REPEAT_INC7(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 7,  data) 
#define REPEAT_INC9(REPEAT_STEP, data)  REPEAT_INC8(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 8,  data) 
#define REPEAT_INC10(REPEAT_STEP, data) REPEAT_INC9(REPEAT_STEP, data) _REPEATE_STEP(REPEAT_STEP, 9,  data) 
#define _REPEAT_INC(n, REPEAT_STEP, data) REPEAT_INC##n(REPEAT_STEP, data)
#define REPEAT_INC(n, REPEAT_STEP, data) _REPEAT_INC(n, REPEAT_STEP, data)

#define DEC_NUM1 0
#define DEC_NUM2 1
#define DEC_NUM3 2
#define DEC_NUM4 3
#define DEC_NUM5 4
#define DEC_NUM6 5
#define _DEC_NUM(n) DEC_NUM##n
#define DEC_NUM(n) _DEC_NUM(n)

#define INC_NUM0 1 
#define INC_NUM1 2 
#define INC_NUM2 3 
#define INC_NUM3 4 
#define INC_NUM4 5 
#define INC_NUM5 6 
#define INC_NUM6 7 
#define _INC_NUM(n) INC_NUM##n
#define INC_NUM(n) _INC_NUM(n)


#define ARG0(arg0, ...) arg0
#define ARG1(arg0, arg1, ...) arg1
#define ARG2(arg0, arg1, arg2, ...) arg2
#define ARG3(arg0, arg1, arg2, arg3, ...) arg3
#define ARG4(arg0, arg1, arg2, arg3, arg4, ...) arg4
#define ARG5(arg0, arg1, arg2, arg3, arg4, arg5, ...) arg5
#define ARG6(arg0, arg1, arg2, arg3, arg4, arg5, arg6, ...) arg6
#define ARG7(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, ...) arg7
#define ARG8(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, ...) arg8
#define ARG9(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, ...) arg9
#define _ARG_N(n, ...)ARG##n(##__VA_ARGS__)
#define ARG_N(n, ...) _ARG_N(n, ##__VA_ARGS__)

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

#if MD_DBG_TB
class test_mod {
public:
    test_mod():md("md"), m_flag(false)
    {
    }
    void init(const char* name)
    {
        if (!m_flag) {
            md.initFileGoldenStd("md_unused.txt", name);
            m_flag = true;
        }
    }
    Tmodule md;
private:
    bool m_flag;
};
#endif // MD_DBG_TB
extern const s8 com_tbl_log2[257];

template<const int W, const int H, typename T0, typename T1 = T0>
void copy_array_1_to_2(const T0 array0[], T1 array1[][W], const int stride = W)
{
    const T0* src = array0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            array1[i][j] = src[j];
        }
        src += stride;
    }
}
template<const int W, const int H, typename T1, typename T0 = T1>
void copy_array_2_to_1(const T1 array1[][W], T0 array0[])
{
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            array0[i * W + j] = array1[i][j];
        }
    }
}

template<const int W, const int H, typename T>
void set_array(T array[H][W], const T &v)
{
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            array[i][j] = v;
        }
    }
}
template<const int L, typename T>
void set_array(T array[], const T &v)
{
    for (int i = 0; i < L; i++) {       
            array[i] = v;
    }
}
template<typename T>
void set_array(T array[], const T &v, int x0, int y0, int width, int height, int stride)
{
    T *pos = &array[y0 * stride + x0];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            pos[j] = v;
        }
        pos += stride;
    }
}

template<const int L, typename T0, typename T1 = T0>
void copy_array_1(const T0 array0[], T1 array1[])
{
    for (int i = 0; i < L; i++) {
       
        array1[i] = array0[i];
    }
}

template<const int W_S, const int W_D, const int H_D, typename T0, typename T1 = T0>
void copy_array_2(const T0 array0[][W_S], T1 array1[][H_D])
{
    for (int i = 0; i < H_D; i++) {
        for (int j = 0; j < W_D; j++) {
            array1[i][j] = array0[i][j];
        }
    }
}

template <const int W, const int H, typename T>
void copy_to_small_array(const T src[], T dst[], const int stride = 64, u16 x = 0, u16 y = 0)
{
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            dst[W * i + j] = src[(i + y) * stride + j + x];
        }
    }
}

template <const int W, const int H, typename T>
void copy_to_big_array(T dst[], const T src[], u16 x, u16 y, const int stride)
{
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            dst[stride * (y + i) + j + x] = src[W * i + j];
        }
    }
}

int get_transform_shift(const int bit_depth, const int tr_size_log2);

//int get_transform_shift(const int bit_depth, const int tr_size_log2);

#define MD_CONV_LOG2(v)                    (com_tbl_log2[v])

#define CALL_F_FUNC_R(func, ret, width, height, W, H, ...) \
if ((width == W) && (height == H)) {\
    ret = func<W, H>(__VA_ARGS__);\
}
#define CALL_SIZE_FUNC_R(func, ret, width, height, ...) \
{\
         CALL_F_FUNC_R(func, ret, width, height, 64, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 32, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 16, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 8, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 4, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 8, 4,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 4, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 16, 4,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 4, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 32, 4,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 8, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 16, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 8, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 32, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 8, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 64, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 16, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 32, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 16, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 64, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 32, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_R(func, ret, width, height, 64, 32,##__VA_ARGS__) \
    else {\
        assert(0);\
    }\
}

#define CALL_F_FUNC_V(func, width, height, W, H, ...) \
if ((width == W) && (height == H)) {\
    func<W, H>(__VA_ARGS__);\
}

#define CALL_SIZE_FUNC_V(func, width, height, ...) \
{\
         CALL_F_FUNC_V(func, width, height, 64, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 32, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 16, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 8, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 4, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 8, 4,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 4, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 16, 4,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 4, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 32, 4,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 8, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 16, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 8, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 32, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 8, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 64, 8,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 16, 32,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 32, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 16, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 64, 16,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 32, 64,##__VA_ARGS__) \
    else CALL_F_FUNC_V(func, width, height, 64, 32,##__VA_ARGS__) \
    else {\
        assert(0);\
    }\
}


#endif //_MD_COMMON_H_