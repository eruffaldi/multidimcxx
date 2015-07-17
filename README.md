/**
Working Principle: sizes and steps, in row (lower index) major form, that, by default gives: steps[i] >= steps[i+1]. 
The access to the indices i1..iN or simply ii... is obtained by: base + ii .* steps. Several view operations
are possible, and even generalized transposition (sN...s1). Note that, when transposition is applied the condition over
steps is not more true.

Given the above we describe every multidim matrix (static or not) by:
- content (if it cannot be offsetted)
- offset 
- sizes...
- steps...

For the application to BN and specifically discrete distributions we need:

- expand(A,B,ii...) -> B
	Given a matrix A, whose dimensioni is smaller than B, has common dimensions with B, that is
		max(ii...) < numdim(B)
		numdims(A) == sizeof(ii...)
	The content of A is replicated over the non common dimensions
- scalar operations: sum
- optional summation along one or more dimensions sum(A, ii...) -> B where the result B
		max(ii...) < numdim(A)
		numdim(B) = numdim(A) - sizeof(ii...)

For general use the following operations:
- filter1<i>(B, j) => filters out the dimension i of B taking the j-th element

Emanuele Ruffaldi @SSSA 2015

------
Existing

- boost multidimensional
- http://cpptruths.blogspot.be/2011/10/multi-dimensional-arrays-in-c11.html
*/