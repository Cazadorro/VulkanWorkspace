//
// Created by Shae Bolt on 8/23/2021.
//

#ifndef VULKANWORKSPACE_REPEAT_MACRO_H
#define VULKANWORKSPACE_REPEAT_MACRO_H
//https://stackoverflow.com/a/8556436/
#define VUL_EXPAND(x) x
#define VUL_EXPAND_VA(...) __VA_ARGS__

#define VUL_EMPTY()
#define VUL_DEFER(x) x VUL_EMPTY()
#define VUL_OBSTRUCT(...) __VA_ARGS__ VUL_DEFER(VUL_EMPTY)()

#define VUL_REP0(X)
#define VUL_REP1(X) X
#define VUL_REP2(X) VUL_REP1(VUL_OBSTRUCT(X)) X
#define VUL_REP3(X) VUL_REP2(VUL_OBSTRUCT(X)) X
#define VUL_REP4(X) VUL_REP3(VUL_OBSTRUCT(X)) X
#define VUL_REP5(X) VUL_REP4(VUL_OBSTRUCT(X)) X
#define VUL_REP6(X) VUL_REP5(VUL_OBSTRUCT(X)) X
#define VUL_REP7(X) VUL_REP6(VUL_OBSTRUCT(X)) X
#define VUL_REP8(X) VUL_REP7(VUL_OBSTRUCT(X)) X
#define VUL_REP9(X) VUL_REP8(VUL_OBSTRUCT(X)) X
#define VUL_REP10(X) VUL_REP9(VUL_OBSTRUCT(X)) X

#define VUL_REP(HUNDREDS,TENS,ONES,X) \
    VUL_REP##HUNDREDS(VUL_REP10(VUL_REP10(X))) \
    VUL_REP##TENS(VUL_REP10(X)) \
    VUL_REP##ONES(X)

#define VUL_COMMA ,
#define VUL_COMMA_REP0(X)
#define VUL_COMMA_REP1(X) X VUL_COMMA
#define VUL_COMMA_REP2(X) VUL_COMMA_REP1(X) X VUL_COMMA
#define VUL_COMMA_REP3(X) VUL_COMMA_REP2(X) X VUL_COMMA
#define VUL_COMMA_REP4(X) VUL_COMMA_REP3(X) X VUL_COMMA
#define VUL_COMMA_REP5(X) VUL_COMMA_REP4(X) X VUL_COMMA
#define VUL_COMMA_REP6(X) VUL_COMMA_REP5(X) X VUL_COMMA
#define VUL_COMMA_REP7(X) VUL_COMMA_REP6(X) X VUL_COMMA
#define VUL_COMMA_REP8(X) VUL_COMMA_REP7(X) X VUL_COMMA
#define VUL_COMMA_REP9(X) VUL_COMMA_REP8(X) X VUL_COMMA
#define VUL_COMMA_REP10(X) VUL_COMMA_REP9(X) X VUL_COMMA

#define VUL_COMMA_REP(HUNDREDS,TENS,ONES,X) \
    VUL_REP##HUNDREDS(VUL_REP10(VUL_COMMA_REP10(X))) \
    VUL_REP##TENS(VUL_COMMA_REP10(X)) \
    VUL_COMMA_REP##ONES(X)

#define VUL_COMMA_REP_PLUS(HUNDREDS,TENS,ONES,X) \
    VUL_COMMA_REP(HUNDREDS,TENS,ONES,X) \
    X

