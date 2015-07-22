/**
 * Multidimensional Static Matrix C++11 
 * Copyright Emanuele Ruffaldi (2015) at Scuola Superiore Sant'Anna Pisa
 *
 * AKA extreme parameter packs
 *
 * Core functionalities ... the rest is "trivial"
 *
 * Under Apache License
 */
#pragma once
#include <type_traits>

namespace multidim
{
  // for type-values
  template <class T>
  struct type_holder
    {
        using type = T;
    };

    /// this holds the (size,step)
    template <int size, int step>
    struct sspair {
      static constexpr  int xsize = size; // for access
      static constexpr  int xstep = step; // for access
      static constexpr  int xstepsize = step*size; // for access of next
      static constexpr  bool singleton = size == 1;
    };

template <int...I> struct isumseq;

template <> struct isumseq<>
{
   static constexpr int value = 0;
};

template <int i, int...I> struct isumseq<i,I...>
{
   static constexpr int value = i + isumseq<I...>::value;
};

/// actually not used, and it can be attached to integer_sequence
template<int... N> struct iproduct;

template<>
struct iproduct<> {
    static constexpr  int value = 1;
};

template<int x, int... N>
struct iproduct<x, N...> {
    static constexpr  int value = x * iproduct<N...>::value;
};

template <int x>
using intholder = std::integral_constant<int,x>;

template <bool x>
using boolholder = std::integral_constant<bool,x>;

template<class T,T...I> struct integer_sequence;

template<class T,T...I> struct integer_sequence
{ 
  typedef T type;
  static constexpr T size = sizeof...(I);

  template<T N>
  using append = integer_sequence<T, I..., N>;

  template<T N>
  using prepend = integer_sequence<T, N, I...>;

  using next = append<sizeof...(I)>;

  static constexpr int sum = isumseq<I...>::value;

  static constexpr int product = iproduct<I...>::value;

}; 



template<class...I> struct type_sequence;

namespace details
{
  template<int looked,int current,class... N> struct saccessor;

  template<int looked, int current>
  struct saccessor<current,looked> {
      using type = void;
  };

  template<int looked, int current, class x, class... N>
  struct saccessor<looked, current, x, N...> {
      using type = typename std::conditional<looked == current, x, typename saccessor<looked,current+1,N...>::type >::type;
  };    

  template<int looked, class T> struct saccessorseqhelp;

  template<int looked, class...N>
  struct saccessorseqhelp<looked, type_sequence<N...> > {
      static constexpr  int value = saccessor<looked,0,N...>::value;
  };    

  template <int looked, class T>
  using saccessorseq = saccessorseqhelp<looked,T>;

  template <int looked, class...N>
  using saccessorlist = saccessor<looked,0,N...>;

  namespace invert_details
  {

  /// build a parameter pack with size sizeof(N)-1 removing the j-th
  template <class T, class...Is>
  struct inverter;

  // last one, when the post sequence is terminated and we have only the Is
  template <class...Pre>
  struct inverter<type_sequence<Pre...> >
    {
        using type = type_sequence<Pre...>;
    };


  template <class x, class...Pre, class...Past>
  struct inverter<type_sequence<Pre...>, x, Past...>
    : inverter<type_sequence<x,Pre...>, Past...>
    {
    };

  }


  namespace dropper_details
  {

  template<bool match, int looked,int current,class T, class...Is> struct __drophelp;

  /// build a parameter pack with size sizeof(N)-1 removing the j-th
  template <int looked, int current, class T, class...Is>
  struct dropper;

  template <int looked, int current, class...Pre, class...Past>
  struct dropper<looked,current,type_sequence<Pre...>, Past...>
    : __drophelp<looked==current,looked,current, type_sequence<Pre...>, Past...>
    {
    };

  // looked==current matches, so the rest c be taken as it is
  template<int looked, int current, class x, class...Pre, class...Past> 
  struct __drophelp<true,looked,current,type_sequence<Pre...>, x, Past...>:
     type_holder< type_sequence<Pre...,Past...> >
  {
  };

  template<int looked, int current,class x, class...Pre, class...Past> 
  struct __drophelp<false,looked,current,type_sequence<Pre...>,x,Past...>:
    dropper<looked,current+1,type_sequence<Pre...,x>,Past...>
  {
  };
  }

  namespace removeif_details
  {

  template<bool match, template <class T> class Pred , class OT, class...Is> struct __drophelp;

  /// build a parameter pack with size sizeof(N)-1 removing the j-th
  template <template <class T> class Pred, class OT, class...Is>
  struct dropper;

