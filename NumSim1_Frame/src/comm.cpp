#include <mpi.h>
#include "comm.hpp"
#include "grid.hpp"
#include "iterator.hpp"
#include "typedef.hpp"

Communicator::Communicator (int* argc, char*** argv):
_tidx(), _tdim(), _mpi_cart_comm(){
    MPI_Init(argc, argv);
    // Get the rank of the current thread
    MPI_Comm_rank(MPI_COMM_WORLD, &this->_rank);
    // Get total number of threads
    MPI_Comm_size(MPI_COMM_WORLD, &this->_size);
  // compute idx and dim from rank and size
  int dims[DIM], periodic[DIM];
  for(index_t dim = 0; dim < DIM; dim++) {
    dims[dim] = 0;
    periodic[dim] = 0;
  }
  MPI_Dims_create(this->_size, (int)DIM, dims);
  for(index_t dim = 0; dim < DIM; dim++) {
    this->_tdim[dim] = dims[dim];
  }
    
    //TODO vll nicht notwendig und anders lösbar (nurkopiert)
  MPI_Cart_create(MPI_COMM_WORLD, (int)DIM, dims, periodic, 0, &this->_mpi_cart_comm);
  int coords[DIM];
  MPI_Cart_get(this->_mpi_cart_comm, (int)DIM, dims, periodic, coords);
  this->_evenodd = true;
  for(index_t dim = 0; dim < DIM; dim++) {
    this->_tidx[dim] = coords[dim];
    this->_evenodd ^= ((coords[dim] % 2) == 1);
  }

  // TODO: for testing
//   printf("Dims: %lu %lu Rank: %i Idx: %lu %lu odd: %i\n", _tdim[0], _tdim[1], _rank, _tidx[0], _tidx[1], _evenodd);
}
    Communicator::~Communicator (){
        MPI_Finalize();
    }

    const multi_index_t& Communicator::ThreadIdx () const{
        return _tidx;
    }
    const multi_index_t& Communicator::ThreadDim () const{
        return _tdim;
    }

    const int&	Communicator::ThreadNum	() const{
        return _rank;
    }
    const int&	Communicator::ThreadCnt	() const{
        return _size;
    }

    const bool&	Communicator::EvenOdd() const{
        return _evenodd;
    }

//     real_t	geatherSum		(const real_t& val) const;
//     real_t	geatherMin		(const real_t& val) const;
//     real_t	geatherMax		(const real_t& val) const;

    void Communicator::copyBoundary (Grid* grid) const{
    //TODO 
    }

    const bool Communicator::isLeft () const{
        return (_tidx[0] == 0);
    }
    const bool Communicator::isRight () const{
        return (_tidx[0] == 1);
    }
    const bool Communicator::isTop () const{
        return (_tidx[1] == 1);
    }
    const bool Communicator::isBottom () const{
        return (_tidx[1] == 0);
    }
// private:
// 	multi_index_t _tidx;
// 	multi_index_t _tdim;
// 	int _rank;
// 	int _size;
// 	bool _evenodd;
// 
//     bool copyLeftBoundary	(Grid* grid) const;
//     bool copyRightBoundary	(Grid* grid) const;
//     bool copyTopBoundary	(Grid* grid) const;
//     bool copyBottomBoundary	(Grid* grid) const;
// };