#define VUL_COMMA_NUM_REP0(Y, X)
#define VUL_COMMA_NUM_REP1(Y, X) X##Y##0 VUL_COMMA
#define VUL_COMMA_NUM_REP2(Y, X) VUL_COMMA_NUM_REP1(Y, X) X##Y##1 VUL_COMMA
#define VUL_COMMA_NUM_REP3(Y, X) VUL_COMMA_NUM_REP2(Y, X) X##Y##2 VUL_COMMA
#define VUL_COMMA_NUM_REP4(Y, X) VUL_COMMA_NUM_REP3(Y, X) X##Y##3 VUL_COMMA
#define VUL_COMMA_NUM_REP5(Y, X) VUL_COMMA_NUM_REP4(Y, X) X##Y##4 VUL_COMMA
#define VUL_COMMA_NUM_REP6(Y, X) VUL_COMMA_NUM_REP5(Y, X) X##Y##5 VUL_COMMA
#define VUL_COMMA_NUM_REP7(Y, X) VUL_COMMA_NUM_REP6(Y, X) X##Y##6 VUL_COMMA
#define VUL_COMMA_NUM_REP8(Y, X) VUL_COMMA_NUM_REP7(Y, X) X##Y##7 VUL_COMMA
#define VUL_COMMA_NUM_REP9(Y, X) VUL_COMMA_NUM_REP8(Y, X) X##Y##8 VUL_COMMA
#define VUL_COMMA_NUM_REP10(Y, X) VUL_COMMA_NUM_REP9(Y, X) X##Y##9 VUL_COMMA


#define VUL_COMMA_NUM_FILLER_REP0(MACRO_FUNC, Y, X)
#define VUL_COMMA_NUM_FILLER_REP1(MACRO_FUNC, Y, X) MACRO_FUNC(Y##0,X)
#define VUL_COMMA_NUM_FILLER_REP2(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP1(MACRO_FUNC, Y, X) MACRO_FUNC(Y##1,X)
#define VUL_COMMA_NUM_FILLER_REP3(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP2(MACRO_FUNC, Y, X) MACRO_FUNC(Y##2,X)
#define VUL_COMMA_NUM_FILLER_REP4(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP3(MACRO_FUNC, Y, X) MACRO_FUNC(Y##3,X)
#define VUL_COMMA_NUM_FILLER_REP5(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP4(MACRO_FUNC, Y, X) MACRO_FUNC(Y##4,X)
#define VUL_COMMA_NUM_FILLER_REP6(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP5(MACRO_FUNC, Y, X) MACRO_FUNC(Y##5,X)
#define VUL_COMMA_NUM_FILLER_REP7(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP6(MACRO_FUNC, Y, X) MACRO_FUNC(Y##6,X)
#define VUL_COMMA_NUM_FILLER_REP8(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP7(MACRO_FUNC, Y, X) MACRO_FUNC(Y##7,X)
#define VUL_COMMA_NUM_FILLER_REP9(MACRO_FUNC, Y, X) VUL_COMMA_NUM_FILLER_REP8(MACRO_FUNC, Y, X) MACRO_FUNC(Y##8,X)
#define VUL_COMMA_NUM_FILLER_REP10(MACRO_FUNC,Y, X) VUL_COMMA_NUM_FILLER_REP9(MACRO_FUNC, Y, X) MACRO_FUNC(Y##9,X)

#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP0(MACRO_FUNC0, MACRO_FUNC1, X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP1(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,0,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP2(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP1(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,1,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP3(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP2(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,2,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP4(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP3(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,3,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP5(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP4(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,4,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP6(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP5(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,5,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP7(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP6(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,6,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP8(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP7(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,7,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP9(MACRO_FUNC0, MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP8(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,8,X)
#define  VUL_COMMA_NUM_FILLER_HUNDREDS_REP10(MACRO_FUNC0,MACRO_FUNC1, X) VUL_COMMA_NUM_FILLER_HUNDREDS_REP9(MACRO_FUNC0, MACRO_FUNC1, X) MACRO_FUNC0(MACRO_FUNC1,9,X)

#define VUL_COMMA_NUM_FILLER_REP(HUNDREDS,TENS,ONES,X) \
    VUL_COMMA_NUM_FILLER_HUNDREDS_REP##HUNDREDS(VUL_COMMA_NUM_FILLER_REP10, VUL_COMMA_NUM_REP10, X) \
    VUL_COMMA_NUM_FILLER_REP##TENS(VUL_COMMA_NUM_REP10, HUNDREDS, X) \
    VUL_COMMA_NUM_REP##ONES(HUNDREDS##TENS, X)