  /// no more to process
  template <template <class T> class Pred, class...Pre>
  struct dropper<Pred,  type_sequence<Pre...> >
    : type_holder<type_sequence<Pre...> > 
    {
    };

  template <template <class T> class Pred, class...Pre, class x, class...Past>
  struct dropper<Pred,  type_sequence<Pre...>, x, Past...>
    : __drophelp<Pred<x>::value, Pred,type_sequence<Pre...>, x, Past...>
    {
    };

  // positive means remove
  template<template <class T> class Pred,class x, class...Pre, class...Past> 
  struct __drophelp<true,Pred,type_sequence<Pre...>, x, Past...>:
     dropper<Pred,type_sequence<Pre...>,Past... >
  {
  };

  // negative remove
  template<template <class T> class Pred,class x, class...Pre, class...Past> 
  struct __drophelp<false,Pred,type_sequence<Pre...>,x,Past...>:
     dropper<Pred,type_sequence<Pre...,x>, Past... >
  {
  };

  }

  namespace replacetype_details
  {

  /// helper that, with partial specialization, deals with the looked==current case
  template<bool match, class newtype, int looked,int current,class OTS, class...Is> struct __replacehelp;

  /// replaces the dimension "looked" with size "newsize"
  /// iterates using current
  /// the output is the output OTS
  template <class newtype, int looked, int current, class OTS, class...Is>
  struct replacer;

  /// final empty in case of NO match, this is a type_holder of the output sequence
  template <class newtype,int looked, int current,class...Pre>
  struct replacer<newtype,looked,current,type_sequence<Pre...> >:
    type_holder<type_sequence<Pre...> >
    {
    };
  
  /// supports Past... with 0+ elements
  template <class newtype, int looked, int current,class...Pre, class x, class...Past>
  struct replacer<newtype,looked,current,type_sequence<Pre...>, x, Past...>
    : __replacehelp<looked==current,newtype,looked,current, type_sequence<Pre...>, x, Past...>
    {
    };

  // looked==current matches, so the rest c be taken as it is
  template<class newtype, int looked, int current, class x, class...Pre, class...Past> 
  struct __replacehelp<true,newtype,looked,current,type_sequence<Pre...>, x, Past...>
    : type_holder<type_sequence<Pre...,newtype,Past...> >
  {
  };

  // good append it
  template<class newtype,int looked, int current, class x, class...Pre, class...Past> 
  struct __replacehelp<false,newtype,looked,current,  type_sequence<Pre...>,x,Past...>
    : replacer<newtype,looked,current+1,type_sequence<Pre...,x>,Past...>
  {
  };

  }

  /// drops j-th from sequence I
  /// obtained by cascade inheritance templated
  template <template <class T> class Pred, class...Is>
  using removeif_make = typename removeif_details::dropper<Pred,  type_sequence<>,Is...>::type;

  /// drops j-th from sequence I
  /// obtained by cascade inheritance templated
  template <int index, class newtype, class...Is>
  using replacetypemake = typename replacetype_details::replacer<newtype,index,0,type_sequence<>,Is...>::type;

  /// drops j-th from sequence I
  /// obtained by cascade inheritance templated
  template <int j, class...Is>
  using droppermake = typename dropper_details::dropper<j,0,type_sequence<>, Is...>::type;

  /// drops j-th from sequence I
  /// obtained by cascade inheritance templated
  template <class...Is>
  using invertmake = typename invert_details::inverter<type_sequence<>, Is...>::type;



  template <class X, class...I> struct sumseqsub;

  template <class X> struct sumseqsub<X>
  {
     static constexpr X value = 0;
  };

  template <class X, class i, class...I> struct sumseqsub<X,i,I...>
  {
     static constexpr X value = i::value + sumseqsub<X, I...>::value;
  };

}

/// bool-holder for matching sspair<1,b> 
/// we could replace it adding a boolean flag singleton to 
template <class T> struct singletondim;

template <int a, int b> struct singletondim<sspair<a,b> >:
std::conditional<a==1,boolholder<true>,boolholder<false> > ::type
{

};

/** 
 * A sequence of types that can be manipulated
 */
template<class...I> struct type_sequence
{ 
  /// number of elements
  static constexpr int size = sizeof...(I);

  /// appends a type
  template<class T>
  using append = type_sequence<I..., T>;

  /// prepedns a type
  template<class T>
  using prepend = type_sequence<T, I...>;

  /// picks the type at looked index
  template<int looked>
  using pick = typename details::saccessor<looked,0,I...>::type;

  /// returns the list inverted
  /// Note: the template is needed to prevent the compiler compute the type
  /// unless really needed
  template <class T = void> 
  using inverted = details::invertmake<I...>;

  /// returns the list removing the index-th entry
  template <int index> 
  using drop = details::droppermake<index, I...>;

  /// returns the list removing the index-th entry
  template <int index, class newtype> 
  using replacetype = details::replacetypemake<index, newtype, I...>;

  template <template <class T> class Pred>
  using removeif = details::removeif_make<Pred,I...>; 

  /// multidim specific, here for convenience
  /// Assumes that I... is value_holder (int with ::value and it returns)
  /// the sum with type T
  template <class T> 
  using sumvalued = details::sumseqsub<T, I...>;

  /// EXERCISE: make the reduce, that allows to generalize sumvalued. This allow to compute summation of the 
  /// contained types as intholder or instead to compute the produce of dimensions
  /// 
  /// returns a value-holder type as int using Pred and initial value
  //  template <int initial, template <class T> class Pred>
  /// using intreduce = details::reduce_make<RT,Pred,I...>; 
}; 


