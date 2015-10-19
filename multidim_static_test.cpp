/**
 * Multidimensional Static Matrix C++11 
 * Copyright Emanuele Ruffaldi (2015) at Scuola Superiore Sant'Anna Pisa
 *
 * AKA extreme parameter packs
 *
 * Core functionalities ... the rest is "trivial"
 */
#include "multidim_static.hpp"
#include <iostream>

template <class T>
void dumpinfo(const T& x,const char * name)
{
	std::cout << name << " has ndims:" << x.ndims() << " numel:" << x.numel() << std::endl;
	std::cout << " sizes: ";
	for(int i = 0; i < x.ndims(); i++)
	{
		std::cout << x.getsize(i) << " ";
	}
	std::cout << std::endl;
	std::cout << " steps: ";
	for(int i = 0; i < x.ndims(); i++)
	{
		std::cout << x.getstep(i) << " ";
	}
	std::cout << std::endl;
}

int main(int argc, char const *argv[])
{
	using X = multidim::MultiDimNRow<double,5,6,7,8> ;
	using Y = multidim::MultiDimNCol<double,1,2,3,4> ;
	dumpinfo(X(),"byrow(2,2,3,4)");
	dumpinfo(X().limit1<0>(2),"byrow(2,2,3,4) fix dim 0 at index 2");
	dumpinfo(X().limit1<2>(2),"byrow(2,2,3,4) fix dim 2 at index 2");
	dumpinfo(X().limit1<3>(2),"byrow(2,2,3,4) fix dim 3 at index 2");
	// compile error dumpinfo(X().limit1<4>(2),"byrow(2,2,3,4) fix beyond");
	dumpinfo(Y(),"bycol(2,2,3,4)");

	dumpinfo(X().limit1block<0,3>(0),"getblock0");
	dumpinfo(X().limit1block<1,3>(0),"getblock1");
	dumpinfo(X().limit1block<2,3>(0),"getblock2"); // 2 2 1 4
	//BUG dumpinfo(X().limit1block<3,1>(1),"getblocklast");
	dumpinfo(X().limit1block<0,1>(0),"getblock0 non squeezed");
	dumpinfo(X().limit1block<0,1>(0).squeeze(),"getblock0 squeezed");
	
	X().limit1<2>(2).setZero();
	X().limit1<2>(2).setZero();
	X().setZero();

	dumpinfo(X().permutedim<3,2,1,0>(),"flip byrow(4,3,2,2)");
	
	dumpinfo(X().reshapeC<5,3,2,7,4,2>(),"reshape col major(5,3,2,7,4,2)");

	dumpinfo(X().reshapeR<5,3,2,7,4,2>(),"reshape row major(5,3,2,7,4,2)");

	dumpinfo(X().reshapeC<5,3,2,7,4,2>().reshapeR<5,3,2,7,4,2>(),"reshape row major(5,3,2,7,4,2)");
	
	// as static
	// as args
	// as initializer list
	// TBD as eigen
	std::cout << "offset " << X::offsetvalue<0,1,2,2>::value << std::endl;
	std::cout << "offset " << X().offset(0,1,2,2) << std::endl;
	//compiletime: std::cout << "offset " << X().offset(0,1,2,2,3) << std::endl;
	//runtime: std::cout << "offset " << X().offset({0,1,2,2,4}) << std::endl;

//	std::cout << "dimension 3 is " << x.getsize<3>() << std::endl;
//	std::cout << "dimension 3 is " << x.getsize(3) << std::endl;
	return 0;
}