#define VUL_COMMA_NUM_FILLER_REP_PLUS(HUNDREDS,TENS,ONES,X) \
    VUL_COMMA_NUM_FILLER_REP(HUNDREDS,TENS,ONES,X) \
    X##HUNDREDS##TENS##ONES

#define VUL_REPEAT_WITH_NUM_ONES_REP0(HUNDREDS,TENS,MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP1(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,0)
#define VUL_REPEAT_WITH_NUM_ONES_REP2(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,1) VUL_REPEAT_WITH_NUM_ONES_REP1(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP3(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,2) VUL_REPEAT_WITH_NUM_ONES_REP2(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP4(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,3) VUL_REPEAT_WITH_NUM_ONES_REP3(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP5(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,4) VUL_REPEAT_WITH_NUM_ONES_REP4(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP6(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,5) VUL_REPEAT_WITH_NUM_ONES_REP5(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP7(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,6) VUL_REPEAT_WITH_NUM_ONES_REP6(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP8(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,7) VUL_REPEAT_WITH_NUM_ONES_REP7(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP9(HUNDREDS,TENS,MACRO_FUNC)  MACRO_FUNC(HUNDREDS,TENS,8) VUL_REPEAT_WITH_NUM_ONES_REP8(HUNDREDS,TENS, MACRO_FUNC)
#define VUL_REPEAT_WITH_NUM_ONES_REP10(HUNDREDS,TENS,MACRO_FUNC) MACRO_FUNC(HUNDREDS,TENS,9) VUL_REPEAT_WITH_NUM_ONES_REP9(HUNDREDS,TENS, MACRO_FUNC)

#define VUL_REPEAT_WITH_NUM_TENS_REP0(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP1(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,0,MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP2(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,1,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP1(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP3(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,2,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP2(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP4(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,3,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP3(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP5(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,4,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP4(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP6(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,5,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP5(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP7(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,6,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP6(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP8(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,7,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP7(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP9(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)   MACRO_FUNC0(HUNDREDS,8,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP8(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)
#define VUL_REPEAT_WITH_NUM_TENS_REP10(HUNDREDS, MACRO_FUNC0, MACRO_FUNC1)  MACRO_FUNC0(HUNDREDS,9,MACRO_FUNC1) VUL_REPEAT_WITH_NUM_TENS_REP9(HUNDREDS,MACRO_FUNC0, MACRO_FUNC1)

#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP0(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP1(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(0,MACRO_FUNC1,MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP2(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(1,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP1(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP3(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(2,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP2(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP4(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(3,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP3(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP5(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(4,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP4(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP6(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(5,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP5(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP7(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(6,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP6(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP8(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(7,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP7(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP9(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)   MACRO_FUNC0(8,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP8(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)
#define VUL_REPEAT_WITH_NUM_HUNDREDS_REP10(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)  MACRO_FUNC0(9,MACRO_FUNC1,MACRO_FUNC2) VUL_REPEAT_WITH_NUM_HUNDREDS_REP9(MACRO_FUNC0, MACRO_FUNC1, MACRO_FUNC2)

#define VUL_REPEAT_WITH_NUM(HUNDREDS,TENS,ONES,MACRO_FUNC) \
    VUL_REPEAT_WITH_NUM_ONES_REP##ONES(HUNDREDS,TENS,MACRO_FUNC) \
    VUL_REPEAT_WITH_NUM_TENS_REP##TENS(HUNDREDS,VUL_REPEAT_WITH_NUM_ONES_REP10,MACRO_FUNC) \
    VUL_REPEAT_WITH_NUM_HUNDREDS_REP##HUNDREDS(VUL_REPEAT_WITH_NUM_TENS_REP10,VUL_REPEAT_WITH_NUM_ONES_REP10,MACRO_FUNC) \


#endif //VULKANWORKSPACE_REPEAT_MACRO_H