  /// and now support functions/types that are specific:
  /// - product of the sizes to obtain overal numel
  /// - col major and row major builder
  /// - offset computation by pair-wise product of step and index at compile time
  /// - dynamic (index based) accessor to the content of the type_sequence, assuming all sspair 
  namespace details
  {

    template<class... N> struct product;

    template<>
    struct product<> {
        static constexpr  int value = 1;
    };

    template<int size, int step, class... N>
    struct product<sspair<size,step>, N...> {
        static constexpr  int value = size * product<N...>::value;
    };

    template<class T> struct producseqhelp;

    template<class...N>
    struct producseqhelp<type_sequence<N...> > {
        static constexpr  int value = product<N...>::value;
    };    

    template <class TS>
    using productseq = producseqhelp<TS>;

    /// helps creating a sspair<size,step> with row-major layout
    template <int... N>
    struct RowMajorStepper;

    template<int x>
    struct RowMajorStepper<x> {
      using atype = sspair<x, 1>; 
      using type  = type_sequence<atype>;
    };

    template<int x, int... N>
    struct RowMajorStepper<x, N...> {
      using atype = sspair<x, RowMajorStepper<N...>::atype::xstepsize >; 
      using type  = typename RowMajorStepper<N...>::type::template prepend<atype>;
    };

    /// helps creating a sspair<size,step> with row-major layout
    template <typename above, int... N>
    struct ColMajorStepper;

    template<typename above, int x>
    struct ColMajorStepper<above, x> {
      using atype = sspair<x, above::value>; 
      using type  = type_sequence<atype>;
    };

    template<typename above, int x, int... N>
    struct ColMajorStepper<above, x, N...> {
      using atype = sspair<x, above::value >; 
      using type  = typename ColMajorStepper<intholder<above::value*x>,N...>::type::template prepend<atype>;
    };

    /// building sspair<size,step> using rowmajor
    template <int...N>
    using rowmajorstepper = typename RowMajorStepper<N...>::type;

    /// building sspair<size,step> using rowmajor
    template <int...N>
    using colmajorstepper = typename ColMajorStepper<intholder<1>, N...>::type;

    /// accessor 
    template<int current,class... N> struct daccessor;

    template<int current>
    struct daccessor<current> {
        static constexpr int size(int i) { return 0; };
        static constexpr int step(int i) { return 0; };
    };

    template<int current, class x,class... N>
    struct daccessor<current, x, N...> {
        static constexpr int size(int i) { return i == current ? x::xsize : daccessor<current+1,N...>::size(i); };
        static constexpr int step(int i) { return i == current ? x::xstep : daccessor<current+1,N...>::step(i); };
    };    

    /// needed to convert from a type_sequence<TS...> to recursion
    /// via partial specialization
    template<class TS>
    struct daccessorseq;

    template<class... TS>
    struct daccessorseq<type_sequence<TS...> > {
      using type = daccessor<0,TS...>;
    };    

    /// product of a sspair by index
    template <class a, int b> struct intproductstep
    {
        static constexpr int value = a::xstep*b;  
    };

    template<class... TS> struct sspairbyindex {
        template<int ...Args2> struct byindex {
            using type = typename type_sequence<intproductstep<TS,Args2>...>::template sumvalued<int>;
        };
    };

    /// using the above template/template computes the offset as typevalue (intholder equiv)
    template<class TS, int...I>
    struct offsetcompute;

    template<class... TS,int...I>
    struct offsetcompute<type_sequence<TS...>, I...> {
      using type = typename sspairbyindex<TS...>::template byindex<I...>::type;
    };    

  }




}