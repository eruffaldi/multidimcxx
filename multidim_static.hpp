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
#include <Eigen/Dense>
#include <initializer_list>
#include <iostream>
#include <type_traits>
#include "multidim_details.hpp"

namespace multidim
{

	template <class T, class TS>
	class MultiDimNBase
	{
	public:
		using value_t = T;
		static constexpr int Ncount = TS::size;
		static constexpr int Ntot = details::productseq<TS>::value;
		using indexvector_t = Eigen::Matrix<int,Ncount,1>; // instead of std::vector<int>

		template <int i>
		using getsizetype = std::integral_constant<int,TS::template pick<i>::xsize>;

		template <int i>
		using getsteptype = std::integral_constant<int,TS::template pick<i>::xstep>;

		template <int i>
		constexpr int getssize() const { return details::saccessorseq<i,TS>::xsize; }

		/// can't move in base
		template <int i>
		constexpr int getsstep() const { return details::saccessorseq<i,TS>::xstep; }

		constexpr int getsize(int i) const { return details::daccessorseq<TS>::type::size(i); }

		constexpr int getstep(int i) const { return details::daccessorseq<TS>::type::step(i); }

		constexpr int ndims() const { return Ncount; }

		constexpr int numel() const { return Ntot; } 

		/// compile type via pack: offsetvalue<1,2,3,4>::value
		template<int... I>
		using offsetvalue = typename details::offsetcompute<TS,I...>::type;

		/// returns offset from variadic via initializer list
		template <class...X>
		constexpr int offset(X... I) const 
		{	
			static_assert(sizeof...(I) == Ncount,"wrong number of dimensions");
			return offset({I...});
		}

		/// via initializer list (bit ugly...)
		/// we loop over i Ncount because is compile time length, while for(int x: L) IS NOT
		int offset(const std::initializer_list<int> & L) const 
		{	
			assert(L.size() == Ncount); //,"wrong number of dims");
			auto x = L.begin(); /// C++14 constexpr
			int o = 0;
			for(int i = 0; i < Ncount; x++, i++)
				o += *x *getstep(i);
			return o;
		}
	};

	// column wise
	template <class T, class TS>
	class MultiDimNView: public MultiDimNBase<T,TS>
	{
	public:
		using base_t = MultiDimNBase<T,TS>;
		using data_t = T*;
		using map_t = Eigen::Map<Eigen::Matrix<T,base_t::Ntot,1> >;

		MultiDimNView(data_t x):  data_(x) 
		{

		}

		constexpr const T * data() const { return data_; }

		T * data() { return data_; }

		/// limit by dimension
		/// FUTURE: variant with index compile time
		template<int dim>
	 	auto limit1(int index) -> MultiDimNView<T, typename TS::template drop<dim> >
		{
			return (data()+index*  MultiDimNBase<T,TS>::template getsteptype<dim>::value ); // via implicit construction
		}

		/// for the dimension dim takes from the given index1 up to newsize elements. This is not reducing the number of dimensions
		/// FUTURE: variant with index compile time
		template<int dim, int newsize>
		auto limit1block(int index1) -> 
			MultiDimNView<T, typename TS::template replacetype<dim,sspair<newsize,  MultiDimNBase<T,TS>::template getsteptype<dim>::value   > > >
		{
			static_assert(newsize <= MultiDimNBase<T,TS>::template getsizetype<dim>::value,"sub-size cannot be larger than original");

			return data() + index1*MultiDimNBase<T,TS>::template getsteptype<dim>::value;
		}

		template <class dummy=void>
		auto squeeze() -> MultiDimNView<T, typename TS::template removeif<singletondim> >
		{
			return data();
		}

		void setOnes()
		{
			map_t(data_).setOnes();	
		}

		void setZero()
		{
			map_t(data_).setZero();	
		}

	private:
		data_t data_;
	};

	// column wise
	template <class T, class TS>
	class MultiDimN: public MultiDimNBase<T,TS>
	{
	public:
		using data_t = Eigen::Matrix<T,MultiDimNBase<T,TS>::Ntot,1>;

		MultiDimN()
		{

		}

		const T * data() const { return data_.data(); }

		T * data() { return data_.data(); }

		template<int dim>
		auto limit1(int index) -> MultiDimNView<T, typename TS::template drop<dim> >
		{
			return (data()+index*  MultiDimNBase<T,TS>::template getsteptype<dim>::value ); // via implicit construction
		}

		/// for the dimension dim takes from the given index1 up to newsize elements. This is not reducing the number of dimensions
		/// FUTURE: variant with index compile time
		template<int dim, int newsize>
		auto limit1block(int index1) -> 
			MultiDimNView<T, typename TS::template replacetype<dim,sspair<newsize,  MultiDimNBase<T,TS>::template getsteptype<dim>::value   > > >
		{
			static_assert(newsize <= MultiDimNBase<T,TS>::template getsizetype<dim>::value,"sub-size cannot be larger than original");

			return data() + index1*MultiDimNBase<T,TS>::template getsteptype<dim>::value;
		}

		template <class dummy=void>
		auto squeeze() -> MultiDimNView<T, typename TS::template removeif<singletondim> >
		{
			return data();
		}
		
		void setOnes()
		{
			data_.setOnes();	
		}

		void setZero()
		{
			data_.setZero();	
		}


	private:
		data_t data_;
	};


	/// declares a multidim with type T and given dimensions in row-major
	template <class T, int...N>
	using MultiDimNRow = MultiDimN<T, typename details::rowmajorstepper<N...> >;

	/// declares a multidim with type T and given dimensions in col-major
	template <class T, int...N>
	using MultiDimNCol = MultiDimN<T, typename details::colmajorstepper<N...> >;
}