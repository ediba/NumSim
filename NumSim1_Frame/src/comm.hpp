#include "grid.hpp"
#include "typedef.hpp"
#include <mpi.h>
//------------------------------------------------------------------------------
#ifndef __COMM_HPP
#define __COMM_HPP
//------------------------------------------------------------------------------
class Communicator {
public:
	Communicator (int* argc, char*** argv);
	~Communicator ();

    const multi_index_t& ThreadIdx () const;
    const multi_index_t& ThreadDim () const;
    //id  of the curent threat _rank
    const int&	ThreadNum	() const;
    //Number of threads  _size
    const int&	ThreadCnt	() const;

    const bool&	EvenOdd		() const;

    real_t	geatherSum		(const real_t& val) const;
    real_t	geatherMin		(const real_t& val) const;
    real_t	geatherMax		(const real_t& val) const;

	void	copyBoundary	(Grid* grid) const;

	const bool isLeft () const;
	const bool isRight () const;
	const bool isTop () const;
	const bool isBottom () const;
private:
    // evtl nicht notwendig
    MPI_Comm _mpi_cart_comm;
	multi_index_t _tidx;
	multi_index_t _tdim;
	int _rank;
	int _size;
	bool _evenodd;

	bool copyLeftBoundary	(Grid* grid) const;
    bool copyRightBoundary	(Grid* grid) const;
    bool copyTopBoundary	(Grid* grid) const;
    bool copyBottomBoundary	(Grid* grid) const;
};
//------------------------------------------------------------------------------
#endif // __COMM_HPP
//------------------------------------------------------------------------------
